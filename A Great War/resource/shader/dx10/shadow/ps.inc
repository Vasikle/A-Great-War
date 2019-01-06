#if defined(SHADOW)

#ifdef USING_TEX_ARRAY

 Texture2DArray tex_shadowmap0 : register(t11);
 SamplerComparisonState sam_shadowmap0 : register(s11);

#else//USING_TEX_ARRAY

 Texture2D tex_shadowmap0 : register(t11);
 SamplerComparisonState sam_shadowmap0 : register(s11);

 #if SM_SLICES > 1
   Texture2D tex_shadowmap1 : register(t12);
   SamplerComparisonState sam_shadowmap1 : register(s12);
  #if SM_SLICES > 2
    Texture2D tex_shadowmap2 : register(t13);
    SamplerComparisonState sam_shadowmap2 : register(s13);
  #endif
 #endif

#endif//USING_TEX_ARRAY

#ifdef SHADOW_ULTRA
 Texture2D tex_rnd_rotation : register(t15);
 SamplerState sam_rnd_rotation : register(s15);
 #define JITTER_SAMPLE_COUNT 8
 static float2 irreg_kernel[JITTER_SAMPLE_COUNT] = {
	float2(-0.072, -0.516),	float2(-0.105, 0.989),
	float2(0.949, 0.258),	float2(-0.966, 0.216),
	float2(0.784, -0.601),	float2(0.139, 0.230),
	float2(-0.816, -0.516),	float2(0.529, 0.779)
 };
#endif//SHADOW_ULTRA

bool InRange(float x, float a, float b)
{
	return x >= a && x <= b;
}

#ifdef USING_TEX_ARRAY

#define _SAMPLE_SM0 tex_shadowmap0.SampleCmpLevelZero(sam_shadowmap0, float3(_in.smCoord_0_1.xy + offset, 0), _in.smCoord_2_depth_falloff.z)
#define _SAMPLE_SM1 tex_shadowmap0.SampleCmpLevelZero(sam_shadowmap0, float3(_in.smCoord_0_1.zw + offset, 1), _in.smCoord_2_depth_falloff.z)
#define _SAMPLE_SM2 tex_shadowmap0.SampleCmpLevelZero(sam_shadowmap0, float3(_in.smCoord_2_depth_falloff.xy + offset, 2), _in.smCoord_2_depth_falloff.z)

#else//USING_TEX_ARRAY

#define _SAMPLE_SM0 tex_shadowmap0.SampleCmpLevelZero(sam_shadowmap0, _in.smCoord_0_1.xy + offset, _in.smCoord_2_depth_falloff.z);
#define _SAMPLE_SM1 tex_shadowmap1.SampleCmpLevelZero(sam_shadowmap1, _in.smCoord_0_1.zw + offset, _in.smCoord_2_depth_falloff.z);
#define _SAMPLE_SM2 tex_shadowmap2.SampleCmpLevelZero(sam_shadowmap2, _in.smCoord_2_depth_falloff.xy + offset, _in.smCoord_2_depth_falloff.z);

#endif//USING_TEX_ARRAY

float ShadowSample(vsOut _in, float2 offset)
{
	float cam_depth = view_dist;
	float SAMPLE_SM0 = _SAMPLE_SM0;
#if SM_SLICES > 1
	float SAMPLE_SM1 = _SAMPLE_SM1;
 #if SM_SLICES > 2
	float SAMPLE_SM2 = _SAMPLE_SM2;
 #endif
#endif

#if SM_SLICES == 1
	return SAMPLE_SM0;
#elif SM_SLICES == 2
	if(cam_depth < smSliceDepth0 - smSliceBlendDepth)
	{
		return SAMPLE_SM0;
	}
	if(cam_depth >= smSliceDepth0)
	{
		return SAMPLE_SM1;
	}
	return lerp(SAMPLE_SM1, SAMPLE_SM0, (smSliceDepth0 - cam_depth) / smSliceBlendDepth);
#else
	if(InRange(cam_depth, 0, smSliceDepth1 - smSliceBlendDepth)) // 0 & 1 slices
	{
		if(cam_depth < smSliceDepth0 - smSliceBlendDepth)
		{
			return SAMPLE_SM0;
		}
		if(cam_depth >= smSliceDepth0)
		{
			return SAMPLE_SM1;
		}
		return lerp(SAMPLE_SM1, SAMPLE_SM0, (smSliceDepth0 - cam_depth) / smSliceBlendDepth);
	}
	// 1 & 2 slices
	if(cam_depth < smSliceDepth1 - smSliceBlendDepth)
	{
		return SAMPLE_SM1;
	}
	if(cam_depth >= smSliceDepth1)
	{
		return SAMPLE_SM2;
	}
	return lerp(SAMPLE_SM2, SAMPLE_SM1, (smSliceDepth1 - cam_depth) / smSliceBlendDepth);
#endif
}

float ShadowTerm(vsOut _in, float2 off)
{
	float o = shadowmap_size.y;
	float4 s;

#ifdef SHADOW_HIGH
	s.r = ShadowSample(_in, float2(off + float2(-o, -o)));
	s.g = ShadowSample(_in, float2(off + float2(0, -o)));
	s.b = ShadowSample(_in, float2(off + float2(o, -o)));
	s.a = ShadowSample(_in, float2(off + float2(-o, 0)));
	s.r += ShadowSample(_in, off);
	float4 s1;
	s1.r = ShadowSample(_in, float2(off + float2(o, 0)));
	s1.g = ShadowSample(_in, float2(off + float2(-o, o)));
	s1.b = ShadowSample(_in, float2(off + float2(0, o)));
	s1.a = ShadowSample(_in, float2(off + float2(o, o)));
	s += s1;
	return (s.r + s.g + s.b + s.a) * 0.111;
#else//SHADOW_HIGH
	return ShadowSample(_in, off);
#endif//SHADOW_HIGH
}

#ifdef SHADOW_ULTRA
float ShadowTermJittered(vsOut _in)
{
	float term = 0;
	for(int i = 0; i < JITTER_SAMPLE_COUNT; ++i)
	{
		float2 rot_sample = tex2D(tex_rnd_rotation, _in.smCoord_0_1.xy * 800);
		rot_sample = /*normalize*/(rot_sample * 2 - 1);
		float2 off;
		off.x = dot(irreg_kernel[i], float2(rot_sample.x, -rot_sample.y));
		off.y = dot(irreg_kernel[i], rot_sample.yx);
		term += ShadowTerm(_in, off * 0.0015);
	}
	return term / JITTER_SAMPLE_COUNT;
}
#endif//SHADOW_ULTRA

//=============================================================================
//	SHADOW_TERM
//-----------------------------------------------------------------------------
#ifdef SHADOW_ULTRA

#define SHADOW_TERM		\
  float shadowTerm = min(ShadowTermJittered(_in) + _in.smCoord_2_depth_falloff.w, 1);

#else//SHADOW_ULTRA

#define SHADOW_TERM		\
  float shadowTerm = min(ShadowTerm(_in, float2(0, 0)) + _in.smCoord_2_depth_falloff.w, 1);

#endif//SHADOW_ULTRA

#else //!SHADOW

 #define SHADOW_TERM

#endif// SHADOW
