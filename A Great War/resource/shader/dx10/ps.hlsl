//#define WATER_OPTICS

#include "shader_constants.h"
#include "shadow\interpolants.inc"
#include "light_declaration.inc"
#include "vs_out.inc"


cbuffer ps_scene : register(pscSceneGroup)
{
	float3		cameraOrigin : packoffset(pscCameraOrigin);
 #include "water_optics\wo_scene_constants.inc"
 #include "haze\ps_constants.inc"
 #include "shadow\ps_constants.inc"
 #include "fogofwar\ps_constants.inc"
};

cbuffer ps_mtl : register(pscMaterialGroup)
{
	float4 mtl_params	: packoffset(pscMaterialParams);
#ifdef FADEAWAY
	float2 fadeaway_params : packoffset(pscFadeawayParams);
#endif
#ifdef PARALLAX
	float4 parallax_params0 : packoffset(pscParallaxParams(0));
 #ifdef LAYER1
	float4 parallax_params1 : packoffset(pscParallaxParams(1));
 #endif
#endif
#ifdef PARALLEL_LIGHT
	float4		light_parallel_dir		: packoffset(pscLightDir);	
	float4		light_parallel_diffuse	: packoffset(pscLightDiffuse);
	float4		light_ambient			: packoffset(pscLightAmbient);		
#endif
#ifdef FAKE_TRANSLUCENCY
	float4		translucency_color_amount: packoffset(pscTranslucencyParams);		//rgb -> color, a -> amount
#endif
 #include "material_constants.inc"
 #include "water_optics\wo_material_constants.inc"
};

cbuffer ps_lights : register(pscDynamicLightsGroup)
{
	eLightDesc local_lights[MAX_LIGHTS_PER_PASS] : packoffset(pscDynamicLight0);
};

static float3 view_dir = 0;
static float view_dist = 0;

#include "haze\ps.inc"
#include "fogofwar\ps.inc"
#include "shadow\ps.inc"
#include "light.inc"
#include "parallax.inc"

#ifdef TEX_BLEND
#include "const.inc"
#endif

#define mod_x mtl_params.x
#define alpha_ref mtl_params.w
#include "material.inc"

#ifdef FADEAWAY
#define fadeaway_start fadeaway_params[0]
#define fadeaway_depth fadeaway_params[1]
#endif

#if defined(ALPHA_TEST_DITHER)
 #include "dithered_opacity.inc"
#endif

#include "water_optics\wo_functions.inc"
#include "soft_intersection.inc"

float4 PS(vsOut _in) : SV_TARGET
{
	view_dir = cameraOrigin - _in.pos_world;
	view_dist = length(view_dir);
	view_dir /= view_dist;

	eMaterialOutput mtl = CalcMaterial(_in);
	float4 clr = mtl.diffuse;

#ifdef WATER_OPTICS
	WaterInfo wInfo;
	clr.rgb = CalculateWaterOptics(clr.rgb, mtl, _in, wInfo);
#endif

#ifdef LIGHT
	eLightOutput lights = CalcLightOutput(_in, mtl);
	clr.rgb *= lights.diffuse + lights.ambient;
	clr.rgb += lights.specular;
#endif //LIGHT
#ifndef WATER_OPTICS
	clr.rgb *= mod_x;
#endif
 	#if !defined(NEED_ALPHA)
 		clr.a  = 1;	// optimizer will remove unneeded alpha calculations made before this point
 	#else
		clr.a = mtl.diffuse.a;
		#if defined(FADEAWAY)
			if(view_dist > fadeaway_start)
			{
				clr.a *= saturate(1.0f - (view_dist - fadeaway_start) / fadeaway_depth);
			}
		#endif
		#if defined(ALPHA_TEST)
			clip(clr.a - alpha_ref);
		#elif defined(ALPHA_TEST_EQUAL)
			clip(abs(clr.a - alpha_ref) > 1/255.0f ? -1 : 1);
		#elif defined(ALPHA_TEST_DITHER)
			#if defined(FADEAWAY)
				if(view_dist < fadeaway_start)
					clip(clr.a - alpha_ref);
				else
			#endif//FADEAWAY
			ClipDithered(_in.position, clr.a);
		#endif//ALPHA_TEST
	#endif//NEED_ALPHA
#ifdef WATER_OPTICS		//water has its own "soft intersection".
	//clr.rgb += mtl.emissive * reflectionAmount;
	clr.rgb = wInfo.costalAlpha * clr.rgb + (1 - wInfo.costalAlpha) * wInfo.bareSceneColor;
	clr.a = 1;
	clr.rgb += mtl.emissive * reflectionAmount * wInfo.costalAlpha;
#else
	clr.rgb += mtl.emissive;
	#if defined(SOFT_INTERSECT)
		clr.a *= CalculateSoftIntersection(_in.position.w, _in.position.xy);
	#endif
#endif


	#if defined(ADDITIVE_BLEND)
		clr.rgb *= clr.a;
	#endif


 	ApplyFog(clr.rgb, _in);

	#ifdef HAZE
		#ifdef WATER_OPTICS
			ApplyHazeWaterOptics(clr.rgb, _in.haze, wInfo.costalAlpha, wInfo.bareSceneColor);
		#else
			ApplyHaze(clr.rgb, _in.haze);
		#endif
	#endif
	return clr;
}
