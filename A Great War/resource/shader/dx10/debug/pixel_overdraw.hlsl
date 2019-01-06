#include "shader_constants.h"

struct vsOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};

#ifdef PIXEL_SHADER

float4 PS(vsOut _in) : SV_TARGET
{
	return _in.color;
}

#else//PIXEL_SHADER

cbuffer vs_scene : register(vscSceneGroup)
{
	float4x4	viewProj : packoffset(vscViewProj);
};

#ifdef SKIN
  cbuffer vs_skin : register(vscSkinGroup)
  {
	float4x3	world[MAX_SKIN_BONE_COUNT] : packoffset(vscWorld0Skinned);
  };
#else
  cbuffer vs_prim : register(pscMaterialGroup)
  {
	float4x3	world : packoffset(pvscWorld0);
  };
#endif//SKIN

struct vsIn
{
    float4 position		: POSITION;
#ifdef SKIN
	float blendWeight : BLENDWEIGHT;
	uint4 blendIndex : BLENDINDICES;
#endif
};

vsOut VS(vsIn _in)
{
	vsOut _out;
	float3 posW;
#ifndef SKIN
	posW = mul(_in.position, world);
#else
	uint4 indexVector = _in.blendIndex;
	float3 posW0 = mul(_in.position, world[indexVector[0]]);
	float3 posW1 = mul(_in.position, world[indexVector[1]]);
	posW = lerp(posW1, posW0, _in.blendWeight);
#endif
	_out.position = mul(float4(posW, 1), viewProj);
	_out.color = float4(0.1, 0.1, 0.1, 1);
	return _out;
}

#endif//PIXEL_SHADER
