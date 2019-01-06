#ifdef SHADOW
	float4x4	shadow_viewproj[SM_SLICES] : packoffset(vscShadowViewProjLight0);
	float4		shadow_attenuation : packoffset(vscShadowAttenuation);
#endif//SHADOW
