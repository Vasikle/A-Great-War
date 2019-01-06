#ifdef HAZE
void ApplyHaze(inout float3 color, float _haze)
{
#ifdef ADDITIVE_BLEND
	color *= saturate(_haze);
#else	
	color = lerp(haze_color.rgb, color, saturate(_haze));
#endif
}
#endif
