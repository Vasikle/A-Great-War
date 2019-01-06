//*FH: Functions dedicated to generate a more realistic looking water surface

#ifdef WATER_OPTICS

Texture2D scene_BUFFER : register(t8);
SamplerState scene_SAMPLER : register(s8)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct WaterInfo
{
	float waterDepth;
	float viewDistanceToGround;
	float costalAlpha;
	float3 bareSceneColor;
};

float3 CalculateWaterColor(float2 sampleCoords, float3 color, eMaterialOutput mtl, inout WaterInfo wInfo)
{
	static const float3 colorExtinction = float3(45.f, 750.f, 3000.f);			//This vector describes at which water depth color components fade
	static const float3 negatedColorExtinction = float3(3000.f,3000.f,3000.f) - colorExtinction;

	static const float fogS = 0;											//Fog decreasing starts at the point of view
	
	float fogE = waterParams.w;
	float pollutionFactor = waterParams.x;
	float maxDepth = waterParams.y;
	float3 pollutionColor = waterDiffuse.rgb;
	const static float3 darkWaterColor = float3(0.1f,0.1f,0.1f);
	float3 deepWaterColor = lerp(darkWaterColor, 0.25f * pollutionColor, pollutionFactor);
	float3 normal = mtl.normal.rgb;
	
	//Considiring the water as fog, so its transparency changes with view distance
	float fog = saturate((fogE - wInfo.viewDistanceToGround) / (fogE + 0.0001f)); //avoiding null division by an epsilon value

	//Get refraction
	//boost refraction with water depth, linear formula which is at 0->0.2 and 100->2
	float refractionBoost = clamp(0.2 + 0.018 * wInfo.waterDepth, 0.2,2);
	float2 refractedTexCords = sampleCoords + (normal.xy * float2(0.05, 0.05) * refractionBoost);
	float3 refractedColor = wInfo.bareSceneColor = scene_BUFFER.Sample(scene_SAMPLER, refractedTexCords).rgb;

	float waterDullnes = saturate(wInfo.waterDepth / maxDepth);									//Variable to control water transperancy via depth 
	float3 transparencyFactor = 
	saturate(wInfo.waterDepth / (colorExtinction + negatedColorExtinction * pollutionFactor));	//Variable to control water transperancy against water color, taking color extinction into account
																								//negatedColorExtinction time polution factor negates the "oceanic" fading of colors and so gives polluted 
																								//water a more realistic color fading by depths
	
	float3 waterPixelColor = lerp(refractedColor, deepWaterColor, transparencyFactor);			//Lerp the refracted ground beneath the water and the deep water color to have a physicaly correct color gradient  
	waterPixelColor = lerp(waterPixelColor, deepWaterColor, waterDullnes);						//Lerp the water color with a opaque color, controlled by a max water depth value (at which it's completely opauqe)
	waterPixelColor = lerp(deepWaterColor, waterPixelColor, fog);								//Lerp the water color with a opaque color, controlled by a view distance depened fog factor
	waterPixelColor = lerp(waterPixelColor, pollutionColor * waterPixelColor, pollutionFactor);	//Lerp the water color with a pollution color, controlled by factor describing the amount of polution 

	return waterPixelColor; 
}

WaterInfo CalculateWaterInfo(float2 sampleCoords, float3 waterWorldPos)
{
	float coastDepth = waterParams.z;
	//Grab the view space depth
	float vsDepth = vsDepth_BUFFER.Sample(vsDepth_SAMPLER, sampleCoords).r;
	
	//Project it to clip space and simulate ndc by perspective divide
	float4 projectedDepth = mul(float4(1, 1, vsDepth, 1), projMatrix);
	float4 ndcPos = float4(sampleCoords.x * 2 - 1, (1 - sampleCoords.y) * 2 - 1, projectedDepth.z / projectedDepth.w, 1);
	float4 normalizedWPos = mul(ndcPos, invProjViewMatrix);
	float4 wPos = normalizedWPos * vsDepth;
	
	WaterInfo wInfo = (WaterInfo)0;

	wInfo.waterDepth = abs(waterWorldPos.z - wPos.z);

	//Storing the calculated vs depth here and return it. After the return, the real 
	// accumulated water will get calculated with the .w component of the input position.
	wInfo.viewDistanceToGround = vsDepth;
	wInfo.costalAlpha = saturate(wInfo.waterDepth / coastDepth);
	
	wInfo.bareSceneColor = (float3)0;

	return wInfo;
}

float3 CalculateWaterOptics(float3 clr, eMaterialOutput mtl, vsOut _in, out WaterInfo wInfo)
{

	float width;
	float height;

	scene_BUFFER.GetDimensions(width, height);

	float2 sampleCoords;
	sampleCoords.x = _in.position.x / width;
	sampleCoords.y = _in.position.y / height;

	wInfo = CalculateWaterInfo(sampleCoords, _in.pos_world);

	wInfo.viewDistanceToGround = wInfo.viewDistanceToGround - 100; //Guessing the distance to the near plane suffices
	return CalculateWaterColor(sampleCoords, clr, mtl, wInfo);
}
	#ifdef HAZE
	void ApplyHazeWaterOptics(inout float3 color, float _haze, float _costalAlpha, float3 bareSceneColor)
	{
		float hazeness = 1 - saturate(_haze);
		float alpha = saturate(_costalAlpha);
		color = lerp(color, bareSceneColor, hazeness);
		#ifdef ADDITIVE_BLEND
			color.rgb *= saturate(_haze);
		#else
			color = lerp(color, haze_color.rgb, alpha * hazeness);
		#endif
	}
	#endif
#endif