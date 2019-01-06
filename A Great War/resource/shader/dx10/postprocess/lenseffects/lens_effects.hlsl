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

cbuffer lensEffects_CB : register(pscSceneGroup)
{
	//Shared
	float  viewFactor		: packoffset(pscLEParams.x);	//A factor [0,1] that describes, how "directly" the camear is looking into the sun
	float  lightPosX		: packoffset(pscLELightPos.x);	//Normalized screen x position of the sun
	float  lightPosY		: packoffset(pscLELightPos.y);	//Normalized screen y position of the sun
	float3 lightTone		: packoffset(pscLEColor);		//Used to the manipulate the light tone 
	float  sunMoonRadius    : packoffset(pscLEColor.w);		//Describes the radius of the sun/moon in as a factor
	float  brightIntensity	: packoffset(pscLEParams2.x);	//Controlls the intensity of the screen brightening, when looking into the sun/moon
	float  flickerShift		: packoffset(pscLEParams2.y);	//Shift of the y coordinate of the flicker texture

	//Lightscattering
	float  intensity		: packoffset(pscLEParams.y);		//Determines how strong each sampled pixel is weighted on the sum. 
	float  ray_length		: packoffset(pscLEParams.z);		//Determines how strong the general drop off of sampled pixels in relation to sampleCount. High values -> longer rays
	float  aspectRatio		: packoffset(pscLEParams.w);		//The current resolutions aspect rato
	float  halfPixelX_LR	: packoffset(pscLELightPos.z);		//texel.x size in the illuminated low res texture
	float  halfPixelY_LR	: packoffset(pscLELightPos.w);		//texel.y size in the illuminated low res texture
}

static const float3 lumaFactorsReal = float3(0.21f, 0.72f, 0.07f);
static const float lumaFactorAverage = 0.33333333f;

Texture2D		scene_Buffer				: register(t0);
Texture2D		illuminatedScene_Buffer		: register(t1);

SamplerState	bilinear_Sampler			: register(s0);


float getLuminanceFactor(float3 _sample)
{
	return _sample.x + _sample.y + _sample.z * lumaFactorAverage;
}

#ifdef LIGHT_SCATTERING
#include "postprocess\lenseffects\light_scattering.inc"
#endif
#ifdef LENS_DIRT
#include "postprocess\lenseffects\lens_dirt.inc"
#endif
#ifdef BRIGHTENING
#include "postprocess\lenseffects\brightness.inc"
#endif
#ifdef NIGHT_VISION
#include "postprocess\lenseffects\night_vision.inc"
#endif
#ifdef THERMAL_VISION
#include "postprocess\lenseffects\thermal_vision.inc"
#endif
#ifdef DRONE_VISION
#include "postprocess\lenseffects\drone_vision_.inc"
#endif

float4 lensEffects_PS(vsOut _in) : SV_TARGET
{
	float4 color = scene_Buffer.Sample(bilinear_Sampler, _in.texCoord);

	float globalSunRadius = 0.035f * sunMoonRadius;
#ifdef LIGHT_SCATTERING
	addLightScattering(color, _in.texCoord, globalSunRadius);
#endif
#ifdef LENS_DIRT
	addLensDirt(color, _in.texCoord);
#endif
#ifdef BRIGHTENING
	addScreenBrightness(color);
#endif
#ifdef NIGHT_VISION
	addNightVision(color, _in.texCoord);
#endif
#ifdef THERMAL_VISION
	addThermalVision(color, _in.texCoord);
#endif
#ifdef DRONE_VISION
	addDroneVision(color, _in.texCoord);
#endif
	return color;
}

#else//PIXEL_SHADER

vsOut lensEffects_VS(vsIn _in)
{
	vsOut _out;
	_out.position = _in.position;
	_out.texCoord = _in.texCoord1;
	return _out;
}

#endif//PIXEL_SHADER