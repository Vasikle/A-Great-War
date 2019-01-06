struct vsOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

#ifdef PIXEL_SHADER

#define SAMPLE_COUNT 7
static float	sampleWeight[SAMPLE_COUNT] = {0.227f, 0.275f, 0.414f, 0.525f, 0.414f, 0.275f, 0.227f};
static float	sampleOffset[SAMPLE_COUNT] = {-6, -4, -2, 0, 2, 4, 6};

cbuffer gauss1 : register(b0)
{
	float2 halfPixel : packoffset(c0); 
};
Texture2D tex : register(t0);
SamplerState sam : register(s0);

float4 Blur(float2 texCoord,  bool horiz)
{
	float4 color = 0;
	for(int i = 0; i < SAMPLE_COUNT; ++i)
	{
		float off = sampleOffset[i] + 1;
		float2 offTex = float2(horiz ? off : 1, horiz ? 1 : off) * halfPixel;
#ifdef UV_CORRECTION
		offTex -= halfPixel;
#endif
		color += tex.Sample(sam, texCoord + offTex) * sampleWeight[i] * 0.42427f;
	}
	return color;
}
float4 PS_Horiz(vsOut _in) : SV_TARGET
{
	return Blur(_in.texCoord, true);
}
float4 PS_Vert(vsOut _in) : SV_TARGET
{
	return Blur(_in.texCoord, false);
}

#else//PIXEL_SHADER

struct vsIn
{
	float4 position : POSITION;
	float2 texCoord0 : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
};
vsOut VS(vsIn _in)
{
	vsOut _out;
	_out.position = _in.position;
	_out.texCoord = _in.texCoord1;
	return _out;
}

#endif//PIXEL_SHADER
