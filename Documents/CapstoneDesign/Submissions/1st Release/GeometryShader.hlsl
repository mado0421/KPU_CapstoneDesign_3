#include "Header.hlsli"

[maxvertexcount(18)]
void GS_RenderPointLightShadow(
	triangle float4 input[3] : SV_POSITION, 
	inout TriangleStream< GS_OUTPUT > outStream
)
{
	for (uint iFace = 0; iFace < 6; iFace++) {
		GS_OUTPUT output;

		output.RTIndex = iFace;

		for (int v = 0; v < 3; v++) {
			output.pos = mul(input[v], gmtxLightViewProj[iFace]);
			outStream.Append(output);
		}
		outStream.RestartStrip();
	}
}

/*========================================================================
* Directional Light Shadow 의 Cascade Shadow Map에 사용할 GS
* 일단은 거리 생각하지 않고 (그걸로 되는지도 잘 모르겠지만) 모든 쉐도우맵에 출력
* 
*=======================================================================*/
[maxvertexcount(9)]
void GS_RenderDirectionalLightShadow(
	triangle float4 input[3] : SV_POSITION,
	inout TriangleStream< GS_OUTPUT > outStream
)
{
	for (uint iFace = 0; iFace < 3; iFace++) {
		GS_OUTPUT output;

		output.RTIndex = iFace;

		for (int v = 0; v < 3; v++) {
			output.pos = mul(input[v], gmtxLightViewProj[iFace]);
			outStream.Append(output);
		}
		outStream.RestartStrip();
	}
}

[maxvertexcount(4)]
void GS_ParticleBillboard(
	point VS_PARTICLEOUTPUT input[1],
	inout TriangleStream< GS_PARTICLEOUT > outStream
)
{
	float3 up, right;
	float3 look = input[0].positionW - gvCameraPosition;
	float3 pos = input[0].positionW;
	right = normalize(cross(look, float3(0, 1, 0)));
	up = normalize(cross(look, right));

	//right	= float3(1, 0, 0);
	//up		= float3(0, 1, 0);
	//pos		= float3(0, 2, -4);

	float3 TR, TL, BR, BL;	//Top, Right, Bottom, Left
	TR = pos + input[0].size * up + input[0].size * right;
	TL = pos + input[0].size * up - input[0].size * right;
	BR = pos - input[0].size * up + input[0].size * right;
	BL = pos - input[0].size * up - input[0].size * right;

	GS_PARTICLEOUT output[4];

	output[0].position = mul(mul(float4(BR, 1.0f), gmtxView), gmtxProjection);
	output[1].position = mul(mul(float4(TR, 1.0f), gmtxView), gmtxProjection);
	output[2].position = mul(mul(float4(BL, 1.0f), gmtxView), gmtxProjection);
	output[3].position = mul(mul(float4(TL, 1.0f), gmtxView), gmtxProjection);

	//output.position = mul(mul(float4(input[0].position.xyz, 1.0f), gmtxView), gmtxProjection);

	output[0].uv = float2(0, 1);
	output[1].uv = float2(0, 0);
	output[2].uv = float2(1, 1);
	output[3].uv = float2(1, 0);

	//output[0].position = float4( 1.0f, -1.0f, 0.98f, 1.0f);
	//output[1].position = float4( 1.0f,  1.0f, 0.98f, 1.0f);
	//output[2].position = float4(-1.0f, -1.0f, 0.98f, 1.0f);
	//output[3].position = float4(-1.0f,  1.0f, 0.98f, 1.0f);

	outStream.Append(output[0]);
	outStream.Append(output[1]);
	outStream.Append(output[2]);
	outStream.Append(output[3]);
}