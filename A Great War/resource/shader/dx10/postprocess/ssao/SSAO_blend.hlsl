
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

Texture2D		ssao_Buffer : register(t0);
Texture2D		scene_Buffer : register(t1);

SamplerState	point_Sampler : register(s0);

float4 Blend(vsOut _in) : SV_Target
{
	return scene_Buffer.Sample(point_Sampler, _in.texCoord) * ssao_Buffer.Sample(point_Sampler, _in.texCoord).r;
}
#endif //PIXEL_SHADER
