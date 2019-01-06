#if defined(SHADOW)

	float smSliceDepth0 : packoffset(pscShadowSliceDepth.x);
	float smSliceDepth1 : packoffset(pscShadowSliceDepth.y);
	float smSliceDepth2 : packoffset(pscShadowSliceDepth.z);
	float smSliceBlendDepth : packoffset(pscShadowSliceDepth.w);
	float2 shadowmap_size : packoffset(pscShadowmapParams);
	float shadow_depth_bias : packoffset(pscShadowmapParams.z);

#endif// SHADOW
