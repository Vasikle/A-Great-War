//*FH: Additional functions for the night vision effect

Texture2D flicker_Buffer		: register(t2);		// Though this texture uses the same texture slot as the lens dirt texture, there shouldn't be conclicts, considering only one of these effects can be active as part of the lens effects
Texture2D nvHUD_Buffer			: register(t3);
Texture2D fakeMonitor_Buffer	: register(t4);

void  addNightVision(inout float4 color, float2 texCoords)
{
	float2 halfPixelLowRes = float2(halfPixelX_LR, halfPixelY_LR);
	
	//Grabbing the pixel from the blured and luminated texture/image, generated from the blur shader
	float luma = illuminatedScene_Buffer.Sample(bilinear_Sampler, texCoords - halfPixelLowRes).a;
	//Enhance the "glow"
	color.rgb += luma * 2;

	float2 screenMid = 1 - (abs(float2(0.5, 0.5) - texCoords) * 2);
	float screenMidness = (screenMid.x + screenMid.y) * 0.5;
	screenMidness = screenMidness * screenMidness;
	float greyScaleFactor = saturate(getLuminanceFactor(color.rgb * screenMidness) + 0.25f);
	//Enhance the contrast
	greyScaleFactor *= greyScaleFactor;
	//Tint it to the desired night vision color
	color.rgb = lerp(float3(0, 0, 0),float3(0.5,1.78125, 0.5), greyScaleFactor);

	//Simulate a monitor (scanlines, flickering and lens pollution)
	float4 fakeMonitor = fakeMonitor_Buffer.Sample(bilinear_Sampler, texCoords);
	float3 nvHUD = nvHUD_Buffer.Sample(bilinear_Sampler,texCoords);
	float2 flickerTexCoords = float2(texCoords.x, (texCoords.y - flickerShift) * 1.5f + 0.5f);
	float3 flicker = flicker_Buffer.Sample(bilinear_Sampler, flickerTexCoords).rgb;

	color.rgb = color.rgb * (1 - fakeMonitor.r) + nvHUD.rgb + flicker * 0.01f * (1 - fakeMonitor.r);
}
