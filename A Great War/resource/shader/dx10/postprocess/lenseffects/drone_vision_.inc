//*FH: Additional functions for the drone vision effect

Texture2D flicker_Buffer		: register(t2);		// Though this texture uses the same texture slot as the lens dirt texture, there shouldn't be conclicts, considering only one of these effects can be active as part of the lens effects
Texture2D dvHUD_Buffer			: register(t3);
Texture2D fakeMonitor_Buffer	: register(t4);

float fakeLowBPP(float colorValue)
{
	
	int eightBPP = colorValue * 255;
	int sixBPP = eightBPP >> 4;
	return  float(sixBPP << 4) / 255.f;
}

void  addDroneVision(inout float4 color, float2 texCoords)
{
	float2 halfPixelLowRes = float2(halfPixelX_LR, halfPixelY_LR);

	float greyScaleFactor = saturate(getLuminanceFactor(color.rgb));
	//Tint it to the desired night vision color
	color.rgb = fakeLowBPP(greyScaleFactor);

	//Simulate a monitor (scanlines, flickering and lens pollution)
	float4 fakeMonitor = fakeMonitor_Buffer.Sample(bilinear_Sampler, texCoords);
	float3 dvHUD = dvHUD_Buffer.Sample(bilinear_Sampler,texCoords);
	float2 flickerTexCoords = float2(texCoords.x, (texCoords.y - flickerShift) * 1.5f + 0.5f);
	float3 flicker = flicker_Buffer.Sample(bilinear_Sampler, flickerTexCoords).rgb;

	color.rgb = color.rgb * (1 - fakeMonitor.r) + dvHUD.rgb + flicker * 0.01f * (1 - fakeMonitor.r);
}
