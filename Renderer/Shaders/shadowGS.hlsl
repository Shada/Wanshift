#include "resources.hlsl"
#define NROFCASCADES 4

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

struct PS_Input
{
	float4 pos	: SV_POSITION;
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD1;
};

struct GS_OUTPUT
{
	float4 Pos		: SV_Position;
	uint RTIndex	: SV_RenderTargetArrayIndex;
};

[maxvertexcount(12)]
void main(triangle PS_Input InPos[3], inout TriangleStream<GS_OUTPUT> OutStream)
{

	for (int iFace = 1; iFace < NROFCASCADES; iFace++)
	{
		GS_OUTPUT output;

		output.RTIndex = iFace;

		for (int v = 0; v < 3; v++)
		{
			output.Pos = mul(mWorld, InPos[v].pos); //  - lightDir*5
			output.Pos = mul(CascadeViewProj[iFace], output.Pos);

			OutStream.Append(output);
		}
		OutStream.RestartStrip();
	}

}