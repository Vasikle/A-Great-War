#include "shader_constants.h"
#include "const.inc"
#include "shadow\interpolants.inc"
#include "vs_in.inc"
#include "vs_out.inc"

#ifdef WIND_GLOBAL
#define WIND
#endif

cbuffer vs_scene : register(vscSceneGroup)
{
	float4x4	viewProj	: packoffset(vscViewProj);
	float3		cameraOrigin: packoffset(vscCameraOrigin);	// world
#if defined(WIND)
	float3 		windParams	: packoffset(vscWindParams);
#endif //WIND
 #include "haze\vs_constants.inc"
 #include "shadow\vs_constants.inc"
 #include "fogofwar\vs_constants.inc"
};

cbuffer vs_primitive : register(pscMaterialGroup)
{
#if !defined(SKIN) && !defined(INSTANCED)
	float4x3	world 		: packoffset(pvscWorld0);
#endif
#if defined(WIND) && !defined(WIND_GLOBAL) && !defined(INSTANCED)
	float 		windPhase	: packoffset(pvscWindParamsPrim);
#endif //WIND
};

cbuffer vs_primitive_misc : register(vscPrimitiveMiscGroup)
{
	float4		texXformX	: packoffset(vscTexXformX);
	float4		texXformY	: packoffset(vscTexXformY);
};

#if defined(INSTANCED)
 #define 	world _in.inst_world
 #define 	windPhase _in.inst_wind
#endif//INSTANCED

#ifdef SKIN
cbuffer vs_skin : register(vscSkinGroup)
{
	float4x3	world[MAX_SKIN_BONE_COUNT] : packoffset(vscWorld0Skinned);
};
#endif//SKIN

#include "haze\vs.inc"

#ifdef WIND
 #define windDir windParams.xy
 #ifdef WIND_GLOBAL
  #define windScale _in.windPars.x
  #define windTime (windParams.z + _in.windPars.y)
 #else
  #define windScale _in.position.z
  #define windTime (windParams.z + windPhase)
 #endif
#endif //WIND

#include "shadow\vs.inc"
#include "fogofwar\vs.inc"

vsOut VS(vsIn _in)
{
	vsOut _out;
	float3 posW;
	float3 normal;
	float3 tangent;
	float3 binormal;

	posW = _in.position;

#ifdef SKIN

	uint4 indexVector = _in.blendIndex;
	float3 posW0 = mul(float4(posW, 1), world[indexVector[0]]).xyz;
	float3 posW1 = mul(float4(posW, 1), world[indexVector[1]]).xyz;
	posW = lerp(posW1, posW0, _in.blendWeight);
 #ifdef NEED_NORMAL
	float3 n0 = mul(_in.normal, world[indexVector[0]]);
	float3 n1 = mul(_in.normal, world[indexVector[1]]);
	normal = lerp(n1, n0, _in.blendWeight);
 #endif
 #ifdef BUMP
	float3 t0 = mul(_in.tangent.xyz, world[indexVector[0]]);
	float3 t1 = mul(_in.tangent.xyz, world[indexVector[1]]);
	tangent = lerp(t1, t0, _in.blendWeight);
 #endif

#else //SKIN

	posW = mul(float4(posW, 1), world).xyz;
 #ifdef NEED_NORMAL
	normal = normalize(mul(_in.normal, world));
 #endif

 #ifdef BUMP
	#ifdef TEX_BLEND
		tangent = float3(1, 0, 0);
	#else
		tangent = _in.tangent;
	#endif
	tangent = normalize(mul(tangent, world));
 #endif

#endif //SKIN

#ifdef WIND
	float a = sin(windTime * DOUBLE_PI) * windScale;
	posW.xy += windDir * a;
#endif

	_out.position = mul(float4(posW, 1), viewProj);
#ifdef REAL_REFLECTION
	_out.position_proj = _out.position;
#endif

	_out.pos_world = posW;

#ifdef HAZE
	_out.haze = CalcHaze(posW);
#endif

#ifdef BUMP
	binormal = cross(tangent, normal);
	#ifndef TEX_BLEND
		binormal *= -_in.tangent.w;
	#endif
	_out.tangent = tangent;
	_out.binormal = normalize(binormal);
#endif
#ifdef NEED_NORMAL
	_out.normal = normal * 0.5f + 0.5f;
#endif

#ifdef USE_VCOLOR
  #ifdef VCOLOR
	_out.color = _in.color;
  #else
	_out.color = float4(1, 1, 1, 1);
  #endif
#endif

#if defined(SHADOW)
	CalcShadowInterpolants(posW, _out.smCoord_0_1, _out.smCoord_2_depth_falloff);
#endif 

#ifdef FOGMAP
	_out.fogmapCoord = CalcFogmapCoord(posW);
#endif

#ifdef UV_FROM_POSITION
  	_out.texCoord.x = dot(float4(posW, 1.0f), texXformX);
  	_out.texCoord.y = dot(float4(posW, 1.0f), texXformY);
#else
 #if TEXCOORDS > 0
 	_out.texCoord.x = dot(float3(_in.texCoord, 1), texXformX);
 	_out.texCoord.y = dot(float3(_in.texCoord, 1), texXformY);
 #endif
#endif

#if !defined(WIND_GLOBAL) && !defined(REAL_REFLECTION) && TEXCOORDS > 1
	_out.texCoord1 = _in.texCoord1;
#endif

	return _out;
}
