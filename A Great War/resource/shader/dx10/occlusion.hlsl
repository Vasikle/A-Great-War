#include "shader_constants.h"

struct vsOut
{
	float4 position : SV_POSITION;
};

#ifdef PIXEL_SHADER
  cbuffer occ_ps_prim : register(pscMaterialGroup)
  {
	float4		tFactor	: packoffset(pscTFactor);
  };
#else//PIXEL_SHADER
  cbuffer occ_vs_scene : register(vscSceneGroup)
  {
	float4x4	viewProj: packoffset(vscViewProj);
  };
  cbuffer occ_vs_prim : register(pscMaterialGroup)
  {
	float4x3	world	: packoffset(pvscWorld0);
  };
#endif//PIXEL_SHADER

#ifdef PIXEL_SHADER

float4 PS(vsOut _in) : SV_TARGET
{
	return tFactor;
}

#else//PIXEL_SHADER

struct vsIn
{
	float3 position : POSITION;
};

vsOut VS(vsIn _in)
{
	vsOut _out;
	float3 posW = mul(float4(_in.position, 1), world).xyz;
	_out.position = mul(float4(posW, 1), viewProj);
	return _out;
}

#endif//PIXEL_SHADER