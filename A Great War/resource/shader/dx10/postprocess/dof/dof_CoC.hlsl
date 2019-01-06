#include "const.inc"
#include "shader_constants.h"

struct vsOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

#ifdef PIXEL_SHADER


cbuffer dof_buffer : register(pscSceneGroup)
{
	float4      zPlanes			: packoffset(pscDOFZPlanes);		// .x = nearBlurryPlaneZ, .y = nearSharpPlaneZ or nearThreshold , .z = farSharpPlaneZ or farThreshold, .w = farBlurryPlaneZ
	float       nearScale		: packoffset(pscDOFParams.x);		// Scales the blur in the near blur field
	float       farScale		: packoffset(pscDOFParams.y);		// Scales the blur in the far blur field
}

Texture2D		COLOR_buffer : register(t0);		//Color source
SamplerState	COLOR_sampler: register(s0);

Texture2D		W_Buffer : register(t1);			//View Space Z values
SamplerState	W_Sampler : register(s1);

#ifdef THIRD_PERSON
Texture2D		ScreenMid_Buffer : register(t2);			//1x1 Texture storing the averaded depth of the center of the screen.
SamplerState	ScreenMid_Sampler : register(s2);
#endif


float4 CoCPS(vsOut _in) : SV_Target
{
	float nearBlurryPlaneZ = zPlanes.x;		// Z distance, where the near blur is at max (> 0; 0)
	float farBlurryPlaneZ = zPlanes.w;		// Z distance, where the far blur is at max (< farZ; farZ)
	
#ifdef THIRD_PERSON
	
	float nearThreshold =  zPlanes.y;
	float farThreshold = zPlanes.z;
	
	float screenMidDepth = ScreenMid_Buffer.Sample(ScreenMid_Sampler, float2(0, 0)).r;

	float nearSharpPlaneZ = screenMidDepth - nearThreshold;		// Z distance, where the near blur ends 
	float farSharpPlaneZ = screenMidDepth + farThreshold;		// Z distance, where the far blur starts
#else
	float nearSharpPlaneZ = zPlanes.y;		// Z distance, where the near blur ends 
	float farSharpPlaneZ = zPlanes.z;		// Z distance, where the far blur starts
#endif
	

	float4 colorNcoc;
	//Grabbing the color of the scene
	colorNcoc.rgb = COLOR_buffer.Sample(COLOR_sampler, _in.texCoord).rgb;
	
	//Grabbing the reconstructed viewspace depth value, of this pixel
	float z = W_Buffer.Sample(W_Sampler, _in.texCoord).r;

	if (z < nearSharpPlaneZ) //in the near blurry field
	{
		//Calculates CoC in near field, scaled by nearScale
		//making it negative
		colorNcoc.a = -((nearSharpPlaneZ - max(z, nearBlurryPlaneZ)) / (nearSharpPlaneZ - nearBlurryPlaneZ) * nearScale);
	}
	else
	{
		if (z < farSharpPlaneZ)
		{
			// In the focus field
			colorNcoc.a = 0.0;
		}
		else //in the far field
		{
			//Calculates CoC in far field, scaled by farScale
			colorNcoc.a = (min(z, farBlurryPlaneZ) - farSharpPlaneZ) / (farBlurryPlaneZ - farSharpPlaneZ) * farScale;
		}
	}
	
	/*Map it to range 0 - 1*/
	colorNcoc.a = colorNcoc.a / 2 + 0.5;

	return colorNcoc;
}

#ifdef THIRD_PERSON
float4 CoC_3rdP_M2(vsOut _in) : SV_Target
{
	float4 result = COLOR_buffer.Sample(COLOR_sampler, _in.texCoord);
	result.a = clamp(0.f,0.5,(ScreenMid_Buffer.Sample(ScreenMid_Sampler, _in.texCoord).r / 2));
	return result;
}
#endif

float4 CoC_PauseScreen(vsOut _in) : SV_Target
{
	//Faking a full near field blur for the whole screen.
	return float4(COLOR_buffer.Sample(COLOR_sampler, _in.texCoord).rgb,1.f);
}

#else//PIXEL_SHADER

struct vsIn
{
	float4 position : POSITION;
	float2 texCoord0 : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
};
vsOut dofCoCVS(vsIn _in)
{
	vsOut _out;
	_out.position = _in.position;
	_out.texCoord = _in.texCoord1;
	return _out;
}

#endif//PIXEL_SHADER
