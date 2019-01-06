#ifdef HAZE
	float	hazeScale : packoffset(vscHazeParams.x);
	float	hazeEnd : packoffset(vscHazeParams.y);
	float	hazeBoundWidth : packoffset(vscHazeParams.z);
	float4	hazeBound : packoffset(vscHazeBound);
#endif
