//*FH: Additional functions for the night vision effect

Texture2D flicker_Buffer		: register(t2);		// Though this texture uses the same texture slot as the lens dirt texture, there shouldn't be conclicts, considering only one of these effects can be active as part of the lens effects	
Texture2D fakeMonitor_Buffer	: register(t4);

void  addThermalVision(inout float4 color, float2 texCoords)
{
	float2 halfPixelLowRes = float2(halfPixelX_LR, halfPixelY_LR);
	
	float greyScaleFactor = saturate(getLuminanceFactor(color.rgb));
	greyScaleFactor *= 0.5f;
	
	//Enhance the contrast
	greyScaleFactor *= greyScaleFactor;
	//Enhance the "heatness"
	greyScaleFactor += color.a * 3.f;
	color.rgb = greyScaleFactor;

	//Simulate a monitor (scanlines, flickering and lens pollution)
	float4 fakeMonitor = fakeMonitor_Buffer.Sample(bilinear_Sampler, texCoords);
	float2 flickerTexCoords = float2(texCoords.x, (texCoords.y - flickerShift) * 1.5f + 0.5f);
	float3 flicker = flicker_Buffer.Sample(bilinear_Sampler, flickerTexCoords).rgb;

	color.rgb = color.rgb * (1 - fakeMonitor.r) + flicker * 0.01f * (1 - fakeMonitor.r);
}
