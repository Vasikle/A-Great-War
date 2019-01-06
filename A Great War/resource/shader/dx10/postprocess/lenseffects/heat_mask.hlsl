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

Texture2D		heatEmissionBase_Buffer	: register(t0);
SamplerState	bilinear_Sampler		: register(s0);

float4 heatMask_PS(vsOut _in) : SV_TARGET
{
	float4 color = heatEmissionBase_Buffer.Sample(bilinear_Sampler, _in.texCoord);
	return (color.r + color.g + color.b) * 0.3333333f;
}

#else//PIXEL_SHADER

vsOut heatMask_VS(vsIn _in)
{
	vsOut _out;
	_out.position = _in.position;
	_out.texCoord = _in.texCoord1;
	return _out;
}

#endif//PIXEL_SHADER