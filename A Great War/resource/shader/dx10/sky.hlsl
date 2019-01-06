#include "shader_constants.h"

struct vsOut
{
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD0;
	float4 color 	: COLOR0;
};

#ifdef PIXEL_SHADER

Texture2D 		texEnvmap: register(t0);
SamplerState	samEnvmap: register(s0);

Texture2D		sunMoon_Buffer	: register(t1);
SamplerState	sunMoon_Sampler : register(s1);

cbuffer ps_scene : register(pscSceneGroup)
{
	float4	sunMoonPosRes	   : packoffset(pscLELightPos);		//*FH: x/y holds the sun coordinates in normalized screenspace. z/w is the device's actual resolution settings
	float	shouldDrawSunMoon  : packoffset(pscLEParams.x);		//*FH: should the sun/moon be drawn?
	float	sunMoonRadius	   : packoffset(pscLEParams.y);		//*FH: A factor for manipulating the size of the sun.
	float	isNight			   : packoffset(pscLEParams.z);		//*FH: Indicates if it is night or day in the environment settings
};
cbuffer ps_mtl : register(pscMaterialGroup)
{
	float4 mtl_params	: packoffset(pscMaterialParams);
	float4 tFactor		: packoffset(pscTFactor);
};

#define mod_x mtl_params.x

static const float sunMoonTexcoordShift = 0.5f; //moon : 0 - 255, sun : 256 - 511

float4 drawSun(float2 pxScreenCoord)
{

	//Light/Sun pos in texture coordinates
	float2 lightPos = float2(sunMoonPosRes.x, sunMoonPosRes.y);
	//The devices actual screen dimension/resolution
	float2 screenDim = float2(sunMoonPosRes.z, sunMoonPosRes.w);
	float aspectRatio = screenDim.x / screenDim.y;

	//project screen coordinates into normaliced screen coordinates
	pxScreenCoord.x /= screenDim.x;
	pxScreenCoord.y /= screenDim.y;
	
	 
	float radius = 0.035 * sunMoonRadius;

	float2 upperLeftBounds = float2(lightPos.x - radius / aspectRatio, lightPos.y - radius);
	
	float distanceToSun = pow((pxScreenCoord.x - lightPos.x) * aspectRatio, 2) + pow((pxScreenCoord.y - lightPos.y), 2);
	
	float samplePoint = radius * radius - distanceToSun > 0 ? 1 : 0;

	float2 sampleCoords = float2( (pxScreenCoord.x - upperLeftBounds.x),  (pxScreenCoord.y - upperLeftBounds.y));
	sampleCoords.y /= (radius * 2);
	sampleCoords.x /= (radius  * 4 / aspectRatio);
	sampleCoords.x += sunMoonTexcoordShift * (1 - isNight);
	
	float4 ssample = sunMoon_Buffer.Sample(sunMoon_Sampler, sampleCoords );


	//If not, we just return a black value and transparent value, since the sunMoon gets "added" to the screen space
	return lerp(float4(0, 0, 0, 0), ssample, samplePoint);
}

float4 PS(vsOut _in) : SV_TARGET
{
	float4 color = texEnvmap.Sample(samEnvmap, _in.uv) * _in.color;
	color.rgb *= mod_x;
	color *= tFactor;
	float4 sunMoonSample = drawSun(_in.position.xy);
	sunMoonSample = lerp(color, sunMoonSample, sunMoonSample.a);
	
	return lerp(color, sunMoonSample, shouldDrawSunMoon);
}

#else//PIXEL_SHADER

cbuffer vs_scene : register(vscSceneGroup)
{
	float4x4	viewProj	: packoffset(vscViewProj);
};

cbuffer vs_primitive : register(pscMaterialGroup)
{
	float4x3	world		: packoffset(pvscWorld0);
};

struct vsIn
{
	float3 position	: POSITION;
	float3 normal	: NORMAL;
	float4 color	: COLOR0;
	float2 texcoord	: TEXCOORD0;
};

vsOut VS(vsIn _in)
{
	vsOut _out;
	float3 posW = mul(float4(_in.position, 1), world).xyz;
	_out.position = mul(float4(posW, 1), viewProj);
	_out.color = _in.color;
	_out.uv = _in.texcoord;
	return _out;
}

#endif//PIXEL_SHADER
