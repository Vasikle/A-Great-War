Texture2D baseTexture : register(t0);
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

//Default
static const float BloomThreshold = 0.9f;
static const float BloomThreshold_NV = 0.5f;

float4 LumaPS(vsOut _in) : SV_TARGET
{
	
	float4 color = baseTexture.Sample(baseSampler, _in.texCoord);
	

	float4 luma = saturate((color - BloomThreshold_NV) / (1 - BloomThreshold_NV));
    // Adjust it to keep only values brighter than the specified threshold.
	color = saturate((color - BloomThreshold) / (1 - BloomThreshold));
	color.a = (luma.r + luma.g + luma.b) * 0.333333333;
	return color;
}
vsOut LumaVS(vsIn _in)
{
	vsOut _out;
	_out.position = _in.position;
	_out.texCoord = _in.texCoord1;
	return _out;
}
