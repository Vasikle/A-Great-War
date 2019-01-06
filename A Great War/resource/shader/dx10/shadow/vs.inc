#ifdef SHADOW

 float _FalloffByDistance(float distance)
 {
	return 1 - saturate((shadow_attenuation.y - distance) * shadow_attenuation.x * 2);
 }

  void CalcShadowInterpolants(float3 _posW, out float4 _smc_0_1, out float4 _smc_2_df)
  {
	_smc_0_1 = 0;
	_smc_2_df = 0;
	float4 smc0 = mul(float4(_posW, 1), shadow_viewproj[0]);
	_smc_0_1.xy = smc0.xy;
#if SM_SLICES > 1
	float4 smc1 = mul(float4(_posW, 1), shadow_viewproj[1]);
	_smc_0_1.zw = smc1.xy;
 #if SM_SLICES > 2
	float4 smc2 = mul(float4(_posW, 1), shadow_viewproj[2]);
	_smc_2_df.xy = smc2.xy;
 #endif
#endif
	_smc_2_df.z = smc0.z;
	_smc_2_df.w = _FalloffByDistance(length(cameraOrigin - _posW));
  }

#endif//SHADOW
