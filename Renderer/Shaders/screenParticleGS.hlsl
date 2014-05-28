#include "resources.hlsl"

struct GS_Input
{
	float2 pos		: ANCHOR;
	float2 size		: TEXCOORD0;
};

struct PS_Input
{
	float4 pos	: SV_POSITION;
	float2 tex	: TEXCOORD0;
};

[maxvertexcount(4)]
void main( point GS_Input input[1], inout TriangleStream<PS_Input> triStream )
{
	float2 quadTexCoords[4] =
	{
		float2( 0.0f,	0.0f ),
		float2( 1.0f,	0.0f ),
		float2( 0.0f,	1.0f ),
		float2( 1.0f,	1.0f )
	};

	float4 positions[4] =
	{
		float4(	input[0].pos.x,						input[0].pos.y,						0, 1 ),
		float4(	input[0].pos.x + input[0].size.x,	input[0].pos.y,						0, 1 ),
		float4(	input[0].pos.x,						input[0].pos.y - input[0].size.y,	0, 1 ),
		float4(	input[0].pos.x + input[0].size.x,	input[0].pos.y - input[0].size.y,	0, 1 )
	};

	PS_Input output = (PS_Input)0;

	[unroll]
	for(unsigned int i = 0; i < 4; i++)
	{
		output.pos			= positions[i];
		output.tex			= quadTexCoords[i];
		triStream.Append(output);
	}
}