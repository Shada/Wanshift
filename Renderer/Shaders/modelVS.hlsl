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
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD1;
};

PS_Input main(VS_Input input)
{
	PS_Input output = (PS_Input)0;
	//convert from model space to world, view then screen space.
	output.pos = mul(mProj, mul(mView, mul(mWorld, float4(input.pos, 1.0))));
	//convert from model space to world view space.
	output.norm = normalize(mul((float3x3)mWorld, input.norm));
	output.tex = input.tex;
	return output;
}