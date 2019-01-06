//*FH: Additional functions for the light scattering effect

float3 getModifiedColor(float2 textCoords, float sunFactor)
{
	float2 halfPixelLowRes = float2(halfPixelX_LR, halfPixelY_LR);
	
	//Grabbing the pixel from the blured and luminated texture/image, generated from the blur shader
	float4 baseColour = illuminatedScene_Buffer.Sample(bilinear_Sampler, textCoords - halfPixelLowRes);

	float3 sunColor = lightTone;
	float lumaReal = dot(lumaFactorsReal,baseColour.rgb);

	baseColour.r = lumaReal * sunColor.r;
	baseColour.b = lumaReal * sunColor.b;
	baseColour.g = lumaReal * sunColor.g;
	
	baseColour.rgb = saturate(baseColour.rgb * sunFactor);
	return baseColour.rgb; 
}

void addLightScattering(inout float4 color, float2 texCoords, float globalSunRadius)
{
	float2 lightPos = float2(lightPosX, lightPosY);
	// Calculate vector from pixel to light source in screen space.  

	float2 deltaTexCoord = texCoords - lightPos;
	
	// Divide by number of samples.  
	deltaTexCoord *= 0.2f; //1.0f / numSamples -> 1/5 -> 0.2;

	float2 sampleTexCoord = texCoords;
	// Set up illumination ray_length factor.  
	float illuminationDecay = 1.0f;

	//constant sun size multiplied with a user defined factor
	float effectRadius = globalSunRadius * 32 * saturate(3.33333 * viewFactor - 2.33333);

	float globalRestDistance = sqrt(pow((sampleTexCoord.x - lightPos.x) * aspectRatio, 2) + pow((sampleTexCoord.y - lightPos.y), 2));
	float globalSunFactor = saturate(max(effectRadius - globalRestDistance, 0) / effectRadius);
	//globalSunFactor = 0.0192255 * exp(3.95177 * globalSunFactor);

	globalSunFactor = 0.0296708 * exp(3.51797 * globalSunFactor);

	[unroll]
	for (int i = 0; i < 5; i++)
	{
		// Step sample location along ray.  
		sampleTexCoord -= deltaTexCoord;

		//*FH: Calculate th rest distance to the light source 
		float restDistance = sqrt(pow((sampleTexCoord.x - lightPos.x) * aspectRatio, 2) + pow((sampleTexCoord.y - lightPos.y), 2));

		float sunFactor = saturate(max(globalSunRadius - restDistance, 0) / globalSunRadius);
		float3 ssample = getModifiedColor(sampleTexCoord, sunFactor);
		// Apply sample attenuation scale/ray_length factors.  
		ssample *= illuminationDecay * intensity;
		// Accumulate combined color.  
		color.rgb += ssample * globalSunFactor;
		// Update exponential ray_length factor.  
		illuminationDecay *= ray_length;
	}
}