#include "shader_constants.h"
#include "tools.inc"

struct vsOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

#ifdef PIXEL_SHADER

cbuffer color_tune1 : register(pscSceneGroup)
{
	float3 colorTone 	: packoffset(pscTone);
	float gamma 		: packoffset(pscGBCSat.x);
	float brightness 	: packoffset(pscGBCSat.y);
	float contrast 		: packoffset(pscGBCSat.z);
	float saturation 	: packoffset(pscGBCSat.w);

//*FH added
#ifdef GRAIN_EFFECT
	float renderedTextureDimsX		: packoffset(pscFGRenRes.x);		//Vector3.x
	float renderedTextureDimsY		: packoffset(pscFGRenRes.y);		//Vector3.y
	float fTime						: packoffset(pscFGRenRes.z);		//Vector3.z
	float grainAmount				: packoffset(pscFGParameters.x);	//Vecotr3.x
	float grainParticleSize			: packoffset(pscFGParameters.y);	//Vecotr3.y
	float lumAmount					: packoffset(pscFGParameters.z);	//Vecotr3.z
	float colorAmount				: packoffset(pscFGParameters.w);	//Vector3.w
#endif//GRAIN_EFFECT
#ifdef COLOR_SHADE
	float3 shadeTone					: packoffset(pscCSColorTone);		//The channel which should be considred( must be something normalized, like (1,0,0) for red for example.
#endif	//COLOR_SHADE
};


#ifdef COLOR_SHADE

/**FH: Small helper to create the overlay by calculating the grayscale and
  *    just adding it to the diesred colour channel.
  *
  *@param color a four comp color, allready altered by previous shader steps
  *@return the averaged color for a specific channel
  */
float3 getAveragedColor(float3 lcolor)
{
	//Calculating the pixels luminance
	//0.21 R + 0.72 G + 0.07 B -> realistic
	//0.5R + 0.5g + 0.5B -> pure average
	
	float lum = lcolor.r * 0.21 + lcolor.g * 0.72 + lcolor.b * 0.07;
	
	//Take the passed tone and scale it by the pixel luminance
	lcolor.r = lum * shadeTone.r;
	lcolor.b = lum * shadeTone.b;
	lcolor.g = lum * shadeTone.g;

	return lcolor;
	
}
#endif //COLOR_SHADE

#ifdef GRAIN_EFFECT
#include "postprocess\film_grain_assist.inc"
#endif //GRAIN_EFFECT

Texture2D baseTexture : register(t0);
SamplerState baseSampler : register(s0);

float4 PS(vsOut _in) : SV_TARGET
{
	//@to_do optimize:
	//color = pow(color * A - B, C) where 
	//A = contrast * brightness
	//B = brightness * (0.5f * contrast + 0.5f)
	//C = 1.0f / gamma
	//pass to shader A, B, C instead brightness, contrast, gamma

	float4 color = baseTexture.Sample(baseSampler, _in.texCoord);
 	color.rgb = pow(saturate((color.rgb - 0.5f) * contrast + 0.5f) * brightness, 1.0f / gamma);
	color.rgb = saturate(color.rgb + colorTone);
 	color.rgb = lerp(Luminance(color.rgb), color.rgb, saturation);


/**FH: adding a color tinting per channel*/
#ifdef COLOR_SHADE
	color.rgb = getAveragedColor(color.rgb);
#endif
/* *FH: Starting incooperating the grain effect but only if GRAIN_EFFECT is defined */

#ifdef GRAIN_EFFECT
	
	float2 texCoord = _in.texCoord;

	float xGrainSize = renderedTextureDimsX / grainParticleSize;
	float yGrainSize = renderedTextureDimsY / grainParticleSize;

	static float3 rotOffset = float3(1.425, 3.892, 5.835); //rotation offset values	
	float2 rotCoordsR = coordRot(texCoord, fTime + rotOffset.x);

	float tmp = pnoise3D(float3(rotCoordsR*float2(xGrainSize, yGrainSize), 0.0));
	float3 noise = float3(tmp, tmp, tmp);

	//Coloring part
	if(colorAmount >= 0.f)
	{
		float2 rotCoordsG = coordRot(texCoord, fTime + rotOffset.y);
		float2 rotCoordsB = coordRot(texCoord, fTime + rotOffset.z);
		noise.g = lerp(noise.r, pnoise3D(float3(rotCoordsG*float2(xGrainSize, yGrainSize), 1.0)), colorAmount);
		noise.b = lerp(noise.r, pnoise3D(float3(rotCoordsB*float2(xGrainSize, yGrainSize), 2.0)), colorAmount);
	} 

	float3 col = color.rgb;

	//noisiness response curve based on scene luminance
	static float3 lumcoeff = float3(0.299, 0.587, 0.114);
	float luminance = lerp(0.0, dot(col, lumcoeff), lumAmount);
	float lum = smoothstep(0.2, 0.0, luminance);
	lum += luminance;


	noise = lerp(noise, float3(0, 0, 0), pow(lum, 4.0));
	col = col + noise * grainAmount;

	return float4(col, color.a);
#endif //GRAIN_EFFECT
		
	return color;
}

#else//PIXEL_SHADER

struct vsIn
{
	float4 position : POSITION;
	float2 texCoord0 : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
};


vsOut VS(vsIn _in)
{
	vsOut _out;
	_out.position = _in.position;
	_out.texCoord = _in.texCoord1;
	return _out;
}

#endif//PIXEL_SHADER
