#include "Header.hlsli"

/*========================================================================
* PackGBufferPSO
*
* - 평범하게 월드 변환, 뷰 변환, 투영 변환 행렬곱
* - position, normal, texCoord
*=======================================================================*/
VS_OUTPUT VS_PackGBuffer(VS_INPUT input) {
	VS_OUTPUT output;

	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
	output.uv = input.uv;

	return output;
}

/*========================================================================
* AnimatedPackGBufferPSO
*
* - 평범하게 월드 변환, 뷰 변환, 투영 변환 행렬곱
* - position, normal, texCoord
*=======================================================================*/
VS_OUTPUT VS_AnimatedWVP(VS_INPUT input) {
	VS_OUTPUT output;

	float weights[4] = { input.weight[0], input.weight[1], input.weight[2], input.weight[3] };

	float3 normal = normalize(input.normal);
	float3 posL		= 0;
	float3 normalL	= 0;
	float3 tangentL = 0;

	for (int i = 0; i < 4; ++i) {
		posL		+= weights[i] * mul(float4(input.position, 1.0f), gmtxAnimation[input.boneIdx[i]]).xyz;
		normalL		+= weights[i] * mul(normal, (float3x3)gmtxAnimation[input.boneIdx[i]]);
		tangentL	+= weights[i] * mul(input.tangent, (float3x3)gmtxAnimation[input.boneIdx[i]]);
	}

	output.positionW	= (float3)mul(float4(posL, 1.0f), gmtxGameObject);
	output.position		= mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.normalW		= mul(normalL, (float3x3)gmtxGameObject);
	output.tangentW		= mul(tangentL, (float3x3)gmtxGameObject);
	output.uv			= input.uv;

	return output;
}


/*========================================================================
* RenderShadowPSO
* VS_MulW_LV_LP
* TransformToWolrdLightViewProj ?
*
* - 월드 변환 후에 조명공간의 뷰 변환, 투영 변환 행렬을 곱
* - position 만 넘겨주면 됨. 깊이를 쓸거니까.
*=======================================================================*/
VS_OUTPUT VS_RenderSpotLightShadowObject(VS_INPUT input)
{
	VS_OUTPUT output;

	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(float4(output.positionW, 1.0f), gmtxLightViewProj[0]);

	return output;
}
VS_OUTPUT VS_RenderSpotLightShadowAnimatedObject(VS_INPUT input)
{
	VS_OUTPUT output;

	float weights[4] = { input.weight[0], input.weight[1], input.weight[2], input.weight[3] };

	float3 posL = 0;

	for (int i = 0; i < 4; ++i) {
		posL += weights[i] * mul(float4(input.position, 1.0f), gmtxAnimation[input.boneIdx[i]]).xyz;
	}

	output.positionW = (float3)mul(float4(posL, 1.0f), gmtxGameObject);
	output.position = mul(float4(output.positionW, 1.0f), gmtxLightViewProj[0]);

	return output;
}


/*========================================================================
* RenderPointLightShadowPSO, VS_RenderDirectionalLightShadowPSO
* TransToWorldSpace 로 이름을 바꾸는게?
* 아니면 VS_MulW
*
* - 월드 변환까지만 해줌.
*=======================================================================*/
float4 VS_RenderPointLightShadow(VS_INPUT input) : SV_POSITION {

	float4 result = mul(float4(input.position, 1.0f), gmtxGameObject);

	return result;
}
float4 VS_RenderPointLightShadowAnimatedObject(VS_INPUT input) : SV_POSITION{
	float weights[4] = { input.weight[0], input.weight[1], input.weight[2], input.weight[3] };
	float3 posL = 0;
	for (int i = 0; i < 4; ++i) {
		posL += weights[i] * mul(float4(input.position, 1.0f), gmtxAnimation[input.boneIdx[i]]).xyz;
	}

	float4 result = mul(float4(posL, 1.0f), gmtxGameObject);

	return result;
}

/*========================================================================
* VS_RenderDirectionalLightShadowPSO
*
* - 월드 변환까지만 해줌.
*=======================================================================*/
float4 VS_RenderDirectionalLightShadow(VS_INPUT input) : SV_POSITION{

	float4 result = mul(float4(input.position, 1.0f), gmtxGameObject);

	return result;
}
float4 VS_RenderDirectionalLightShadowAnimatedObject(VS_INPUT input) : SV_POSITION{
	float weights[4] = { input.weight[0], input.weight[1], input.weight[2], input.weight[3] };
	float3 posL = 0;
	for (int i = 0; i < 4; ++i) {
		posL += weights[i] * mul(float4(input.position, 1.0f), gmtxAnimation[input.boneIdx[i]]).xyz;
	}

	float4 result = mul(float4(posL, 1.0f), gmtxGameObject);

	return result;
}

/*========================================================================
* ColorFromGBufferPSO, AddLightPSO
*
* - 평면 스크린을 그릴 예정. 월드 변환만 진행. 값은 메쉬 생성때 미리 저장.
* - position, uv
*=======================================================================*/
VS_OUTPUT VS_FlatScreen(VS_INPUT input)
{
	VS_OUTPUT output;
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = float4(output.positionW, 1.0f);
	output.uv = input.uv;

	return output;
}

/*========================================================================
* VS_ParticlePSO
*=======================================================================*/
VS_PARTICLEOUTPUT VS_Particle(VS_INPUT input) {
	VS_PARTICLEOUTPUT output;

	//output.positionW	= float3(0.5f, 2, 0);
	output.positionW	= float3(gmtxGameObject._41, gmtxGameObject._42, gmtxGameObject._43);
	//output.position		= float4(output.positionW, 1);
	output.size			= gmtxGameObject._44;

	return output;
}


/*========================================================================
* VS_Effect
*=======================================================================*/
VS_EFFECTOUTPUT VS_Effect(VS_INPUT input) {
	VS_EFFECTOUTPUT output;

	float3 positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return output;
}