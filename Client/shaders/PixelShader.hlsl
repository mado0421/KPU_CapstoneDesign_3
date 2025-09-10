#include "Header.hlsli"
#include "Light.hlsli"

/*========================================================================
* PackGBufferPSO
*
* - position, normal, texCoord�� �Ѿ��.
*=======================================================================*/
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
    // Uncompress each component from [0,1] to [-1,1].
    float3 normalT = 2.0f * normalMapSample - 1.0f;

    // Build orthonormal basis.
    float3 N = unitNormalW;
    float3 T = normalize(tangentW - dot(tangentW, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

    // Transform from tangent space to world space.
    float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
}

GBuffer PS_PackGBuffer(VS_OUTPUT input)
{
    GBuffer output;
    input.normalW     = normalize(input.normalW);
    output.cColor.rgb = gtxtColorMap.Sample(gSamplerState, input.uv).rgb;
    output.cColor.a   = 1 - gtxtDepthMap.Sample(gSamplerState, input.uv).r;

    output.cNormal.rgb = NormalSampleToWorldSpace(gtxtNormalMap.Sample(gSamplerState, input.uv).rgb, input.normalW, input.tangentW) * 0.5 + 0.5;

    return output;
}

// ����Ʈ�� ��ƼŬ�� GBuffer�� ���°� �´���
// float4�� ���� ���� ��ũ���� ���ϴ°� ���� ������
float4 PS_EffectAlpha(VS_EFFECTOUTPUT input) : SV_TARGET0
{
    float4 output = gtxtColorMap.Sample(gSamplerState, input.uv);

    return output;
}

float4 PS_Particle(GS_PARTICLEOUT input) : SV_TARGET0
{
    float4 output = gtxtColorMap.Sample(gSamplerState, input.uv);

    clip(output.a - 0.01f);

    //float depth = gtxtDepthMap.Sample(gSamplerState, input.uv).r;
    //float distance = length(gvCameraPosition - input.position.xyz);

    //output.r = distance;
    //output.gba = float3(0, 0, 1);

    return output;
}

float4 PS_Text(GS_TEXTOUTPUT input) : SV_TARGET0
{
    float4 output = gtxtColorMap.Sample(gSamplerState, input.uv);

    //clip(output.a - 0.01f);

    return output;
}

/*========================================================================
* RenderShadowPSO
*
* - Depth�� �� �����̹Ƿ� PS�� ����θ� ��. ���� ����� ���� �� �ҰŴϱ�.
*=======================================================================*/
void PS_RenderShadow(VS_OUTPUT input) {}

void PS_RenderPointLightShadow(GS_OUTPUT input) {}

/*========================================================================
* ColorFromGBufferPSO, DebugColorPSO
*
* - Color�� �о�� ����. �״�� �Ѹ��� �ȴ�.
*=======================================================================*/
float4 PS_ColorFromGBuffer(VS_OUTPUT input) : SV_TARGET
{
    float3 color = gtxtColorMap.Sample(gSamplerState, input.uv).xyz;
    return float4(color, 1.0f);
}

float4 PS_ColorFromGBufferAmbient(VS_OUTPUT input) : SV_TARGET
{
    float3 vWorldPosition = WorldPosFromLinearDepth(input.uv);
    float3 vColor         = gtxtColorMap.Sample(gSamplerState, input.uv).xyz;
    float3 vNormal        = gtxtNormalMap.Sample(gSamplerState, input.uv).xyz * 2.0f - 1.0f;
    float3 vToEye         = normalize(gvCameraPosition - vWorldPosition);

    float3 color         = gtxtColorMap.Sample(gSamplerState, input.uv).xyz;
    float3 vAmbientLight = float3(0.05f, 0.05f, 0.05f);
    return float4(color * vAmbientLight, 1.0f);
}

/*========================================================================
* DebugDepthPSO
*
* - Depth�� �о�� ����. �״�� �Ѹ��� �ȴ�.
*=======================================================================*/
float4 PS_DepthFromGBuffer(VS_OUTPUT input) : SV_TARGET
{
    float depth = gtxtShadowArrayMap.Sample(gSamplerState, float3(input.uv, 0.0f)).r;
    depth       = pow(depth, 5);

    return float4(depth.xxx, 1.0f);
}

/*========================================================================
* AddLightPSO
*
* - �������� ����ϱ� ���� GBuffer�� Normal�� Depth�� �о�� �Ѵ�.
*=======================================================================*/
float4 PS_AddLight(VS_OUTPUT input) : SV_TARGET
{
    float3 vWorldPosition = WorldPosFromLinearDepth(input.uv);
    float3 vColor         = gtxtColorMap.Sample(gSamplerState, input.uv).rgb;
    float3 vNormal        = gtxtNormalMap.Sample(gSamplerState, input.uv).xyz * 2.0f - 1.0f;
    float3 vToEye         = normalize(gvCameraPosition - vWorldPosition);
    float  fRoughness     = gtxtColorMap.Sample(gSamplerState, input.uv).a;

    float3 result = float3(0.0f, 0.0f, 0.0f);

    switch (gLightType)
    {
    case 1: result += CalcPointLight(vWorldPosition, vNormal, vToEye, vColor, fRoughness);
        break;
    case 2: result += CalcSpotLight(vWorldPosition, vNormal, vToEye, vColor, fRoughness);
        break;
    case 3: result += CalcDirectionalLight(vWorldPosition, vNormal, vToEye, vColor, fRoughness);
        break;
    default: break;
    }
    return float4(result, 1.0f);
}


/*=============================================================================
* HDR
=============================================================================*/
float3 ToneMapping(float3 vColor)
{
    float MiddleGrey  = 0.229f;
    float LumWhiteSqr = 5.789f;
    float LumScale    = dot(vColor, LUM_FACTOR);

    LumScale *= MiddleGrey / gfAvgLum[0];
    LumScale = (LumScale + LumScale * LumScale / LumWhiteSqr) / (1.0f + LumScale);

    return vColor * LumScale;
}

float4 PS_HDRToneMapping(VS_OUTPUT input) : SV_TARGET
{
    float  fBloomScale = 0.9f;
    float3 vColor      = gtxtColorMap.Sample(gSamplerState, input.uv).rgb;

    vColor += fBloomScale * gtxtNormalMap.Sample(gSamplerState, input.uv).rgb;

    vColor = ToneMapping(vColor);
    return float4(vColor, 1.0f);
}
