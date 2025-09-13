#include "Header.hlsli"

[numthreads(256, 1, 1)]
void CS_main( uint3 DTid : SV_DispatchThreadID )
{
	gtxtPostProcessMap[DTid.xy] = float4(gtxtColorMap[DTid.xy].xyz * float3(1.7f, 0.3f, 0.3f), gtxtColorMap[DTid.xy].w);
	//gtxtPostProcessMap[DTid.xy] = float4(1,1,1,1);
}

/*=============================================================================
* Blur
=============================================================================*/
static const int	gMaxBlurRadius = 5;
#define NBlurTh 256
#define CacheSize (NBlurTh + 2 * gMaxBlurRadius)
groupshared float4	gBlurCache[CacheSize];

[numthreads(256, 1, 1)]
void CS_VerticalBlur(
	uint3 GTid : SV_GroupThreadID,
	uint3 DTid : SV_DispatchThreadID)
{
	int					gBlurRadius = 4;

	float weights[9] = {
		0.000133831,
		0.00443186,
		0.0539911,
		0.241971,
		0.398943,
		0.241971,
		0.0539911,
		0.00443186,
		0.000133831
		//0,
		//0,
		//0.000263865,
		//0.106451,
		//0.786571,
		//0.106451,
		//0.000263865,
		//0,
		//0
	};

	if (GTid.x < gBlurRadius) {
		int x = max(DTid.x - gBlurRadius, 0);
		gBlurCache[GTid.x] = gtxtColorMap[int2(x, DTid.y)];
	}
	if (GTid.x >= NBlurTh - gBlurRadius) {
		int x = min(DTid.x + gBlurRadius, gtxtColorMap.Length.x - 1);
		gBlurCache[GTid.x + 2 * gBlurRadius] = gtxtColorMap[int2(x, DTid.y)];
	}
	gBlurCache[GTid.x + gBlurRadius] = gtxtColorMap[min(DTid.xy, gtxtColorMap.Length.xy-1)];

	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0, 0, 0, 0);

	for (int i = -gBlurRadius; i <= gBlurRadius; ++i) {
		int k = GTid.x + gBlurRadius + i;
		blurColor += weights[i + gBlurRadius] * gBlurCache[k];
	}

	gtxtPostProcessMap[DTid.xy] = blurColor;
}

