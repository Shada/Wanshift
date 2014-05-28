#include "resources.hlsl"

struct VS_Input
{
	float2 pos		: ANCHOR;
	float2 size		: TEXCOORD0;
};

VS_Input main(VS_Input input)
{
	return input;
}