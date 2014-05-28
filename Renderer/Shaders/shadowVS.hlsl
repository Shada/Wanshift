#include "resources.hlsl"

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

PS_Input main(VS_Input input) //: SV_Position
{
	PS_Input output = (PS_Input)0;

	//convert from model space to world, view then screen space.
	output.pos = float4(input.pos, 1.0); //  - lightDir
	//convert from model space to world view space.
	output.norm = normalize(mul(lightView, mul(mWorld, float4(input.norm, 0))).xyz);
	output.tex = input.tex;

	return output;

}