#include "resources.hlsl"

//texture maps
Texture2D map_ka			: register(t1);				//ambient map
Texture2D map_kd			: register(t2);				//diffuse map
Texture2D map_ks			: register(t3);				//specular map
Texture2D map_ke			: register(t4);				//emissive map
Texture2D map_ns			: register(t5);				//specular intenwsity map
Texture2D map_tr			: register(t6);				//transparency map
Texture2D map_decal			: register(t7);				//stencil decal map

struct PS_Input
{
	float4 pos	: SV_POSITION;
	float2 tex	: TEXCOORD1;
};

float4 main(PS_Input input) : SV_TARGET0
{
	float darknessFactor = timeofday*0.85 + 0.15;
	return map_kd.Sample(samLinear, input.tex) * float4(darknessFactor,darknessFactor,darknessFactor, 1);
}