#include "resources.hlsl"

struct VS_Input
{
	float3 pos	: POSITION;
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD0;
	float weight[15] : ANIM0;
};

cbuffer cbLightInfo		: register(b4)
{
	int		lightCount;
	float3  lightDir;
	float4	lightPos[maxLightCount];
	float4	lightColor[maxLightCount];
	float4	lightReach[maxLightCount];
	matrix  lightView;
	matrix  lightProj;
	matrix	toShadowSpace;
	matrix	CascadeViewProj[4];
};

struct GS_Input
{
	float4 pos	: SV_POSITION;
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD0;
	float weight[15] : TYNGD;
};

GS_Input main(VS_Input input)
{
	GS_Input output = (GS_Input)0;

	//convert from model space to world, view then screen space.
	output.pos = float4(input.pos, 1.0);
	//convert from model space to world view space.
	output.norm = float3(0, 0, 0);
	output.tex = input.tex;

	[unroll]
	for (int i = 0; i < 15; i++)
	{
		output.weight[i] = input.weight[i];
	}

	return output;
}