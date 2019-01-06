
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

Texture2D		scene_Buffer : register(t0);
SamplerState	scene_Sampler : register(s0);

Texture2D		vingeteTex_Buffer : register(t1);
SamplerState	vingeteTex_Sampler : register(s1);

cbuffer Zoom_PS_Parameters : register(pscSceneGroup)
{
	float fadeState : packoffset(pscZoomParams);
}

float4 ZoomPS(vsOut _in) : SV_Target
{ 
	float4 sceneclr = scene_Buffer.Sample(scene_Sampler, _in.texCoord);
	float vingeteFactor = vingeteTex_Buffer.Sample(vingeteTex_Sampler, _in.texCoord).r;
	return lerp(sceneclr,lerp(float4(0.1, 0.1, 0.1, 1), sceneclr, vingeteFactor),fadeState);
}

#else //PIXEL_SHADER

vsOut ZoomVS(vsIn _in)
{
	vsOut _out;
	_out.position = _in.position;
	_out.texCoord = _in.texCoord1;
	return _out;
}
#endif //PIXEL_SHADER