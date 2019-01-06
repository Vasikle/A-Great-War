#ifdef LAYER1
#define LAYERED
#endif

#if defined(SOFT_INTERSECT) || defined(WATER_OPTICS)
Texture2D vsDepth_BUFFER : register(t10);
SamplerState vsDepth_SAMPLER : register(s10);
#endif

#ifdef ENVMAP
 #ifdef WATER
  #ifdef REAL_REFLECTION
	Texture2D texEnvmap		: register(t2);
  #else
	TextureCube texEnvmap	: register(t2);
  #endif
	SamplerState samEnvmap	: register(s2);
 #else
	TextureCube texEnvmap	: register(t1);
	SamplerState samEnvmap	: register(s1);
	#if defined(MASK)
		Texture2D texMask	: register(t2);
		SamplerState samMask: register(s2);
	#endif
 #endif
#endif //ENVMAP

#ifdef NEED_NORMAL
static	float3 normal_w;
#endif

Texture2D texDiffuse0 : register(t0);
SamplerState samDiffuse0 : register(s0);

#ifdef BUMP
	Texture2D texNormal0 : register(t1);
	SamplerState samNormal0 : register(s1);
	static	float3 tangent_w;
	static	float3 binormal_w;
float3 FromTangentSpace(float3 ts)
{
	return tangent_w * ts.x + binormal_w * ts.y + normal_w * ts.z;
}
float3 ToTangentSpace(float3 ws)
{
	return float3(dot(ws, tangent_w), dot(ws, binormal_w), dot(ws, normal_w));
}
#endif

#ifdef SPEC
 	Texture2D texSpecular0 : register(t2);
 	SamplerState samSpecular0 : register(s2);
#endif

#if defined(TEXMUL) || defined(TEXMORPH)
	Texture2D texDiffuse1 : register(t1);
	SamplerState samDiffuse1 : register(s1);
#endif

#ifdef LAYER1
	Texture2D texDiffuse1 : register(t4);
	SamplerState samDiffuse1 : register(s4);
 #ifdef BUMP
	Texture2D texNormal1 : register(t5);
	SamplerState samNormal1 : register(s5);
 #endif
 #ifdef SPEC
	Texture2D texSpecular1 : register(t6);
	SamplerState samSpecular1 : register(s6);
 #endif
#endif

float3 UnpackNormal(float4 n)
{
#ifdef WATER
 	return n.rgb * 2 - 1;
#else
	return n.agb * 2 - 1;
#endif
}

#ifdef LAYERED
#define CONDITION(name, id) if(name##id)
#else
#define CONDITION(name, id)
#endif

