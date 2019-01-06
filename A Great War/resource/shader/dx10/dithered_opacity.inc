static const int dither_threshold[64] = {
	2 , 60, 16, 56, 3 , 57, 13, 53,
	34, 18, 48, 32, 35, 19, 45, 29,
	10, 50, 6 , 64, 11, 51, 7 , 61,
	42, 26, 38, 22, 43, 27, 39, 23,
	4 , 58, 14, 54, 1 , 59, 15, 55,
	36, 20, 46, 30, 33, 17, 47, 31,
	12, 52, 8 , 62, 9 , 49, 5 , 63,
	44, 28, 40, 24, 41, 25, 37, 21
};

void ClipDithered(float2 screen_pos, float a)
{
	int a_norm = a * 64;
	int th = dither_threshold[int(screen_pos.x) % 8 + (int(screen_pos.y) % 8) * 8];
	clip(a_norm - th);
}