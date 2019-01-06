static float3 light_dir = 0;
static const float EPSILON = 0.001f;

float CalcLightAttenuation(eLightDesc light, float3 pos_world, float3 normal)
{
	float k = 0;
	light_dir = light.position.xyz - pos_world;
	float l_dist = length(light_dir);
	light_dir /= l_dist;
	k = max(dot(normal, light_dir), 0);
	k *= saturate(light.range_att.y + l_dist * light.range_att.x);
	if(light.diffuse.w > 0)//spot light
	{
		float cosA = dot(-light.dir.xyz, light_dir);
		if(cosA > light.diffuse.w)
		{
			k *= pow((cosA - light.diffuse.w) * light.position.w, light.dir.w);
		}
		else
			k = 0;
	}
	return k;
}

float3 CalcSpecular(float k, eMaterialOutput mtl)
{
	float3 halfVec = normalize(light_dir + view_dir);
	float spec_k = max(dot(mtl.normal, halfVec), 0);
	spec_k = pow(spec_k, mtl.specular_power);
	return mtl.specular * k * spec_k;
}

void ApplyLocalLight(int id, float3 pos_world, inout eLightOutput _out, eMaterialOutput mtl)
{
	eLightDesc light = local_lights[id];
	float k = CalcLightAttenuation(light, pos_world, mtl.normal);
	_out.diffuse  += light.diffuse.rgb * k;

#if defined(SPEC) || defined(WATER)
	_out.specular += CalcSpecular(k, mtl);
#endif
};

eLightOutput CalcLightOutput(vsOut _in, eMaterialOutput mtl)
{
	eLightOutput _out;
	_out.diffuse	 = 0;
	_out.specular = 0;
	_out.ambient	 = 0;
	float k = 1;
	float3 diffuse;
	float3 specular = 0;
#ifdef PARALLEL_LIGHT
	light_dir = light_parallel_dir.xyz;
	k = max(dot(mtl.normal, light_dir), 0);

	diffuse = light_parallel_diffuse.rgb * k;

	//Taking irradiance into acount when calculating the diffuse lighting for foliage by reversing the pixels normal and dotting it with light
	//direction. The resulting factor is used to add an additional light diffusion to the diffsue light and therefore
	//simulating transluency with out the cost of addtional subsurface scattering calculations.
	#ifdef FAKE_TRANSLUCENCY
		#ifdef SHADOW		//Added this constraint to avoid over compensating when shadows are turned off.
			float transmittance =  max(dot(-mtl.normal, light_parallel_dir.xyz), 0.f);
			float3 transmittanceColor = translucency_color_amount.r < 0 ? light_parallel_diffuse.rgb : (light_parallel_diffuse.rgb + translucency_color_amount.rgb) * 0.5f;  
			diffuse += transmittanceColor * transmittance * translucency_color_amount.a;
		#endif
	#endif
	
	
	#if defined(SPEC) || defined(WATER)
		specular = CalcSpecular(k, mtl);
	#endif

	#ifdef SHADOW
		SHADOW_TERM;
		diffuse  *= shadowTerm;
		specular *= shadowTerm;
	#endif
	#ifdef RIM_LIGHT
		float rim_k = dot(mtl.normal, view_dir);
		float3 helper_rim = float3(0.1f, 0.3f, 1.0f);
		float rimLight =  helper_rim.x * smoothstep(helper_rim.y, helper_rim.z, 1 - rim_k);
		diffuse += rimLight;
	#endif
	_out.diffuse  += diffuse;
	_out.specular += specular;
	_out.ambient  = light_ambient.rgb;
#endif
 	if(light0)
 		ApplyLocalLight(0, _in.pos_world, _out, mtl);
 	if(light1)
 		ApplyLocalLight(1, _in.pos_world, _out, mtl);
	if(light2)
		ApplyLocalLight(2, _in.pos_world, _out, mtl);
	if(light3)
		ApplyLocalLight(3, _in.pos_world, _out, mtl);
	if(light4)
		ApplyLocalLight(4, _in.pos_world, _out, mtl);
	if(light5)
		ApplyLocalLight(5, _in.pos_world, _out, mtl);
	if(light6)
		ApplyLocalLight(6, _in.pos_world, _out, mtl);
	if(light7)
		ApplyLocalLight(7, _in.pos_world, _out, mtl);
	return _out;
}