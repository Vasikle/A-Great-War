#include "shader_constants.h"
#include "vs_out.inc"

float sinWave(float3 projectedWorldPosition);
float3 gerstnerWave(float3 projectedWorldPosition);

float CalculateDimmFactor(float distanceBetweenVerticesHighCap, float distanceBetweenVerticesLowCap, float waveLength);
float CalculateAverageDistanceBetweenVertices(float distanceToSurface);

struct vsIn
{
	float3 position : POSITION;
	float3 texCoord : TEXCOORD;
};

#define MAX_WAVES 16								//Max supported Waves
static const float g = 9.8f;						//gravitational constant
static const float TwoPI = 6.28318530718;
static const float FADE_FACTOR = 15;

cbuffer OCEAN_VS_Parameters : register(vscSceneGroup)
{
	float3 planeNormal			: packoffset(vscOceanPlaneNormal);
	float3 planeWSPos			: packoffset(vscOceanPlaneWSPos);
	float3 cameraOrigin			: packoffset(vscCameraOrigin);

	float4 waveData[MAX_WAVES]	: packoffset(vscOceanWaveData);			//x,y -> wave direction, z -> amplitude, w -> wave length
	float4 globalWaveData		: packoffset(vscOceanWaveParams);			//x -> Active Waves, y -> time, z -> scaleFactor, w -> wind speed

	float4x4 viewProjection		: packoffset(vscViewProj);
	float4x4 invViewProjection	: packoffset(vscInverseViewProj);
	float4x4 uvRotationMatrix   : packoffset(vscUVRotationMatrix);

	#include "haze\vs_constants.inc"
	#include "fogofwar\vs_constants.inc"
}

#include "haze\vs.inc"
#include "fogofwar\vs.inc"
/* 
* With the grid projected into the world, we have also to project the texture cordinates, or we would
* have the same coordinates no matter the camera orientation, which would destroy the illusion.
*/
float2 TransformUVCoordinates(float3 projectedWorldPos)
{
	float scale = globalWaveData.z;
	projectedWorldPos = mul(uvRotationMatrix, float4(projectedWorldPos,0)).xyz;
	float3 newUV = projectedWorldPos / (scale * 100);
	return newUV.xy;
}

