#include "shader_constants.h"
#include "const.inc"

struct vsOut
{
	float4 position : SV_POSITION;
#ifndef OPAQUE
	float2 texCoord : TEXCOORD0;
#endif
};

#ifndef PIXEL_SHADER

 #ifdef WIND_GLOBAL
  #define WIND
 #endif

 #include "vs_in.inc"

 cbuffer vs_scene : register(vscSceneGroup)
 {
	float4x4	viewProj : packoffset(vscViewProj);
#if defined(WIND)
	float2 		windDir : packoffset(vscWindParams);
  	float 		windTimeBase : packoffset(vscWindParams.z);
#endif //WIND
 };

 cbuffer vs_primitive : register(pscMaterialGroup)
 {
 #if !defined(SKIN) && !defined(INSTANCED)
	float4x3	world : packoffset(pvscWorld0);
 #endif
#if defined(WIND) && !defined(WIND_GLOBAL) && !defined(INSTANCED)
	float 		windPhase	: packoffset(pvscWindParamsPrim);
#endif //WIND
 };
 cbuffer vs_primitive_misc : register(vscPrimitiveMiscGroup)
 {
 #ifndef OPAQUE
	float3		texXformX : packoffset(vscTexXformX);
	float3		texXformY : packoffset(vscTexXformY);
 #endif
 };

 #ifdef SKIN
 cbuffer vs_skin : register(vscSkinGroup)
 {
	float4x3	world[MAX_SKIN_BONE_COUNT] : packoffset(vscWorld0Skinned);
 };
 #endif//SKIN

 #if defined(INSTANCED)
  #define 	world _in.inst_world
  #define 	windPhase _in.inst_wind
 #endif//INSTANCED

 #ifdef WIND
  #ifdef WIND_GLOBAL
   #define windScale _in.windPars.x
   #define windTime (windTimeBase + _in.windPars.y)
  #else
   #define windScale _in.position.z
   #define windTime (windTimeBase + windPhase)
  #endif
 #endif

vsOut VS(vsIn _in)
{
	vsOut _out;
	float3 posW = _in.position;
#ifdef SKIN
    uint4 indexVector = _in.blendIndex;
	float3 posW0 = mul(float4(posW, 1), world[indexVector[0]]);
	float3 posW1 = mul(float4(posW, 1), world[indexVector[1]]);
	posW = lerp(posW1, posW0, _in.blendWeight);
#else
	posW = mul(float4(posW, 1), world).xyz;
#endif

#ifdef WIND
	float a = sin(windTime * DOUBLE_PI) * windScale;
	posW.xy += windDir * a;
#endif

	_out.position = mul(float4(posW, 1), viewProj);
	float depth = max(_out.position.z / _out.position.w, 1.0e-5f);
	_out.position.z = depth * _out.position.w;

#ifndef OPAQUE
	_out.texCoord.x = dot(_in.texCoord, texXformX);
	_out.texCoord.y = dot(_in.texCoord, texXformY);
#endif

	return _out;
}

#else//PIXEL_SHADER

 #ifndef OPAQUE
	Texture2D 	texDiffuse: register(t0);
	SamplerState 	samDiffuse: register(s0);
 #endif

void PS(vsOut _in)
{
 #ifndef OPAQUE
	float alpha = texDiffuse.Sample(samDiffuse, _in.texCoord).a;
	clip(alpha - 0.5f);
 #endif //OPAQUE
}

#endif//PIXEL_SHADER
