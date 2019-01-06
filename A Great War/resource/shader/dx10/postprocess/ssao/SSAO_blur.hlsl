
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


Texture2D		source_Buffer : register(t0);
Texture2D		scene_Buffer : register(t1);

SamplerState	bilinear_Sampler : register(s0);

#define SAMPLE_COUNT 5

//Sigma = 1
static float	sampleWeight[SAMPLE_COUNT] = { 0.153388f, 	0.221461f, 	0.250301f, 0.221461f, 0.153388f };
static float	sampleOffset[SAMPLE_COUNT] = { -2.5, -1.5, 0, 1.5, 2.5};


float4 Blur(float2 texCoord, bool horiz)
{

	float width, height;
	source_Buffer.GetDimensions(width, height);
	float2 texelSize = float2(1.f / width, 1.f / height);
	float2 realTexCoord = texCoord + texelSize * 0.5f;
	
	float color = 0;
	
	for (int i = 0; i < SAMPLE_COUNT; ++i)
	{
		float2 offset = float2(horiz ? sampleOffset[i] * texelSize.x : 0, horiz ? 0 : sampleOffset[i] * texelSize.y);
		color += source_Buffer.Sample(bilinear_Sampler, realTexCoord + offset).r * sampleWeight[i];
	}
	return color;
}



float4 PS_Horiz(vsOut _in) : SV_TARGET
{
	return Blur(_in.texCoord, true);
}
float4 PS_Vert(vsOut _in) : SV_TARGET
{
	return Blur(_in.texCoord, false);
}

float4 PS_VertAndBlend(vsOut _in) : SV_TARGET
{
	float verticalBlurredOcclusion = Blur(_in.texCoord, false);
	return scene_Buffer.Sample(bilinear_Sampler, _in.texCoord) * verticalBlurredOcclusion;
}

#endif //PIXEL_SHADER


