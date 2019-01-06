#include "tools.inc"

#ifdef FOGMAP
#define FOG_OF_WAR
#endif

#ifdef FOG_OF_WAR
#define FOGMAP //???
 #ifdef FOGMAP
	Texture2D texFogmap : register(t14);
	SamplerState samFogmap : register(s14);
 #endif
		
void ApplyFog(inout float3 color, vsOut _in) 
{
	float fog_amount = fogAmount.w;
	float fogK = 0;									
 #ifdef FOGMAP
	fogK = texFogmap.Sample(samFogmap, _in.fogmapCoord).a;
	fog_amount = saturate(fog_amount + fogK);
#endif
	float lumK = 1.0f - fog_amount;									
	float l = Luminance(color.rgb);                 
	color = lerp(color.rgb, l, lumK) * lerp(fogBrightness.w, 1, fogK);	
}
#else
void ApplyFog(inout float3 color, vsOut _in) {}
#endif
