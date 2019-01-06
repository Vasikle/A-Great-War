#include "shader_constants.h"

/**FH:
 *	This shader goes through the pixels of a scene and takes the related dephValues from the (passed as texture object)
 *	depthBuffer back into the homogeneous clip space.
 */
struct vsOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

#ifdef PIXEL_SHADER
	#ifndef MULTISAMPLED
	Texture2D depthBuffer : register(t0);	//The with the scene objects filled depth buffer
	#else
		#if MULTISAMPLED == 1
		Texture2DMS<float4, 2> depthBuffer : register(t0);
		#elif MULTISAMPLED == 2
		Texture2DMS<float4, 4> depthBuffer : register(t0);
		#elif MULTISAMPLED == 3
		Texture2DMS<float4, 8> depthBuffer : register(t0);
		#elif MULTISAMPLED == 4
		Texture2DMS<float4, 16> depthBuffer : register(t0);
		#endif
	#endif		
SamplerState	depthSampler: register(s0);

cbuffer ps_scene : register(pscSceneGroup)
{
	float4x4	invViewProjectionMatrix	   : packoffset(pscInvViewProj);		//*FH: The inverted view projection matrix, to bring the pixels back to worldSpace
	float4x4	viewProjectionMatrix	   : packoffset(pscViewProj);			//*FH: The view projection matrix, to bring the pixels from world space into homogeneous clip space
};


float4 recPS(vsOut _in) : SV_TARGET
{
	//grab the related depth value form the depth buffer
#ifndef MULTISAMPLED
	float z = depthBuffer.Sample(depthSampler, _in.texCoord).r;					
#else
	
	float texWidth;
	float texHeight;
	float samples;

	depthBuffer.GetDimensions(texWidth, texHeight, samples);
	float z = depthBuffer.Load(int2(_in.texCoord.x * texWidth, _in.texCoord.y * texHeight), 0).r;
	
#endif
	
	float x = _in.texCoord.x *2.f - 1.f;
	float y = _in.texCoord.y *2.f - 1.f;										//bring the normalized coordinates back into the range (-1,1)

	float4 postProjection = float4(x, y, z, 1.f);								
	float4 preProjection = mul(postProjection, invViewProjectionMatrix);		//bring the ndc values back into worldspace
	preProjection /= preProjection.w;											//undo perspective projection
	float4 homogenClipSpace = mul(preProjection, viewProjectionMatrix);			//bring the world space coordinates into homogeneous clip space...
	float w = homogenClipSpace.w;												//...and grab the linear distance to near plane
	
	return float4(w, 0, 0, 0);
}

#else//PIXEL_SHADER

struct vsIn
{
	float4 position : POSITION;
	float2 texCoord0 : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
};

vsOut recVS(vsIn _in)
{
	vsOut _out;
	_out.position = _in.position;
	_out.texCoord = _in.texCoord1;
	return _out;
}


#endif//PIXEL_SHADER