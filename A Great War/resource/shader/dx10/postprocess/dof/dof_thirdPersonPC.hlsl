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
	float      deltaTime : packoffset(pscDOFParams.x);
}

Texture2D		W_Buffer : register(t0);					//View Space Z values
SamplerState	W_Sampler : register(s0);

Texture2D		ScreenMidW_Buffer : register(t1);			//View Space Z value of the screen mid from the last frame
SamplerState	ScreenMidW_Sampler : register(s1);

static const float INTERPOLATION_FACTOR = 4.f;
static const float MAX_DEPTH_DIFFERENCE = 1000;				//At which depth diffrence, the update should avoid instanttly taking the new depth into account

float4 ThirdPersonPC(vsOut _in) : SV_Target
{
	//Getting the mid of the screen
	float width, height;
	W_Buffer.GetDimensions(width, height);
	
	//Setting up the texel fetching
	float2 texelPadding = float2(1 / width, 1 / height);
	float2 texCoords = (float2)0;

	//Will hold later on the accumlated depth
	float depth = 0;
	
	//Holds the depth fursthest away and nearest from this samples
	float furthestDepth = 0;
	float nearestDepth = 10000000;

	//Loop through all the pixels in the passed texture and sum up the depth sampled form each texel
	for (int i = 0; i < width; i++)
	{
		texCoords.x = asfloat(i) * texelPadding.x;

		for (int j = 0; j < height; j++)
		{
			texCoords.y = asfloat(j) * texelPadding.y;
			float sampledDepth = W_Buffer.Sample(W_Sampler, texCoords).r;
			depth += sampledDepth;
			
			//Storing the nearest and furthest depth of the collected samples
			nearestDepth = min(nearestDepth, sampledDepth);
			furthestDepth = max(furthestDepth, sampledDepth);
		}
	}

	
	//Get the mean depth
	depth /= width * height;

	//Callculate the max diffrence between the nearest and furthest depth sample and derive a factor for lerp out of it.
	// If the depth diffrence is greater than MAX_DEPTH_DIFFRENCE -> 1 - 1 = 0 -> negating this update.
	// This should avoid flickering caused through single pixels with a huge depth diffrence to thew rest of the samples.
	// For example, a single pixel, which is part of the sky (far plane), between the leafs of a tree you looking at.
	float lerpFactor = 1 - saturate((furthestDepth - nearestDepth) / MAX_DEPTH_DIFFERENCE);

	//grab the depth form the last frame
	float lastDepth = ScreenMidW_Buffer.Sample(ScreenMidW_Sampler, float2(0, 0));

	return lerp(lastDepth, depth, saturate(deltaTime * INTERPOLATION_FACTOR * lerpFactor));
}

#endif//PIXEL_SHADER
