SamplerState gSamplerState					: register(s0);
SamplerComparisonState gShadowSamplerState	: register(s1);
cbuffer cbPassInfo						: register(b0)
{
	matrix		gmtxView				: packoffset(c0);
	matrix		gmtxProjection			: packoffset(c4);
	matrix		gmtxViewInv				: packoffset(c8);
	matrix		gmtxProjectionInv		: packoffset(c12);
	matrix		gmtxTexture				: packoffset(c16);
	float3		gvCameraPosition		: packoffset(c20);
	float		gfTime					: packoffset(c20.w);
};
cbuffer cbGameObjectInfo				: register(b1)
{
	matrix		gmtxGameObject			: packoffset(c0);
};
cbuffer cbLightInfo						: register(b2)
{
	matrix		gmtxLightViewProj[6]	: packoffset(c0);
	float3		gvLightColor			: packoffset(c24);
	float		gfFalloffStart			: packoffset(c24.w);
	float3		gvLightDirection		: packoffset(c25);
	float		gfFalloffEnd			: packoffset(c25.w);
	float3		gvLightPosition			: packoffset(c26);
	float		gfSpotPower				: packoffset(c26.w);
	uint		gLightType				: packoffset(c27);
	bool		gbIsShadow				: packoffset(c27.y);

}
Texture2D gtxtColorMap					: register(t3);
Texture2D gtxtNormalMap					: register(t4);
Texture2D gtxtDepthMap					: register(t5);
Texture2D gtxtShadowMap					: register(t6);
TextureCube gtxtShadowCubeMap			: register(t7);
Texture2DArray gtxtShadowArrayMap		: register(t8);
cbuffer cbAnimationInfo					: register(b9)
{
	matrix		gmtxAnimation[64]		: packoffset(c0);
};
RWTexture2D<float4> gtxtPostProcessMap	: register(u10);
RWStructuredBuffer<float> gfAvgLum		: register(u11);


struct VS_INPUT {
	float3 position : POSITION;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	uint4  boneIdx	: BLENDINDICES;
	float4 weight	: BLENDWEIGHT;
	float2 uv		: TEXCOORD;
};
struct VS_OUTPUT {
	float4 position : SV_POSITION;
	float3 positionW: POSITION;
	float2 uv		: TEXCOORD0;
	float3 normalW	: NORMAL;
	float3 tangentW	: TANGENT;
};

struct VS_EFFECTOUTPUT {
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD0;
};

struct VS_PARTICLEOUTPUT {
	float3 positionW: POSITION;
	float  size		: PSIZE;
};
struct GS_OUTPUT
{
	float4 pos		: SV_POSITION;
	uint RTIndex	: SV_RenderTargetArrayIndex;
};
struct GS_PARTICLEOUT
{
	float4 position	: SV_POSITION;
	float2 uv		: TEXCOORD0;
};
struct GBuffer {
	float4 cColor	: SV_TARGET0;
	float4 cNormal	: SV_TARGET1;
};

float CalcWFromDepth(float depth) {
	float w = gmtxProjection._m32 / (depth - gmtxProjection._m22);
	return w;
}
float3 WorldPosFromLinearDepth(float2 uv) {
	float depth = gtxtDepthMap.Sample(gSamplerState, uv).r;
	float w = CalcWFromDepth(depth);
	float4 result = float4(uv * 2.0f - 1.0f, depth, 1);
	result.y *= -1;
	result *= w;
	result = mul(result, gmtxProjectionInv);
	result = mul(result, gmtxViewInv);
	return result.xyz;
}

static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);
