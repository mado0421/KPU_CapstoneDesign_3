#include "Header.hlsli"

[maxvertexcount(18)]void GS_RenderPointLightShadow(triangle float4 input[3] : SV_POSITION, inout TriangleStream<GS_OUTPUT> outStream)
{
    for (uint iFace = 0; iFace < 6; iFace++)
    {
        GS_OUTPUT output;

        output.RTIndex = iFace;

        for (int v = 0; v < 3; v++)
        {
            output.pos = mul(input[v], gmtxLightViewProj[iFace]);
            outStream.Append(output);
        }
        outStream.RestartStrip();
    }
}

/*========================================================================
* Directional Light Shadow �� Cascade Shadow Map�� ����� GS
* �ϴ��� �Ÿ� �������� �ʰ� (�װɷ� �Ǵ����� �� �𸣰�����) ��� ������ʿ� ���
* 
*=======================================================================*/
[maxvertexcount(9)]void GS_RenderDirectionalLightShadow(triangle float4 input[3] : SV_POSITION, inout TriangleStream<GS_OUTPUT> outStream)
{
    for (uint iFace = 0; iFace < 3; iFace++)
    {
        GS_OUTPUT output;

        output.RTIndex = iFace;

        for (int v = 0; v < 3; v++)
        {
            output.pos = mul(input[v], gmtxLightViewProj[iFace]);
            outStream.Append(output);
        }
        outStream.RestartStrip();
    }
}

[maxvertexcount(4)]void GS_ParticleBillboard(point VS_PARTICLEOUTPUT input[1], inout TriangleStream<GS_PARTICLEOUT> outStream)
{
    float3 up, right;
    float3 look = input[0].positionW - gvCameraPosition;
    float3 pos  = input[0].positionW;

    if (input[0].direction.w)
    {
        // bilboard X
        up    = normalize(cross(look, input[0].direction));
        right = normalize(input[0].direction);
    }
    else
    {
        // bilboard O
        right = normalize(cross(look, float3(0, 1, 0)));
        up    = normalize(cross(look, right));
    }

    //Top, Right, Bottom, Left
    float3 TR = pos + input[0].size * up + input[0].size * right;
    float3 TL = pos + input[0].size * up - input[0].size * right;
    float3 BR = pos - input[0].size * up + input[0].size * right;
    float3 BL = pos - input[0].size * up - input[0].size * right;

    GS_PARTICLEOUT output[4];

    output[0].position = mul(mul(float4(BR, 1.0f), gmtxView), gmtxProjection);
    output[1].position = mul(mul(float4(TR, 1.0f), gmtxView), gmtxProjection);
    output[2].position = mul(mul(float4(BL, 1.0f), gmtxView), gmtxProjection);
    output[3].position = mul(mul(float4(TL, 1.0f), gmtxView), gmtxProjection);

    output[0].uv = float2(0, 0);
    output[1].uv = float2(0, 1);
    output[2].uv = float2(1, 0);
    output[3].uv = float2(1, 1);

    outStream.Append(output[0]);
    outStream.Append(output[1]);
    outStream.Append(output[2]);
    outStream.Append(output[3]);
}


[maxvertexcount(4)]void GS_Text(point VS_TEXTOUTPUT input[1], inout TriangleStream<GS_TEXTOUTPUT> outStream)
{
    float3 pos   = float3(gmtxGameObject._23, gmtxGameObject._33, 0);
    float3 right = float3(1, 0, 0) * (gmtxGameObject._43 / 32.0);
    float3 down  = float3(0, -1.77777, 0) * (gmtxGameObject._43 / 32.0);

    float3 TL = pos;
    float3 TR = pos + right * gmtxGameObject._31;
    float3 BL = pos + down * gmtxGameObject._41;
    float3 BR = pos + right * gmtxGameObject._31 + down * gmtxGameObject._41;

    GS_PARTICLEOUT output[4];

    output[0].position = float4(BL, 1.0f);
    output[1].position = float4(TL, 1.0f);
    output[2].position = float4(BR, 1.0f);
    output[3].position = float4(TR, 1.0f);

    output[0].uv = float2(gmtxGameObject._11, gmtxGameObject._21 + gmtxGameObject._41);
    output[1].uv = float2(gmtxGameObject._11, gmtxGameObject._21);
    output[2].uv = float2(gmtxGameObject._11 + gmtxGameObject._31, gmtxGameObject._21 + gmtxGameObject._41);
    output[3].uv = float2(gmtxGameObject._11 + gmtxGameObject._31, gmtxGameObject._21);

    for (int i = 0; i < 4; i++) output[i].uv / 256.0;

    outStream.Append(output[0]);
    outStream.Append(output[1]);
    outStream.Append(output[2]);
    outStream.Append(output[3]);
}
