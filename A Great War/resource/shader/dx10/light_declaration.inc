cbuffer lights_on : register(pscMaterialFlagsGroup)
{
	bool bump0;
	bool spec0;
	bool prlx0;
	bool bump1;
	bool spec1;
	bool prlx1;
	bool light0;
	bool light1;
	bool light2;
	bool light3;
	bool light4;
	bool light5;
	bool light6;
	bool light7;
};
struct eLightDesc
{
	float4 diffuse;		// RGB, cos F/2
	float4 position;	// XYZ, 1/(cos T/2 - cos F/2)
	float4 range_att;	// range, att0, att1, att2
	float4 dir;			// dir, falloff
};
struct eMaterialOutput
{
	float reflectionAmount;
	float3 normal;
	float4 diffuse;
	float3 emissive;
	float3 specular;
	float  specular_power;
};

struct eLightOutput
{
	float3 diffuse;
	float3 specular;
	float3 ambient;
};
