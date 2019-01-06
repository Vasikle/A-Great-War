cbuffer data1 : register(b0)
{
	float2 src_size : packoffset(c1.x);
	float2 one_pixel : packoffset(c1.z);
	float4 color_modulation : packoffset(c2);
}
#ifndef MULTISAMPLED
  Texture2D baseTexture : register(t0);
#else
  #if MULTISAMPLED == 1
    Texture2DMS<float4, 2> baseTexture : register(t0);
  #elif MULTISAMPLED == 2
    Texture2DMS<float4, 4> baseTexture : register(t0);
  #elif MULTISAMPLED == 3
    Texture2DMS<float4, 8> baseTexture : register(t0);
  #elif MULTISAMPLED == 4
    Texture2DMS<float4, 16> baseTexture : register(t0);
  #endif
#endif
SamplerState baseSampler : register(s0);

struct vsIn
{
	float4 position : POSITION;
	float2 texCoord0 : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
};
struct vsOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

float4 SampleBase(float2 uv)
{
#ifdef UV_CORRECTION
	uv -= one_pixel * 0.5;
#endif
#ifndef MULTISAMPLED
	return baseTexture.Sample(baseSampler, uv);
#else
	return baseTexture.Load(int2(uv.x * src_size.x, uv.y * src_size.y), 0) ;
#endif
}

float4 CopyPS(vsOut _in) : SV_TARGET
{
	return SampleBase(_in.texCoord);
}

float4 CopyWithModulatePS(vsOut _in) : SV_TARGET
{
	return SampleBase(_in.texCoord) * color_modulation;
}

float4 BoxRescalePS(vsOut _in) : SV_TARGET
{
	float4 p0 = SampleBase(_in.texCoord);
	float4 p1 = SampleBase(_in.texCoord + one_pixel);
	float4 p2 = SampleBase(_in.texCoord + float2(one_pixel.x, 0));
	float4 p3 = SampleBase(_in.texCoord + float2(0, one_pixel.y));
	return (p0 + p1 + p2 + p3) / 4;
}

vsOut CopyVS(vsIn _in)
{
	vsOut _out;
	_out.position = _in.position;
	_out.texCoord = _in.texCoord1;
	return _out;
}
