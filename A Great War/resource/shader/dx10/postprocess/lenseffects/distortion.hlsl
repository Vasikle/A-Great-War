#include "const.inc"
#include "shader_constants.h"

struct vsOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

struct vsIn
{
	float4 position : POSITION;
	float2 texCoord0 : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
};

#ifdef PIXEL_SHADER

cbuffer distortion_CB : register(pscSceneGroup)
{
	float  distortionFactor		: packoffset(pscLEParams.x);
	float  blockingFactor		: packoffset(pscLEParams.y);
}


Texture2D		scene_Buffer				: register(t0);
Texture2D		heatMask_Buffer				: register(t1);		// We need to apply heat data while the distortion happens

SamplerState	bilinear_Sampler			: register(s0);

void screenDistortion(inout float2 texCoord)
{
	texCoord.y += (texCoord.y * texCoord.x * distortionFactor) * distortionFactor;
	texCoord.x += cos(texCoord.x + distortionFactor) * distortionFactor;
};

void contentDistortion(inout float2 texCoord)
{
	float blockTextureFactor = 1 - scene_Buffer.Sample(bilinear_Sampler, frac(texCoord + blockingFactor)).r;
	texCoord -= blockingFactor * blockTextureFactor;
}

float4 distortion_PS(vsOut _in) : SV_TARGET
{
	screenDistortion(_in.texCoord);
	contentDistortion(_in.texCoord);
	float4 color = scene_Buffer.Sample(bilinear_Sampler, frac(_in.texCoord));
	color.a = heatMask_Buffer.Sample(bilinear_Sampler, frac(_in.texCoord)).r;
	return color;
}

#endif//PIXEL_SHADER