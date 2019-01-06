#ifdef PARALLAX

Texture2D texHeight0 : register(t3);
SamplerState samHeight0 : register(s3);

#ifdef LAYER1
Texture2D texHeight1 : register(t7);
SamplerState samHeight1 : register(s7);
#endif

void ApplyParallax(inout float2 tc, float3 vvec, float k_steps, Texture2D tex_height, SamplerState sam_height, float4 parallax_params)
{
	float scale		= parallax_params[0];
	float min_steps = parallax_params[1];
	float max_steps = parallax_params[2];
	float2 dx = ddx(tc); 
	float2 dy = ddy(tc);
	float numSteps = lerp(max_steps, min_steps, k_steps);
    float step   = 1.0f / numSteps;
   	float2 delta = float2(vvec.x, -vvec.y) * scale * step / vvec.z;
	
	if(0)
	{
		//simple утоплен
		float h = (1 - tex_height.Sample(sam_height, tc).r) + 0.00;
		tc -= delta * numSteps * h;
	}
	else if(0)
	{
		float height = 1.0;                          
		float h      = 1 - tex_height.Sample(sam_height, tc).r;  

		while(h < height)                            
		{
			height -= step;
	        tc	   += delta;
		    h       = 1 - tex_height.SampleGrad(sam_height, tc, dx, dy).r;
		}
		float2  prev = tc - delta;
		float hPrev  = 1 - tex_height.Sample(sam_height, prev).r - (height + step);
		float hCur   = h - height;
		float weight = hCur / (hCur - hPrev );
		tc = lerp(tc, prev, weight);
	}
	else if(1)
	{
		float h0 = 1 - tex_height.Sample(sam_height, tc).r;
				
		float height = 1 - step;
		float2 offset = tc + delta;
		float h1 = 1 - tex_height.Sample(sam_height, offset.xy).r;
	
		for(int i = 0; i < numSteps; i++)
		{
			[flatten]
			if (h1 >= height)
				break;
				
			h0 = h1;
	
			height -= step;
			offset += delta;
	
			h1 = 1 - tex_height.SampleGrad(sam_height, offset, dx, dy).r;
		}
		
		float error = 1.0f;
	
		float t1 = height;
		float t0 = t1 + step;
		
		float delta1 = t1 - h1;
		float delta0 = t0 - h0;
	
		float4 intersect = float4(delta * numSteps, delta * numSteps + tc);
	
		for(i = 0; i < 10; i++)
		{
			[flatten]
			if(abs(error) <= 0.01)
				break;
		
			float denom = delta1 - delta0;
			float t = (t0 * delta1 - t1 * delta0) / denom;
			offset = -t * intersect.xy + intersect.zw;
			
			float h = 1 - tex_height.SampleGrad(sam_height, offset, dx, dy).r;
	
			error = t - h;
			if (error < 0)
			{
				delta1 = error;
				t1 = t;
			}
			else
			{
				delta0 = error;
				t0 = t;
			}
		}
		tc = offset;
	}
}
#endif
