#ifdef HAZE
float CalcHaze(float3 posW)
{
	float3 viewDir = cameraOrigin - posW;
 	float viewDist = length(viewDir);
	float haze = (hazeEnd - viewDist) * hazeScale;
 #ifdef BOUNDARY_HAZE
	haze = saturate(haze);
	float k1, k2;
	k1 = max(min(posW.x - hazeBound.x, hazeBound.z - posW.x), 0);
	k2 = max(min(posW.y - hazeBound.y, hazeBound.w - posW.y), 0);
	if(k1 < hazeBoundWidth) haze *= k1 / hazeBoundWidth;
	if(k2 < hazeBoundWidth) haze *= k2 / hazeBoundWidth;
 #endif
	return haze;
}
#endif
