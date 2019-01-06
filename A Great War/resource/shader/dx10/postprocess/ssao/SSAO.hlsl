
#include "shader_constants.h"

struct vsIn
{
	float4 position : POSITION;
	float2 texCoord0 : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
};

struct vsOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

#ifdef PIXEL_SHADER

Texture2D		depth_Buffer : register(t0);
Texture2D		noise_Buffer : register(t1);
SamplerState	bilinear_Sampler : register(s0);

#ifndef SSAO_DEBUG
#define power 1.f
#define radius 8.f
#define sampleCount 16
#endif

cbuffer SSAO_PS_Parameters : register(pscSceneGroup)
{
#ifdef SSAO_DEBUG
	float power				: packoffset(pscSSAOParams.x);
	float radius			: packoffset(pscSSAOParams.y);
	float sampleCount		: packoffset(pscSSAOParams.z);
#endif
	float hazeScale			: packoffset(pscSSAOParams2.x);
	float hazeEnd			: packoffset(pscSSAOParams2.y);
	float hazeBoundWidth	: packoffset(pscSSAOParams2.z);
	
	float3 cameraOrigin		: packoffset(pscCameraOrigin);
	float4 hazeBound		: packoffset(pscHazeColor);

	float4x4 projM			: packoffset(pscViewProj);
	float4x4 invProjM		: packoffset(pscInvViewProj);
	float4x4 invViewM		: packoffset(pscInvView);
}

float3 GetViewPos(float2 tc)
{
 	float depth = depth_Buffer.Sample(bilinear_Sampler, tc ).r;
	float4 view_ray = mul(float4(tc * 2 - 1, 1, 1), invProjM);
	view_ray.y = -view_ray.y;
	view_ray /= view_ray.w;
	return view_ray.xyz * (depth / view_ray.z);
}

float3 GetWorldPos(float3 viewPos)
{
	return mul(float4(viewPos, 1), invViewM).xyz;
}

float3 GetNormal(float2 texcoords,float width,float height) 
{

	const float2 offset1 = float2(0.0, 1.f/height);
	const float2 offset2 = float2(1.f/width, 0.0);

	float3 base = GetViewPos(texcoords);

	float3 p11 = GetViewPos(texcoords - offset1);
	float3 p12 = GetViewPos(texcoords + offset1);
	float3 p21 = GetViewPos(texcoords - offset2);
	float3 p22 = GetViewPos(texcoords + offset2);
	
	float3 normal1 = cross(normalize(p21 - base), normalize(p11 - base));
	float3 normal2 = cross(normalize(p11 - base), normalize(p22 - base));
	float3 normal3 = cross(normalize(p22 - base), normalize(p12 - base));
	float3 normal4 = cross(normalize(p12 - base), normalize(p21 - base));

	return (normal1 +normal2 + normal3 + normal4) / 4.f;
}


float CalcHaze(float3 posW)
{
	float3 viewDir = cameraOrigin - posW;
	float viewDist = length(viewDir);
	float haze = (hazeEnd - viewDist) * hazeScale;
#ifdef BOUNDARY_HAZE
	haze = saturate(haze);
	float k1, k2;
	k1 = max(min(posW.x - hazeBound.x, hazeBound.z - posW.x), 0);
	k2 = max(min(posW.y - hazeBound.y, hazeBound.w - posW.y), 0);
	if (k1 < hazeBoundWidth) haze *= k1 / hazeBoundWidth;
	if (k2 < hazeBoundWidth) haze *= k2 / hazeBoundWidth;
#endif
	return haze;
}

static const uint SPHERE_SAMPLES = 16;
//Used as base for calculating a sample kernel
static const float3 sample_sphere[SPHERE_SAMPLES] =
{
	float3( 0.5381, 0.1856,-0.4319), float3( 0.1379, 0.2486, 0.4430),
	float3( 0.3371, 0.5679,-0.0057), float3(-0.6999,-0.0451,-0.0019),
	float3( 0.0689,-0.1598,-0.8547), float3( 0.0560, 0.0069,-0.1843),
	float3(-0.0146, 0.1402, 0.0762), float3( 0.0100,-0.1924,-0.0344),
	float3(-0.3577,-0.5301,-0.4358), float3(-0.3169, 0.1063, 0.0158),
	float3( 0.0103,-0.5869, 0.0046), float3(-0.0897,-0.4940, 0.3287),
	float3( 0.7119,-0.0154,-0.0918), float3(-0.0533, 0.0596,-0.5411),
	float3( 0.0352,-0.0631, 0.5460), float3(-0.4776, 0.2847,-0.0271)
};

float4 PS_SSAO(vsOut _in) : SV_Target
{ 
	float3 view_pos = GetViewPos(_in.texCoord);
	
	float width, height;
	depth_Buffer.GetDimensions(width, height);

	float3 normal = normalize(GetNormal(_in.texCoord, width, height));

	//return float4(normal * 0.5 + 0.5,1.f);
	
	float2 texScale = float2(width / 4, height / 4);


	float3 random = float3(normalize( noise_Buffer.Sample(bilinear_Sampler, _in.texCoord * texScale).rg * 2 - 1 ),0);
	float occlusion = 0.0;
#ifndef SSAO_DEBUG
	[unroll]
#endif
	for (int i = 0; i < sampleCount; ++i)
	{
		float3 random_dir = reflect(sample_sphere[i % SPHERE_SAMPLES], random);

		float3 hemiRay = sign(dot(random_dir, normal)) * random_dir * radius;
		//float useHemiRay = dot(normal, normalize(hemiRay));
		//useHemiRay = useHemiRay > 0.25 ? 1.f : 0.f;
		float3 ssample = view_pos + hemiRay;
  
		// project sample position:
		float4 offset = float4(ssample, 1.0);
		offset = mul(offset, projM);
		offset.xy /= offset.w;
		offset.y = -offset.y;
		offset.xy = offset.xy * 0.5 + 0.5;
  
		// get sample depth:
		float sampleDepth = depth_Buffer.Sample(bilinear_Sampler, offset.xy).r;

		// range check & accumulate:
		float thres = 0.5f;
		float rangeCheck= abs(view_pos.z - sampleDepth) < radius ? 1.0 : 0.0;
		occlusion += (sampleDepth + thres < ssample.z ? 1.0 : 0.0) *rangeCheck;
	}
	float haze = saturate(CalcHaze(GetWorldPos(view_pos)));
	occlusion = 1.f - (occlusion / ( sampleCount ));
	
	float occ_y = abs(ddy(occlusion));
	float occ_x = abs(ddx(occlusion));

	occlusion = lerp(1, pow(abs(occlusion), power), haze);

	//occlusion += max(occ_y, occ_x);
	return saturate(occlusion);

}
#else

vsOut VS_SSAO(vsIn _in)
{
	vsOut _out;
	_out.position = _in.position;
	_out.texCoord = _in.texCoord1;
	return _out;
}

#endif //PIXEL_SHADER


