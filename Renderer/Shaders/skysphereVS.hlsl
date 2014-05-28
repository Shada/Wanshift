#include "resources.hlsl"

struct VS_Input
{
	float3 pos	: POSITION;
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD0;
};

struct PS_Input
{
	float4 pos	: SV_POSITION;
	float3 tex	: TEXCOORD1;
};

PS_Input main(VS_Input input)
{
	PS_Input output = (PS_Input)0;

	output.tex = (input.pos);
	output.pos = float4(mul((float3x3)mView, input.pos * 5).xyz, 1);
	output.pos = mul(mProj, output.pos);
	return output;
}