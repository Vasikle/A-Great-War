//*FH: Additional functions for the lens_dirt effect

void  addScreenBrightness(inout float4 color)
{
	color.rgb += lightTone * brightIntensity * saturate(5 * viewFactor - 4); 
}
