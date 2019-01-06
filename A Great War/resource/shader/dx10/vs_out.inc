#if !defined(NEED_NORMAL) && (defined(LIGHT) || defined(BUMP))
  #define NEED_NORMAL
#endif

#ifdef FOG_OF_WAR
#define FOGMAP //???
#endif

struct vsOut
{
	float4 position	: SV_POSITION;
#if TEXCOORDS > 0
	float2 texCoord	: TEXCOORD0;
#endif
#ifdef REAL_REFLECTION
	float4 position_proj : TEXCOORD8;
#elif !defined(WIND_GLOBAL) && TEXCOORDS > 1
	float2 texCoord1 : TEXCOORD8;
#endif
	float3 pos_world: TEXCOORD1;
#ifdef HAZE
	float  haze		: FOG;
#endif

#ifdef NEED_NORMAL
	float3 normal	: TEXCOORD2;
#endif
#ifdef BUMP
	float3 tangent	: TEXCOORD3;
	float3 binormal	: TEXCOORD4;
#endif

#ifdef USE_VCOLOR
	float4 color	: COLOR0;
#endif

#ifdef SHADOW
	float4 smCoord_0_1			  : TEXCOORD5;
	float4 smCoord_2_depth_falloff: TEXCOORD6;
#endif

#ifdef FOGMAP
	float2 fogmapCoord : TEXCOORD7;
#endif

};