vsOut VS(vsIn _in)
{
	// echte x -> y
	// echte y -> z
	// echte z -> x
	//
	//		z
	//		|	x
	//		|  /
	//		| /
	//		|/_____ y
	//
	vsOut _out = (vsOut)0;

	float time = globalWaveData.y;

	float3 normal = planeNormal;
	float4 wsPos = mul(float4(_in.position,1), invViewProjection);						// Reverting the vertex position from NDC to Worldspace.
	wsPos = wsPos / wsPos.w;															// Undoing perspective projection.

	float3 dir = normalize(wsPos.xyz - (cameraOrigin));// +normalize(CameraLookAt) * (scaleFactor - 1.f)));// Get the direction from the cameras postion to the vertex (world space).
	float denominator = dot(dir, normal);												// Calculate the denominator of the line-plane intersection formula...

	denominator = denominator >= 0.f ? -0.00001 : denominator;							// ... and avoid a division by zero and backfiring by clamping the range to negative values
																						// However, this also prevents the water surface drawn when it's above the camera.
	float dirFactor = dot((planeWSPos - cameraOrigin), normal) / denominator;			// Calculate the factor, by which multilpied, the direction vector intersects the plane.

	float3 projectedWorldPosition = dirFactor * dir + cameraOrigin;						// Add the scaled direction vector to the camera position, to get the projectd position.

	//float scalingFactor = CalculateDimmFactor(distance(cameraOrigin, projectedWorldPosition));
	float distanceBetweenVerticesLowCap = CalculateAverageDistanceBetweenVertices(dirFactor);
	float distanceBetweenVerticesHighCap = distanceBetweenVerticesLowCap * FADE_FACTOR;
	
	_out.texCoord =  TransformUVCoordinates(projectedWorldPosition);

	float3 projectedWorldPositionXOff = projectedWorldPosition + float3(2, 0, 0);
	float3 projectedWorldPositionYOff = projectedWorldPosition + float3(0, 2, 0);
	
	float2 baseVertPosXZ;																// Vertex Position on XY Plane
	float2 baseVertexPosXZ_xOff;
	float2 baseVertexPosXZ_yOff;

	baseVertPosXZ.x = projectedWorldPosition.y;
	baseVertPosXZ.y = projectedWorldPosition.x;
	baseVertexPosXZ_xOff.x = projectedWorldPositionXOff.y;
	baseVertexPosXZ_xOff.y = projectedWorldPositionXOff.x;
	baseVertexPosXZ_yOff.x = projectedWorldPositionYOff.y;
	baseVertexPosXZ_yOff.y = projectedWorldPositionYOff.x;

	float2 vertPosXZ = baseVertPosXZ;
	float2 vertPosXZ_xOff = baseVertexPosXZ_xOff;
	float2 vertPosXZ_yOff = baseVertexPosXZ_yOff;

	float waveHeight = planeWSPos.z;
	float waveHeightXOff = planeWSPos.z;
	float waveHeightYOff = planeWSPos.z;

	//float scalingFactor = 0.00176471 * dirFactor + 0.470588; //linear
	//float scalingFactor = clamp(0.707107f * exp(0.00115525 * dirFactor) ,1.f, 4.f); //exponetial
	//float scalingFactor = 1;
	float q = 1;
	float S = 1;
	float windSpeed = globalWaveData.w * 20; //Windspeed per second transformed into GEM meters
	for (int i = 0; i < globalWaveData.x; i++)
	{
		float2 dir;
		dir.x = waveData[i].x;
		dir.y = waveData[i].y;
		dir = normalize(dir);
		float amplitude = waveData[i].z;// *scalingFactor;
		float waveLength = waveData[i].w;
		float frequency = 6.2831853; //1 Hz
		float phase = S * 6.2831853 / waveLength; //1 Hz

		float scalingFactor = CalculateDimmFactor(distanceBetweenVerticesHighCap, distanceBetweenVerticesLowCap, waveLength);
		
		amplitude *= scalingFactor;

		float lq = 0.280056 * amplitude + 0.997199;

		float freqCoff =  dot(dir, baseVertPosXZ) * phase - time * windSpeed / waveLength;
		float freqCoffXOff = dot(dir, baseVertexPosXZ_xOff) * phase - time * windSpeed / waveLength;
		float freqCoffYOff = dot(dir, baseVertexPosXZ_yOff) * phase - time * windSpeed / waveLength;
		
		float cosiCoff = cos(freqCoff);
		float cosiCoffXOff = cos(freqCoffXOff);
		float cosiCoffYOff = cos(freqCoffYOff);

		float qi = lq / (amplitude * globalWaveData.x);
 
		vertPosXZ.x += qi * amplitude * dir.x * cosiCoff;
		vertPosXZ.y += qi * amplitude * dir.y * cosiCoff;
		vertPosXZ_xOff.x += qi * amplitude * dir.x * cosiCoffXOff;
		vertPosXZ_xOff.y += qi * amplitude * dir.y * cosiCoffXOff;
		vertPosXZ_yOff.x += qi * amplitude * dir.x * cosiCoffYOff;
		vertPosXZ_yOff.y += qi * amplitude * dir.y * cosiCoffYOff;

		waveHeight += amplitude * sin(freqCoff);
		waveHeightXOff += amplitude * sin(freqCoffXOff);
		waveHeightYOff += amplitude * sin(freqCoffYOff);
		
		
	}
	
	projectedWorldPosition.y = vertPosXZ.x;
	projectedWorldPosition.x = vertPosXZ.y;
	projectedWorldPosition.z = waveHeight;// *lodFactor;

	projectedWorldPositionXOff.y = vertPosXZ_xOff.x;
	projectedWorldPositionXOff.x = vertPosXZ_xOff.y;
	projectedWorldPositionXOff.z = waveHeightXOff;

	projectedWorldPositionYOff.y = vertPosXZ_yOff.x;
	projectedWorldPositionYOff.x = vertPosXZ_yOff.y;
	projectedWorldPositionYOff.z = waveHeightYOff;
	
	float3 diffX = projectedWorldPositionXOff - projectedWorldPosition;
	float3 diffY = projectedWorldPositionYOff - projectedWorldPosition;
	normal = normalize(cross(diffX, diffY));
	static float3 tangent = float3(1,0,0);
	float3 binormal = normalize(cross(tangent,normal));
	tangent = normalize(cross(normal, binormal));
	binormal *= -1;

	//projectedWorldPosition = gerstnerWave(projectedWorldPosition);
	//projectedWorldPosition.z = sinWave(projectedWorldPosition);
	_out.position = mul(float4(projectedWorldPosition, 1.f), viewProjection);			// Multiply the projected position by the the View/Projection matrix to bring em back to ndc space.
	_out.normal = normal* 0.5f + 0.5f ;
	//_out.normal = float3(0, 0, 0.5);
#ifdef BUMP
	_out.tangent = normalize(tangent).xyz;
	_out.binormal = normalize(binormal);
#endif
	_out.pos_world = projectedWorldPosition;
#ifdef REAL_REFLECTION
	_out.position_proj = _out.position;
#endif
#ifdef HAZE
	_out.haze = CalcHaze(_out.pos_world);
#endif
#ifdef FOGMAP
	_out.fogmapCoord = CalcFogmapCoord(projectedWorldPosition);
#endif
	return _out;
}

