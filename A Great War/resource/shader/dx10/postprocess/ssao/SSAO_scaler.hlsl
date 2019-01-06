
#include "shader_constants.h"

struct vsIn
{
	float4 position : POSITION;
	float2 texCoord0 : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
};

struct vsOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

#ifdef PIXEL_SHADER

Texture2D		lowRes_Buffer : register(t0);
Texture2D		highRes_Buffer : register(t1);
Texture2D		source_Buffer : register(t2);
Texture2D		destination_Buffer : register(t3);

SamplerState	point_Sampler : register(s0);
SamplerState	trilinear_Sampler : register(s1);


void UpdateNearestSample(inout float minDist,
	inout float2 nearestUV,
	float lowResVal,
	float2 uv,
	float highResVal
	)
{
	float dist = abs(lowResVal - highResVal);
	if (dist < minDist)
	{
		minDist = dist;
		nearestUV = uv;
	}
}

void GetNearestUVCoordinates(float2 texCoord, inout float2 nearestUV)
{
	float minDist = 1.e8f;
	uint w, h;

	lowRes_Buffer.GetDimensions(w, h);
	float2 lowResTexelSize = 1.f / float2(w, h);

	float2 loResUV = texCoord;

	float4 lowResValues = lowRes_Buffer.GatherRed(trilinear_Sampler, loResUV, int2(0, 0));
	float highResValue = highRes_Buffer.Sample(trilinear_Sampler, texCoord).r;

	float2 UV00 = loResUV - 0.5 * lowResTexelSize;
	nearestUV = UV00;
	float Z00 = lowResValues.w;
	UpdateNearestSample(minDist, nearestUV, Z00, UV00, highResValue);

	float2 UV10 = float2(UV00.x + lowResTexelSize.x, UV00.y);
	float Z10 = lowResValues.z;
	UpdateNearestSample(minDist, nearestUV, Z10, UV10, highResValue);

	float2 UV01 = float2(UV00.x, UV00.y + lowResTexelSize.y);
	float Z01 = lowResValues.x;
	UpdateNearestSample(minDist, nearestUV, Z01, UV01, highResValue);

	float2 UV11 = UV00 + lowResTexelSize;
	float Z11 = lowResValues.y;
	UpdateNearestSample(minDist, nearestUV, Z11, UV11, highResValue);
	/*
	[branch]
	if (abs(Z00 - highResValue) < g_DepthThreshold &&
	abs(Z10 - highResValue) < g_DepthThreshold &&
	abs(Z01 - highResValue) < g_DepthThreshold &&
	abs(Z11 - highResValue) < g_DepthThreshold)
	{
	return g_ScreenAlignedQuadSrc.SampleLevel(g_SamplerLoResBilinear, LoResUV, g_ScreenAlignedQuadMip);
	}
	else
	{
	return g_ScreenAlignedQuadSrc.SampleLevel(g_SamplerLoResNearest, NearestUV, g_ScreenAlignedQuadMip);
	}
	*/
}

float4 UpscaleAndBlend(vsOut _in) : SV_Target
{
	float2 nearestUV;
	GetNearestUVCoordinates(_in.texCoord, nearestUV);
	return destination_Buffer.Sample(trilinear_Sampler,_in.texCoord) * source_Buffer.Sample(trilinear_Sampler, nearestUV).r;
}

float4 Upscale(vsOut _in) : SV_Target
{
	float2 nearestUV;
	GetNearestUVCoordinates(_in.texCoord, nearestUV);

	return source_Buffer.Sample(trilinear_Sampler, nearestUV).r;
}

float4 DownscaleNearest(vsOut _in) : SV_Target
{
	return highRes_Buffer.Sample(point_Sampler, _in.texCoord).r;
}
#endif //PIXEL_SHADER
