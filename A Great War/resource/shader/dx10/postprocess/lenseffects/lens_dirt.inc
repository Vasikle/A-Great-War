//*FH: Additional functions for the lens_dirt effect

Texture2D lensDirt_Buffer	: register(t2);

void  addLensDirt(inout float4 color, float2 texCoords)
{
	float2 lightPos = float2(lightPosX, lightPosY);
	float lensReflectionFactor = saturate(3.33333 * viewFactor - 2.33333);
	float lsunMoonRadius = 0.28f * lensReflectionFactor * sunMoonRadius;  // 0.28f = 8 * 0.035 -> 8 is an abitary factor to "enlarge" the sun radius, choosen by testing
	float distanceSample = sqrt(pow((texCoords.x - lightPos.x), 2) + pow((texCoords.y - lightPos.y), 2));
	float lensDirtFactor = saturate(pow(max(lsunMoonRadius - distanceSample, 0) / lsunMoonRadius,2) * lensReflectionFactor);
	//Incooperate lens dirt

	float luminanceFactor = saturate(getLuminanceFactor(color.rgb) * lensDirtFactor);
	color.rgb += lensDirt_Buffer.Sample(bilinear_Sampler, texCoords).rgb * luminanceFactor * lensReflectionFactor;
}