[numthreads(1, 256, 1)]
void CS_HorizontalBlur(
	uint3 GTid : SV_GroupThreadID,
	uint3 DTid : SV_DispatchThreadID)
{
	int					gBlurRadius = 4;

	float weights[9] = {
		0.000133831,
		0.00443186,
		0.0539911,
		0.241971,
		0.398943,
		0.241971,
		0.0539911,
		0.00443186,
		0.000133831
		//0,
		//0,
		//0.000263865,
		//0.106451,
		//0.786571,
		//0.106451,
		//0.000263865,
		//0,
		//0
	};

	if (GTid.y < gBlurRadius) {
		int y = max(DTid.y - gBlurRadius, 0);
		gBlurCache[GTid.y] = gtxtColorMap[int2(DTid.x, y)];
	}
	if (GTid.y >= NBlurTh - gBlurRadius) {
		int y = min(DTid.y + gBlurRadius, gtxtColorMap.Length.y - 1);
		gBlurCache[GTid.y + 2 * gBlurRadius] = gtxtColorMap[int2(DTid.x, y)];
	}
	gBlurCache[GTid.y + gBlurRadius] = gtxtColorMap[min(DTid.xy, gtxtColorMap.Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0, 0, 0, 0);

	for (int i = -gBlurRadius; i <= gBlurRadius; ++i) {
		int k = GTid.y + gBlurRadius + i;
		blurColor += weights[i + gBlurRadius] * gBlurCache[k];
	}

	gtxtPostProcessMap[DTid.xy] = blurColor;
}

/*=============================================================================
* HDR, Bloom
=============================================================================*/
static const uint2 RES = uint2(1920 / 4, 1080 / 4);
static const uint DOMAIN = (1920 * 1080) / 16;
static const uint GroupSize = 1024;
//static const uint GroupSize = DOMAIN * 1024;
groupshared float SharedPositions[1024];

#define BLOOM

float DownScale4x4(uint2 curPixel, uint GTid) {
	float avgLum = 0.0f;
	int3 nFullResPos = int3(curPixel * 4, 0);
	float4 downScaled = float4(0, 0, 0, 0);
	[unroll]
	for (int i = 0; i < 4; i++) {
		[unroll]
		for (int j = 0; j < 4; j++) {
			downScaled += gtxtColorMap.Load(nFullResPos, int2(j, i));
		}
	}

	downScaled /= 16.0;
#ifdef BLOOM
	gtxtPostProcessMap[curPixel.xy] = downScaled;
#else

#endif
	avgLum = dot(downScaled, LUM_FACTOR);
	SharedPositions[GTid] = avgLum;
	GroupMemoryBarrierWithGroupSync();

	return avgLum;
}
float DownScale1024to4(uint DTidx, uint GTid, float avgLum) {
	[unroll]
	for (uint groupSize = 4, step1 = 1, step2 = 2, step3 = 3;
		groupSize < 1024;
		groupSize *= 4, step1 *= 4, step2 *= 4, step3 *= 4) {
		if (GTid % groupSize == 0) {
			float stepAvgLum = avgLum;
			stepAvgLum += DTidx + step1 < 1920 ?
				SharedPositions[GTid + step1] : avgLum;
			stepAvgLum += DTidx + step2 < 1920 ?
				SharedPositions[GTid + step2] : avgLum;
			stepAvgLum += DTidx + step3 < 1920 ?
				SharedPositions[GTid + step3] : avgLum;
			avgLum = stepAvgLum;
			SharedPositions[GTid] = stepAvgLum;
		}
		GroupMemoryBarrierWithGroupSync();

	}
	return avgLum;
}
void DownScale4to1(uint DTidx, uint GTid, uint Gidy, float avgLum) {
	if (GTid == 0) {
		float fFinalAvgLum = avgLum;

		fFinalAvgLum += DTidx + 256 < 1920 ?
			SharedPositions[GTid + 256] : avgLum;
		fFinalAvgLum += DTidx + 512 < 1920 ?
			SharedPositions[GTid + 512] : avgLum;
		fFinalAvgLum += DTidx + 768 < 1920 ?
			SharedPositions[GTid + 768] : avgLum;

		fFinalAvgLum /= 1024.0;

		gfAvgLum[Gidy] = fFinalAvgLum;
	}
}

[numthreads(1024, 1, 1)]
void CS_DownScaleFirstPass(uint3 Gid : SV_GroupID,
	uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID) {

	// Resolution 제한을 두는 이유는
	// gtxtPostProcessMap[xy]에 넣어야 하는데
	// 그 크기가 RES만큼이기 때문.
	// y는 어짜피 Dispatch를 2, RES.y, 1 만큼 할거라 ㄱㅊ음
	uint2 curPixel = uint2(DTid.x%RES.x, DTid.y);

	float avgLum = DownScale4x4(curPixel, GTid.x);
	avgLum = DownScale1024to4(DTid.x, GTid.x, avgLum);
	DownScale4to1(DTid.x, GTid.x, Gid.y, avgLum);
}

groupshared float SharedAvgFinal[1024];

[numthreads(1024, 1, 1)]
void CS_DownScaleSecondPass(uint3 Gid : SV_GroupID,
	uint3 GTid : SV_GroupThreadID,
	uint3 DTid : SV_DispatchThreadID) {

	// 앞에서 저장한 1, 270, 1 의 결과물 (Gid 270까지의 버퍼 내용)
	float avgLum = 0.0f;
	if (DTid.x < 270) avgLum = gfAvgLum[DTid.x];
	SharedAvgFinal[DTid.x] = avgLum;

	GroupMemoryBarrierWithGroupSync();

	// 1024 -> 256
	if (DTid.x % 4 == 0) {
		float stepAvgLum = avgLum;
		stepAvgLum += DTid.x + 1 < GroupSize ?
			SharedAvgFinal[DTid.x + 1] : avgLum;
		stepAvgLum += DTid.x + 2 < GroupSize ?
			SharedAvgFinal[DTid.x + 2] : avgLum;
		stepAvgLum += DTid.x + 3 < GroupSize ?
			SharedAvgFinal[DTid.x + 3] : avgLum;
		
		avgLum = stepAvgLum;
		SharedAvgFinal[DTid.x] = stepAvgLum;
	}

	GroupMemoryBarrierWithGroupSync();

	// 256 -> 64
	if (DTid.x % 16 == 0) {
		float stepAvgLum = avgLum;
		stepAvgLum += DTid.x + 4 < GroupSize ?
			SharedAvgFinal[DTid.x + 4] : avgLum;
		stepAvgLum += DTid.x + 8 < GroupSize ?
			SharedAvgFinal[DTid.x + 8] : avgLum;
		stepAvgLum += DTid.x + 12 < GroupSize ?
			SharedAvgFinal[DTid.x + 12] : avgLum;

		avgLum = stepAvgLum;
		SharedAvgFinal[DTid.x] = stepAvgLum;
	}

	GroupMemoryBarrierWithGroupSync();

	// 64 -> 16
	if (DTid.x % 64 == 0) {
		float stepAvgLum = avgLum;
		stepAvgLum += DTid.x + 16 < GroupSize ?
			SharedAvgFinal[DTid.x + 16] : avgLum;
		stepAvgLum += DTid.x + 24 < GroupSize ?
			SharedAvgFinal[DTid.x + 24] : avgLum;
		stepAvgLum += DTid.x + 48 < GroupSize ?
			SharedAvgFinal[DTid.x + 48] : avgLum;

		avgLum = stepAvgLum;
		SharedAvgFinal[DTid.x] = stepAvgLum;
	}

	GroupMemoryBarrierWithGroupSync();

	//if (DTid.x == 0) {
	//	float fFinalLumValue = avgLum;
	//	fFinalLumValue += DTid.x + 64 < GroupSize ?
	//		SharedAvgFinal[DTid.x + 64] : avgLum;
	//	fFinalLumValue += DTid.x + 128 < GroupSize ?
	//		SharedAvgFinal[DTid.x + 128] : avgLum;
	//	fFinalLumValue += DTid.x + 192 < GroupSize ?
	//		SharedAvgFinal[DTid.x + 192] : avgLum;

	//	fFinalLumValue /= 512.0;
	//	gfAvgLum[0] = fFinalLumValue;
	//}
	// 16 -> 4
	if (DTid.x % 64 == 0) {
		float stepAvgLum = avgLum;
		stepAvgLum += DTid.x + 64 < GroupSize ?
			SharedAvgFinal[DTid.x + 64] : avgLum;
		stepAvgLum += DTid.x + 128 < GroupSize ?
			SharedAvgFinal[DTid.x + 128] : avgLum;
		stepAvgLum += DTid.x + 192 < GroupSize ?
			SharedAvgFinal[DTid.x + 192] : avgLum;

		avgLum = stepAvgLum;
		SharedAvgFinal[DTid.x] = stepAvgLum;
	}

	GroupMemoryBarrierWithGroupSync();

	// 4 -> 1
	if (DTid.x == 0) {
		float fFinalLumValue = avgLum;
		fFinalLumValue += DTid.x + 256 < GroupSize ?
			SharedAvgFinal[DTid.x + 256] : avgLum;
		fFinalLumValue += DTid.x + 512 < GroupSize ?
			SharedAvgFinal[DTid.x + 512] : avgLum;
		fFinalLumValue += DTid.x + 768 < GroupSize ?
			SharedAvgFinal[DTid.x + 768] : avgLum;

		fFinalLumValue /= 1024.0;
		gfAvgLum[0] = fFinalLumValue;
	}
}

static const float fBloomThreshols = 0.8f;

[numthreads(1024,1,1)]
void CS_Bloom(uint3 DTid : SV_DispatchThreadID) {
	uint2 curPixel = uint2(DTid.x % RES.x, DTid.y % RES.y);
	//if (curPixel.y < RES.y) {
		float4 color = gtxtColorMap.Load(int3(curPixel, 0));
		float Lum = dot(color, LUM_FACTOR);
		float avgLum = gfAvgLum[0];

		float colorScale = saturate(Lum - avgLum * fBloomThreshols);
		gtxtPostProcessMap[curPixel.xy] = color * colorScale;
	//}
}