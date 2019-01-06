#include "const.inc"
#include "shader_constants.h"

struct vsOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

#ifdef PIXEL_SHADER

bool inNearField(float radiusPixels) 
{
	return radiusPixels > 0.25;
}

cbuffer dofb : register(pscSceneGroup)
{
	int         maxCoCRadiusPixels : packoffset(pscDOFParams.x);
}




/** Source image in RGB, normalized CoC in A. */
/** For the second (Vertical) pass, the output of the previous near-field blur pass. */
Texture2D	  RGBCoC_Buffer : register(t0);
SamplerState  RGBCoC_Sampler : register(s0);

Texture2D	  DepthVS_Buffer : register(t1);
SamplerState  DepthVS_Sampler : register(s1);

/**
* During the mapping from [-1,1] to [0,1] and back again, precission errors arise, which impact render logic.
* This function checks if a value should be zero after mapped from an 0.5 value.
*/
bool IsZero(float x)
{
	return float(x < 0.01) * float(x > -0.01);
}


#define KERNEL_TAPS 6
float4 blurPS(vsOut _in) : SV_Target
{

#ifdef HORIZONTAL
	float2 direction = int2(1, 0);
#else
	float2 direction = int2(0, 1);
#endif

	float4 blurResult;

	float kernel[KERNEL_TAPS + 1];
	// 11 x 11 separated kernel weights.  This does not dictate the 
	// blur kernel for depth of field; it is scaled to the actual
	// kernel at each pixel.
	kernel[6] = 0.00000000000000;  // Weight applied to outside-radius values

	// Custom // Gaussian
	kernel[5] = /*0.50;//*/0.04153263993208;
	kernel[4] = /*0.60;//*/0.06352050813141;
	kernel[3] = /*0.75;//*/0.08822292796029;
	kernel[2] = /*0.90;//*/0.11143948794984;
	kernel[1] = /*1.00;//*/0.12815541114232;
	kernel[0] = /*1.00;//*/0.13425804976814;

	blurResult.rgb = float3(0, 0, 0);
	float blurWeightSum = 0.0f;

	
	//Grabbing the texture dimensions to use it for calculations from normaliced values to actual pixels
	float width;
	float height;

	RGBCoC_Buffer.GetDimensions(width, height);

	// Location of the central filter tap (i.e., "this" pixel's location).
	float2 A = float2(_in.texCoord.x * width, _in.texCoord.y * height);

	float4 packedColor = RGBCoC_Buffer.Sample(RGBCoC_Sampler, _in.texCoord.xy);
	packedColor.a = packedColor.a * 2 - 1;

	//Eliminating transformation errors
	packedColor.a = float(!IsZero(packedColor.a)) * packedColor.a;

	float r_A = packedColor.a *float(maxCoCRadiusPixels);

	float2 maxCoord = float2(width - 1, height - 1);
	int pixelRadi = maxCoCRadiusPixels / 2;

	for (int delta = -pixelRadi; delta <= pixelRadi; ++delta)
	{
		// Tap location near A
		float2   B = A + (direction * delta);

		float2 bClamped = clamp(B, float2(0, 0), maxCoord);
		float2 normCoords = float2(bClamped.x / width, bClamped.y / height);
		// Packed values
		float4 blurInput = RGBCoC_Buffer.Sample(RGBCoC_Sampler, normCoords);
		blurInput.a = blurInput.a * 2 - 1;

		//Eliminating transformation errors
		blurInput.a = float(!IsZero(blurInput.a)) * blurInput.a;

		// Signed kernel radius at this tap, in pixels
		float r_B = blurInput.a * float(maxCoCRadiusPixels);
		

		//float(any(packedColor.a)) * 

		if (packedColor.a > 0) //Farfield blur
		{
			float weight = kernel[clamp(int(float(abs(delta) / maxCoCRadiusPixels * (KERNEL_TAPS - 1))), 0, KERNEL_TAPS)] 
				//Consider only pixels which are in a blur field
				//This avoids "glowy" in focus objects, whene they are rendered besides blury objects. 
				// Map the blur sample to the relative weight insid the blur kernel.
				* float(!IsZero(blurInput.a));
				
			blurWeightSum += weight;
			blurResult.rgb += blurInput.rgb * weight;
		}
		else //Here, we are either in the focus or near field
		{
			
			if (packedColor.a == 0 && blurInput.a < packedColor.a) //if we are in focus field and the blur radius of the sampled pixel is smaler (i.e. "bigger") than the mapped pixel...
			{ 
				//Don't blur, just write the radius into the alpha channel of the base sample, so it gets recognized, when blending the blurred and sharp image.
				//This ensures, that the near blur bleeds into the focus field and don't creates sharp edges. 
				packedColor.a = blurInput.a;
			}
			else // here, we are in the near blur field
			{
				float weight = kernel[clamp(int(float(abs(delta) / maxCoCRadiusPixels * (KERNEL_TAPS - 1))), 0, KERNEL_TAPS)];

				blurWeightSum += weight;
				blurResult.rgb += blurInput.rgb * weight;
			}
		}
	}

	blurResult.a = packedColor.a * 2 + 0.5;

	if (blurWeightSum == 0)
		blurResult.rgb = packedColor.rgb;
	else
		blurResult.rgb /= blurWeightSum;

	return blurResult;
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
