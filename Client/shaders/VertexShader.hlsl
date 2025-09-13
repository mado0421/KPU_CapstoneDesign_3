#include "Header.hlsli"

/*========================================================================
* PackGBufferPSO
*
* - ����ϰ� ���� ��ȯ, �� ��ȯ, ���� ��ȯ ��İ�
* - position, normal, texCoord
*=======================================================================*/
VS_OUTPUT VS_PackGBuffer(VS_INPUT input)
{
    VS_OUTPUT output;

    output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position  = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.normalW   = mul(input.normal, (float3x3)gmtxGameObject);
    output.tangentW  = mul(input.tangent, (float3x3)gmtxGameObject);
    output.uv        = input.uv;

    return output;
}

/*========================================================================
* AnimatedPackGBufferPSO
*
* - ����ϰ� ���� ��ȯ, �� ��ȯ, ���� ��ȯ ��İ�
* - position, normal, texCoord
*=======================================================================*/
VS_OUTPUT VS_AnimatedWVP(VS_INPUT input)
{
    VS_OUTPUT output;

    float weights[4] = {input.weight[0], input.weight[1], input.weight[2], input.weight[3]};

    float3 normal   = normalize(input.normal);
    float3 posL     = 0;
    float3 normalL  = 0;
    float3 tangentL = 0;

    for (int i = 0; i < 4; ++i)
    {
        posL += weights[i] * mul(float4(input.position, 1.0f), gmtxAnimation[input.boneIdx[i]]).xyz;
        normalL += weights[i] * mul(normal, (float3x3)gmtxAnimation[input.boneIdx[i]]);
        tangentL += weights[i] * mul(input.tangent, (float3x3)gmtxAnimation[input.boneIdx[i]]);
    }

    output.positionW = (float3)mul(float4(posL, 1.0f), gmtxGameObject);
    output.position  = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.normalW   = mul(normalL, (float3x3)gmtxGameObject);
    output.tangentW  = mul(tangentL, (float3x3)gmtxGameObject);
    output.uv        = input.uv;

    return output;
}


/*========================================================================
* RenderShadowPSO
* VS_MulW_LV_LP
* TransformToWolrdLightViewProj ?
*
* - ���� ��ȯ �Ŀ� ���������� �� ��ȯ, ���� ��ȯ ����� ��
* - position �� �Ѱ��ָ� ��. ���̸� ���Ŵϱ�.
*=======================================================================*/
VS_OUTPUT VS_RenderSpotLightShadowObject(VS_INPUT input)
{
    VS_OUTPUT output;

    output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position  = mul(float4(output.positionW, 1.0f), gmtxLightViewProj[0]);

    return output;
}

VS_OUTPUT VS_RenderSpotLightShadowAnimatedObject(VS_INPUT input)
{
    VS_OUTPUT output;

    float weights[4] = {input.weight[0], input.weight[1], input.weight[2], input.weight[3]};

    float3 posL = 0;

    for (int i = 0; i < 4; ++i) posL += weights[i] * mul(float4(input.position, 1.0f), gmtxAnimation[input.boneIdx[i]]).xyz;

    output.positionW = (float3)mul(float4(posL, 1.0f), gmtxGameObject);
    output.position  = mul(float4(output.positionW, 1.0f), gmtxLightViewProj[0]);

    return output;
}


/*========================================================================
* RenderPointLightShadowPSO, VS_RenderDirectionalLightShadowPSO
* TransToWorldSpace �� �̸��� �ٲٴ°�?
* �ƴϸ� VS_MulW
*
* - ���� ��ȯ������ ����.
*=======================================================================*/
float4 VS_RenderPointLightShadow(VS_INPUT input) : SV_POSITION
{
    float4 result = mul(float4(input.position, 1.0f), gmtxGameObject);

    return result;
}

float4 VS_RenderPointLightShadowAnimatedObject(VS_INPUT input) : SV_POSITION
{
    float  weights[4] = {input.weight[0], input.weight[1], input.weight[2], input.weight[3]};
    float3 posL       = 0;
    for (int i = 0; i < 4; ++i) posL += weights[i] * mul(float4(input.position, 1.0f), gmtxAnimation[input.boneIdx[i]]).xyz;

    float4 result = mul(float4(posL, 1.0f), gmtxGameObject);

    return result;
}

/*========================================================================
* VS_RenderDirectionalLightShadowPSO
*
* - ���� ��ȯ������ ����.
*=======================================================================*/
float4 VS_RenderDirectionalLightShadow(VS_INPUT input) : SV_POSITION
{
    float4 result = mul(float4(input.position, 1.0f), gmtxGameObject);

    return result;
}

float4 VS_RenderDirectionalLightShadowAnimatedObject(VS_INPUT input) : SV_POSITION
{
    float  weights[4] = {input.weight[0], input.weight[1], input.weight[2], input.weight[3]};
    float3 posL       = 0;
    for (int i = 0; i < 4; ++i) posL += weights[i] * mul(float4(input.position, 1.0f), gmtxAnimation[input.boneIdx[i]]).xyz;

    float4 result = mul(float4(posL, 1.0f), gmtxGameObject);

    return result;
}

/*========================================================================
* ColorFromGBufferPSO, AddLightPSO
*
* - ��� ��ũ���� �׸� ����. ���� ��ȯ�� ����. ���� �޽� ������ �̸� ����.
* - position, uv
*=======================================================================*/
VS_OUTPUT VS_FlatScreen(VS_INPUT input)
{
    VS_OUTPUT output;
    output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position  = float4(output.positionW, 1.0f);
    output.uv        = input.uv;

    return output;
}

/*========================================================================
* VS_ParticlePSO
*=======================================================================*/
VS_PARTICLEOUTPUT VS_Particle(VS_INPUT input)
{
    VS_PARTICLEOUTPUT output;

    output.positionW = float3(gmtxGameObject._41, gmtxGameObject._42, gmtxGameObject._43);
    output.size      = gmtxGameObject._44;
    output.direction = float4(gmtxGameObject._11, gmtxGameObject._12, gmtxGameObject._13, gmtxGameObject._14);

    return output;
}


/*========================================================================
* VS_Effect
*=======================================================================*/
VS_EFFECTOUTPUT VS_Effect(VS_INPUT input)
{
    VS_EFFECTOUTPUT output;

    float3 positionW = mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position  = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv        = input.uv;

    return output;
}


/*========================================================================
* VS_Text
*=======================================================================*/
VS_TEXTOUTPUT VS_Text(VS_INPUT input)
{
    VS_TEXTOUTPUT output;

    output.positionW = float3(gmtxGameObject._32, gmtxGameObject._33, gmtxGameObject._34);

    return output;
}
