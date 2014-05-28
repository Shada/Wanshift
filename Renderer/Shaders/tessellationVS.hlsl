struct VS_Input
{
	float3 pos	: POSITION;
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD0;
};

struct VS_OUTPUT
{
	float3 pos	: POSITION;
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD0;
	float2 wTex : TEXCOORD1;
};

cbuffer cbTerrain	:register(b5)
{
	float2	tStartPos;
	float	tPointStep;
	float	tPadding;
};

// should probably return HS_Input later
VS_OUTPUT main(VS_Input input)
{
	//just passing through. Nothing to see here.
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.wTex = input.pos.xz - tStartPos;
	output.pos = input.pos;
	output.norm = input.norm;
	output.tex = input.tex;
	return output;
}