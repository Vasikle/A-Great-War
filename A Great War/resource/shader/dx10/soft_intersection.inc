#if defined(SOFT_INTERSECT) | defined(WATER_IMPROVEMENTS)

Texture2D scene_BUFFER : register(t8);
SamplerState scene_SAMPLER : register(s8)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

float CalculateSoftIntersection(float DepthDiffrence)
{

	#ifdef WATER_IMPROVEMENTS
		static float maxDist = 3;
	#else
		static float maxDist = 20;
	#endif

	float diff = clamp(DepthDiffrence, 0, maxDist);
	float normdDiff = diff / maxDist;

	normdDiff = normdDiff * 2 - 1;

	int signDiff = sign(normdDiff);
	return 0.5f * (signDiff * pow(abs(normdDiff), 1.5)) + 0.5;

}

float CalculateSoftIntersection(float pixelVSDepth, float2 screenCoords)
{
	float width;
	float height;

	vsDepth_BUFFER.GetDimensions(width, height);

	float2 sampleCoords;
	sampleCoords.x = screenCoords.x / width;
	sampleCoords.y = screenCoords.y / height;

	float backgroundVSDepth = vsDepth_BUFFER.Sample(vsDepth_SAMPLER,sampleCoords).r;
	return CalculateSoftIntersection(backgroundVSDepth - pixelVSDepth );
}

#endif //SOFT_INTERSECTION | WATER