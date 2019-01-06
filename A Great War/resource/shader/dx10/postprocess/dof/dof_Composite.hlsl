#include "const.inc"
#include "shader_constants.h"

struct vsOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

#ifdef PIXEL_SHADER

Texture2D		BLURRED_buffer : register(t0);
SamplerState	BLURRED_sampler : register(s0);

Texture2D		RGBCoC_buffer : register(t1);
SamplerState	RGBCoC_sampler : register(s1);

Texture2D		ALPHA_buffer : register(t2);
SamplerState	ALPHA_sampler: register(s2);
    
float4 ComPoSite(vsOut _in) : SV_Target
{
	//Grab the sharp color (rgb) and the normaliced radius(a)
	float3 sharp = RGBCoC_buffer.Sample(RGBCoC_sampler, _in.texCoord.xy).rgb;


	// Bilinear interpolate when reading from the blurry buffer
	float4 pack = BLURRED_buffer.Sample(BLURRED_sampler, (_in.texCoord.xy));
	float3 blurred = pack.rgb;

	// Normalized Radius
	float normRadius = abs(pack.a *2 - 1);
	if (normRadius <= 0.01)
		normRadius = 0;


	//Interpolate between blurred and sharp picture
	float3 result = lerp(sharp, blurred, normRadius);
	return float4(result, 1.0f);
}

float4 Composite3rdPerson(vsOut _in) : SV_Target
{
	//Grab the sharp color (rgb) and the normaliced radius(a)
	float3 sharp = RGBCoC_buffer.Sample(RGBCoC_sampler, _in.texCoord.xy).rgb;


	// Bilinear interpolate when reading from the blurry buffer
	float3 blurred = BLURRED_buffer.Sample(BLURRED_sampler, (_in.texCoord.xy)).rgb;
	float alpha = ALPHA_buffer.Sample(ALPHA_sampler, (_in.texCoord.xy)).r;

	//Interpolate between blurred and sharp picture
	float3 result = lerp(blurred, sharp, alpha);
	return float4(result, 1.0f);
}
#endif//PIXEL_