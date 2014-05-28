#include "resources.hlsl"

//texture maps
Texture2D map_ka			: register(t1);				//ambient map
Texture2D map_kd			: register(t2);				//diffuse map
Texture2D map_ks			: register(t3);				//specular map
Texture2D map_ke			: register(t4);				//emissive map
Texture2D map_ns			: register(t5);				//specular intenwsity map
Texture2D map_tr			: register(t6);				//transparency map
Texture2D map_decal			: register(t7);				//stencil decal map
Texture2D map_normal		: register(t8);				//normal map / bump / disp

struct PS_Input
{
	float4 pos	: SV_POSITION;
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD1;
};


PS_MRTOutput main(PS_Input input)
{
	PS_MRTOutput output = (PS_MRTOutput)0;					//create an empty output
	output.color = map_kd.Sample(samLinear, input.tex);	//get color from diffuse map
	//output.color	= float4(0,0,1,1);	//get color from diffuse map

	//fetch the normalmap.
	float3 normal = map_normal.Sample(samLinear, input.tex).xyz;

	//right now b channel is up when g should be. this will prob
	//change when we use the generated one
	float temp = normal.b;
	normal.b = normal.g;
	normal.g = temp;

	// add the normals from the map to the original normal
	normal = normalize(normal + input.norm);

	output.normal = float4((normal + 1) * 0.5, 0);		//convert normal from (-1 - 1) to (0-1) to fit a texture

	float ka = 0.08;
	output.specular = float4(map_ks.Sample(samLinear, input.tex).xyz,ka);  //get specular from specular map only id there is a specular map

	float e = min(energy, 1000);
	if (e < 100)
	{
		output.glow = output.color * map_ke.Sample(samLinear, input.tex) * (1 * (e / 100)) * float4(1, 0, 0, 1) * efpadding.x;
	}
	else
	{
		output.glow = output.color * map_ke.Sample(samLinear, input.tex) * (5 * (e / 1000))  * efpadding.x;
	}

	

	return output;
}