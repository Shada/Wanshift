#include "resources.hlsl"

TextureCube texSkymap	: register(t1);

struct PS_Input
{
	float4 pos : SV_POSITION;
	float3 tex : TEXCOORD1;
};

float4 main(PS_Input input) : SV_TARGET0
{
	float t = (1 - timeofday) * (1 - timeofday) * (1 - timeofday);
	return float4(texSkymap.Sample(samLinear2, input.tex).rgb, t);
}