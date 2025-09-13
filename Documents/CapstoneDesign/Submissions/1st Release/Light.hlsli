// Rf is Fresnel Factor.
// ex. water (0.02, 0.02, 0.02), gold (1.0, 0.71, 0.29)
float3 CalcSpecular(float3 Rf, float roughness, float3 cLight, float3 vToLight, float3 vNormal, float3 vToEye) {
	float3 vHalf = normalize(vToEye + vToLight);
	float3 Specular = max(dot(vToLight, vNormal), 0) * cLight;
	// Fresnel 계산 공식에서 dot(vNormal, vHalf) 값이 음수가 나올 수 있기 때문에 max를 넣어줬음.
	float3 Fresnel = Rf * ((roughness + 8) / 8) * pow(max(dot(vNormal, vHalf), 0), roughness);
	return Specular * Fresnel;
}
float3 CalcDiffuse(float3 cLight, float3 cDiff, float3 vToLight, float3 vNormal) {
	return max(dot(vToLight, vNormal), 0) * cLight * cDiff;
}
float3 CalcAmbient(float3 cLight, float3 cDiff) {
	return cLight * cDiff;
}

float CalcAttenuation(float d, float falloffStart, float falloffEnd) {
	return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}
float3 SchlickFresnel(float3 R0, float3 vNormal, float3 vLight) {
	float cosIncidentAngle = saturate(dot(vNormal, vLight));
	float f0 = 1.0f - cosIncidentAngle;
	float3 reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);

	return reflectPercent;
}

/*========================================================================
* m 과 fresnelFactor에 material 값을 넣어주면 된다.
* material은 mesh 정보 올릴 때 같이 올리거나,,, 미리 정해둔 프리셋을 두고 그걸로 셋을 하거나,,,
* material은 어디 종속된 정보값인가? 텍스처? 메쉬? 오브젝트?
* 텍스처로 올려주면 그게 roughness 맵인 듯
*=======================================================================*/
float3 BlinnPhong(float3 lightColor, float3 vToLight, float3 vNormal, float3 vToEye, float3 vDiffuseColor, float roughness) {
	const float m = roughness * 256.0f;
	float3 vHalf = normalize(vToEye + vToLight);

	float roughnessFactor = (m + 8.0f) * pow(max(dot(vHalf, vNormal), 0.0f), m) / 8.0f;
	float3 fresnelFactor = SchlickFresnel(float3(0.9f, 0.9f, 0.9f), vHalf, vToLight);

	float3 specAlbedo = fresnelFactor * roughnessFactor;
	specAlbedo = specAlbedo / (specAlbedo + 1.0f);


	return (vDiffuseColor + specAlbedo) * lightColor;
}

float CalcPointLightShadowFactor(float3 vToPixel) {
	float3 toPixelAbs = abs(vToPixel);
	float z = max(toPixelAbs.x, max(toPixelAbs.y, toPixelAbs.z));
	float depth = (gmtxProjection._m22 * z + gmtxProjection._m32) / z;
	return gtxtShadowCubeMap.SampleCmpLevelZero(gShadowSamplerState, vToPixel, depth).r;
}
float CalcSpotLightShadowFactor(float3 vWorldPos) {

	float4 temp = mul(float4(vWorldPos, 1.0f), gmtxLightViewProj[0]);
	temp /= temp.w;
	temp = mul(temp, gmtxTexture);
	return gtxtShadowMap.SampleCmpLevelZero(gShadowSamplerState, temp.xy, temp.z).r;
}
float CalcDirectionalLightShadowFactor(float3 vWorldPos) {
	float4 v = mul(float4(vWorldPos, 1.0f), gmtxView);

	float d = v.z;
	float idxCascade;

	//idxCascade = 0;

	//if (6 <= d && d < 20) idxCascade = 1;
	if (0 <= d && d < 6) idxCascade = 0;
	else if (6 <= d && d < 20) idxCascade = 1;
	else if (20 <= d && d < 60) idxCascade = 2;
	else return 1.0f;

	float4 temp = mul(float4(vWorldPos, 1.0f), gmtxLightViewProj[idxCascade]);
	temp /= temp.w;
	temp = mul(temp, gmtxTexture);
	return gtxtShadowArrayMap.SampleCmpLevelZero(gShadowSamplerState, float3(temp.xy, idxCascade), temp.z).r;
}

float3 CalcDirectionalLight(float3 vWorldPosition, float3 vNormal, float3 vToEye, float3 vDiffuseColor, float roughness) {
	float3 vToLight = -gvLightDirection;

	float ndotl = max(dot(vToLight, vNormal), 0.0f);
	float3 lightColor = gvLightColor * ndotl;

	if (gbIsShadow) {
		float shadowFactor = CalcDirectionalLightShadowFactor(vWorldPosition);
		lightColor *= shadowFactor;
	}

	return BlinnPhong(lightColor, vToLight, vNormal, vToEye, vDiffuseColor, roughness);
}
float3 CalcPointLight(float3 vPos, float3 vNormal, float3 vToEye, float3 vDiffuseColor, float roughness) {
	float3 vToLight = gvLightPosition - vPos;

	float d = length(vToLight);

	if (d > gfFalloffEnd) return float3(0, 0, 0);

	vToLight /= d;

	float ndotl = max(dot(vToLight, vNormal), 0.0f);
	float3 lightColor = gvLightColor * ndotl;

	float att = CalcAttenuation(d, gfFalloffStart, gfFalloffEnd);
	lightColor *= att;

	if (gbIsShadow) {
		float shadowFactor = CalcPointLightShadowFactor(vPos - gvLightPosition);
		lightColor *= shadowFactor;
	}

	return BlinnPhong(lightColor, vToLight, vNormal, vToEye, vDiffuseColor, roughness);
}
float3 CalcSpotLight(float3 vPos, float3 vNormal, float3 vToEye, float3 vDiffuseColor, float roughness) {
	float3 vToLight = gvLightPosition - vPos;
	float d = length(vToLight);
	if (d > gfFalloffEnd) return float3(0, 0, 0);
	vToLight /= d;

	float ndotl = max(dot(vToLight, vNormal), 0.0f);
	float3 lightColor = gvLightColor * ndotl;

	float att = CalcAttenuation(d, gfFalloffStart, gfFalloffEnd);
	lightColor *= att;

	float spotFactor = pow(max(dot(-vToLight, gvLightDirection), 0.0f), gfSpotPower);
	lightColor *= spotFactor;

	if (gbIsShadow) {
		float shadowFactor = CalcSpotLightShadowFactor(vPos);
		lightColor *= shadowFactor;
	}

	return BlinnPhong(lightColor, vToLight, vNormal, vToEye, vDiffuseColor, roughness);
}
