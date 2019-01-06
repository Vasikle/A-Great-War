//common shader functions
float Luminance(float3 color)
{
	return dot(color, float3(0.2125, 0.7154, 0.0721));
}

float PackSceneDepth( float4 pos ) {

  return( pow( pos.z / pos.w, 4.0 ) );
	
}

