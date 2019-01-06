#ifdef FOGMAP
#define FOG_OF_WAR
#endif

#ifdef FOG_OF_WAR
#define FOGMAP //???
#ifdef FOGMAP
	float2 CalcFogmapCoord(float3 _posW)
	{
		return  mul(float4(_posW, 1), worldToFogmap).xy;
	}
 #endif
#endif
