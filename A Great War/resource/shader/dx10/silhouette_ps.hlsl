#include "shader_constants.h"
#include "vs_out.inc"

#ifdef DITHER_OPACITY
 #include "dithered_opacity.inc"
#endif

#if TEXCOORDS > 0
Texture2D texDiffuse : register(t0);
SamplerState samDiffuse : register(s0);
#endif

cbuffer ps_scene : register(pscSceneGroup)
{
 #include "haze\ps_constants.inc"
};

cbuffer ps_mtl : register(pscMaterialGroup)
{
	float4 tFactor : packoffset(pscTFactor);
};

#include "haze\ps.inc"

float4 PS(vsOut _in) : SV_TARGET
{
	float4 color = tFactor;

#if TEXCOORDS > 0
	color.a = texDiffuse.Sample(samDiffuse, _in.texCoord).a;
	clip(color.a - 0.5f);
#else
	color.a = 1;
#endif

#ifdef DITHER_OPACITY
	ClipDithered(_in.position, tFactor.a);
#endif

#ifdef HAZE
	ApplyHaze(color.rgb, _in.haze);
#endif
	return color;
}