float sinWave(float3 projectedWorldPosition)
{
	//need per wave: amplitude, waveSpeedPerSecond, dir.xy, waveLength

	//float4 waves[MAX_WAVES]		: packoffset(vscOceanWaves);			//x,y -> wave direction, z -> amplitude, w -> wave length
	//float4 globalWaveData		: packoffset(vscOceanWaveParams);		//x -> Active Waves, y -> time, z -> scaleFactor
		
	float pi = 3.14159265359f;

	float time = globalWaveData.y;
	float z = planeWSPos.z;

	for (int i = 0; i < globalWaveData.x; i++)
	{
		float waveLength = waveData[i].w;
		float waveSpeedPerSecond = 20; //need to be wave depentend
		float amplitude = waveData[i].z;
		float2 dir = waveData[i].xy;

		float frequency = 2 * pi / waveLength;
		float phaseConstant = waveSpeedPerSecond * frequency;
		z += amplitude * sin(dot(dir, float2(projectedWorldPosition.x, projectedWorldPosition.y)) * frequency + time * phaseConstant);
	}
	return z;
}

float3 gerstnerWave(float3 projectedWorldPosition)
{
	float2 animatedSurfacePoint = projectedWorldPosition.xy;
	float animatedZ = projectedWorldPosition.z;
	float pi = 3.14159265359f;

	float time = globalWaveData.y;
	float z = planeWSPos.z;

	for (int i = 0; i < globalWaveData.x; i++)
	{
		float waveLength = waveData[i].w;
		float waveSpeedPerSecond = 20; //need to be wave depentend
		float amplitude = waveData[i].z;
		float2 dir = normalize(waveData[i].xy);
		float frequency = 2 * pi / waveLength;
		float phaseConstant = waveSpeedPerSecond * frequency;
		float scalarTest = dot(dir, projectedWorldPosition.xy);
		animatedSurfacePoint += amplitude * dir * sin(frequency * time - scalarTest);
		animatedZ += amplitude * cos(frequency * time - scalarTest);
	}
	return float3(animatedSurfacePoint.x, animatedSurfacePoint.y, animatedZ);
}

float CalculateDimmFactor(float distanceBetweenVerticesHighCap, float distanceBetweenVerticesLowCap, float waveLength)
{

	float scalingFactor = 1.f;
	if (distanceBetweenVerticesHighCap < waveLength)
	{
		return 1.f;
	}
	else //(distanceBetweenVerticesHighCap >= waveLength)
	{
		if (distanceBetweenVerticesLowCap < waveLength)
		{

			float diff = (waveLength - distanceBetweenVerticesLowCap) / (distanceBetweenVerticesHighCap - distanceBetweenVerticesLowCap);
			return clamp(diff * diff, 0.01, 1.f);
		}
		else //(distanceBetweenVerticesLowCap >= waveLength)
			return 0.01f;
	}
	//scalingFactor = waveLength > distanceBetweenVerticesHighCap ? 1 : 0.01;
}

float CalculateAverageDistanceBetweenVertices(float distanceToSurface)
{
	// After evaluating diffrent tests meassuring the relation between distance to vertex/triangle and the distance between the verteices 
	// of the related triangle, following relation could be meassured:
	//http://www.wolframalpha.com/input/?i=fit+%7B(133,+0.9217),+(153,+1.45),+(433,+9.685),+(717,+14.821),(2533,+209.785),+(4344,+631.729)%7D

	//With this formula, we're now able to check, if the grids resolution at a certain point in world space is dense enought to draw
	//a specific wave condition.

	return (0.0000346907 * (distanceToSurface * distanceToSurface) - 0.00583045 * distanceToSurface + 2.3153);// (quadratic)
}