//*FH: Additional functions for the night vision effect

Texture2D flicker_Buffer	: register(t2);		// Though this texture uses the same texture slot as the lens dirt texture, there shouldn't be conclicts, considering only one of these effects can be active as part of the lens effects	
Texture2D nvMonitor_Buffer	: register(t3);

void  addDroneVision(inout float4 color, float2 texCoords)
{
	float2 halfPixelLowRes = float2(halfPixelX_LR, halfPixelY_LR);
	
	//Grabbing the pixel from the blured and luminated texture/image, generated from the blur shader
	//float luma = illuminatedScene_Buffer.Sample(bilinear_Sampler, texCoords - halfPixelLowRes).a;
	//Enhance the "glow"
	//color.rgb += luma * 2;
	float greyScaleFactor = 1 - saturate(getLuminanceFactor(color.rgb) );
	//Enhance the contrast
	greyScaleFactor *= greyScaleFactor;
	//Tint it to the desired night vision color
	color.rgb = greyScaleFactor;

	//Simulate a monitor (scanlines, flickering and lens pollution)
	float4 nv_monitor = nvMonitor_Buffer.Sample(bilinear_Sampler, texCoords);
	float2 flickerTexCoords = float2(texCoords.x, (texCoords.y - flickerShift) * 1.5f + 0.5f);
	float3 flicker = flicker_Buffer.Sample(bilinear_Sampler, flickerTexCoords).rgb;

	color.rgb = color.rgb * (1 - nv_monitor.a) + nv_monitor.rgb * (nv_monitor.a) + flicker * 0.025f * (1 - nv_monitor.a);
}
