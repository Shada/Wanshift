#include "resources.hlsl"
struct VS_Input
{
	float pos	: POSITION;
	float4 tex	: TEXCOORD0;
};

VS_Input main(VS_Input input)
{
	return input;
}