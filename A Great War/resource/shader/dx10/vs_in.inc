struct vsIn
{
    float3 position	: POSITION;
#ifdef VSKIN
	float blendWeight : BLENDWEIGHT;
	uint4 blendIndex : BLENDINDICES;
#endif
#ifdef VNORMAL
	float3 normal	: NORMAL;
#endif
#ifdef VCOLOR
	float4 color	: COLOR0;
#endif
#if VTEXCOORDS > 0
    float2 texCoord	: TEXCOORD0;
 #if VTEXCOORDS > 1
    float2 texCoord1	: TEXCOORD1;
 #endif
#endif
#ifdef WIND_GLOBAL
    float2 windPars	: TEXCOORD7; //scale, phase
#endif

#if defined(VTANGENT)
	float4 tangent	: TANGENT;
#endif

// instance data
#if defined(INSTANCED)
	float4x3 inst_world	: TEXCOORD4; // TEXCOORD4:TEXCOORD6
 #if defined(WIND)
	float inst_wind	: TEXCOORD7; // phase
 #endif
#endif

};