#ifdef PARALLAX
#define CALC_PARALLAX(id)	\
	CONDITION(prlx, id)		\
	{						\
		ApplyParallax(tc, view_dir_ts, k_steps, texHeight##id, samHeight##id, parallax_params##id);\
	}
#else
#define	CALC_PARALLAX(id)
#endif

#ifdef DIFFUSE
#if defined(TEXMUL)
#define CALC_DIFFUSE(id)									\
	float4 dif0 = texDiffuse0.Sample(samDiffuse0, tc_base);	\
	float4 dif1 = texDiffuse1.Sample(samDiffuse1, tc_base1);\
	mtl.diffuse = dif0 * dif1 * mtl_params.y;		
#elif defined(TEXMORPH)
#define CALC_DIFFUSE(id)									\
	float4 dif0 = texDiffuse0.Sample(samDiffuse0, tc_base);	\
	float4 dif1 = texDiffuse1.Sample(samDiffuse1, tc_base1);\
	mtl.diffuse = lerp(dif1, dif0, morphK.x);		
#else
#define CALC_DIFFUSE(id)	\
	mtl.diffuse += texDiffuse##id.Sample(samDiffuse##id, tc) * k##id; 
#endif
#elif defined(WATER)
#define CALC_DIFFUSE(id)	\
	mtl.diffuse = waterDiffuse;
#else
#define CALC_DIFFUSE(id)	\
	mtl.diffuse = 1.0f;
#endif //DIFFUSE

#ifdef ENVMAP
 #if defined(MASK)
	#define	ENV_AMOUNT texMask.Sample(samMask, tc).rgb
 #else
	#define	ENV_AMOUNT env_amount.rgb
 #endif

//*FH: i need the reflect amount in he output for blending refraction with reflection.
//	   Therfore, i use a float 4 if water is defined and store the reflection amount in the .a/.w channel
#ifdef WATER_OPTICS
float4 CalcEmissive(vsOut _in, float3 mtl_normal, float2 tc)
{
#else
float3 CalcEmissive(vsOut _in, float3 mtl_normal, float2 tc)
{
#endif
 #ifdef REAL_REFLECTION
	float2 uv = (_in.position_proj.xy / _in.position_proj.w) * float2(0.5, -0.5) + 0.5f;
	float reflection_scale = 0.1f;
	uv += mtl_normal.xy * reflection_scale;
	float3 reflection = texEnvmap.Sample(samEnvmap, uv).rgb;
 #else
	float3 tc_cube = reflect(view_dir, mtl_normal).xzy;
	tc_cube.xy *= -1;
	float3 reflection = texEnvmap.Sample(samEnvmap, tc_cube).rgb;
 #endif
	float reflect_amount = 1.0f;
 #ifdef WATER
	reflect_amount = 1.0f - dot(mtl_normal, view_dir);
// 	float fresnel_scale = 1.0f;
// 	float fresnel_power = 1.0f;
// 	reflect_amount = pow(abs(reflect_amount) * fresnel_scale, fresnel_power);
 #endif
 #ifdef WATER_OPTICS
	return float4(reflection * reflect_amount * ENV_AMOUNT,reflect_amount);
 #else
	return reflection * reflect_amount * ENV_AMOUNT;
 #endif
}
#endif //ENVMAP

#ifdef LIGHT
#ifdef BUMP
#define CALC_BUMP(id)	\
	float3 normal##id = normal_w;		\
 	if(bump##id)						\
	{									\
		normal##id = UnpackNormal(texNormal##id.Sample(samNormal##id, tc));	\
		normal##id = FromTangentSpace(normal##id);	\
	}									\
	mtl.normal += normal##id * k##id;
#else
#define	CALC_BUMP(id)				\
	mtl.normal += normal_w * k##id;
#endif //BUMP

#ifdef SPEC
#define	CALC_SPEC(id)									\
	float3 specular##id = 0;							\
	CONDITION(spec, id)									\
	{													\
		specular##id = lightSpecular##id.rgb * texSpecular##id.Sample(samSpecular##id, tc).r;	\
	}													\
	mtl.specular += specular##id * k##id;				\
	mtl.specular_power += lightSpecular##id.a * k##id;
#elif defined(WATER)
#define	CALC_SPEC(id)			\
	mtl.specular = 1.0f;		\
	mtl.specular_power = 1000.0f;
#else
#define	CALC_SPEC(id)
#endif //SPEC
#else
#define	CALC_BUMP(id)
#define	CALC_SPEC(id)
#endif //LIGHT

eMaterialOutput CalcMaterial(vsOut _in)
{
	eMaterialOutput mtl;
	mtl.normal				= 0;
	mtl.diffuse				= 0;
	mtl.emissive			= 0;
	mtl.specular			= 0;
	mtl.specular_power		= 0;
	mtl.reflectionAmount	= 0;

#if TEXCOORDS > 0
	float2 tc_base = _in.texCoord;
#ifdef TEX_BLEND
	tc_base *= TEXTURTILING;
#endif
	float2 tc_base1 = tc_base;
#if !defined(REAL_REFLECTION) && TEXCOORDS > 1
	tc_base1 = _in.texCoord1;
#endif
	float2 tc = tc_base;
#endif

#ifdef NEED_NORMAL
	normal_w = normalize(_in.normal * 2 - 1);
#endif
#ifdef BUMP
	tangent_w = normalize(_in.tangent);
	binormal_w =	normalize(_in.binormal);
#endif

	float k0 = 1;
#ifdef LAYERED
	k0 = _in.color.a;
#endif

#ifdef PARALLAX
	float3 view_dir_ts = ToTangentSpace(view_dir);
	float k_steps = dot(view_dir, normal_w);
#endif

	CALC_PARALLAX(0);
	CALC_DIFFUSE(0);	
 	CALC_BUMP(0);
 	CALC_SPEC(0);

#ifdef LAYER1
	float k1 = 1 - _in.color.a;
	tc = tc_base;
	CALC_PARALLAX(1);
	CALC_DIFFUSE(1);	
	CALC_BUMP(1);
	CALC_SPEC(1);
#endif
	mtl.normal = normalize(mtl.normal);
#ifdef ENVMAP
	#ifdef WATER_OPTICS
		float4 emissionNreflection = CalcEmissive(_in, mtl.normal, tc);
		mtl.emissive = emissionNreflection.rgb;
		mtl.reflectionAmount = emissionNreflection.a;
	#else
		mtl.emissive = CalcEmissive(_in, mtl.normal, tc);
	#endif
#endif

#ifdef TFACTOR
	mtl.diffuse *= tFactor;
#endif	

#ifdef USE_VCOLOR
	mtl.diffuse *= _in.color;
#endif	

	return mtl;
}