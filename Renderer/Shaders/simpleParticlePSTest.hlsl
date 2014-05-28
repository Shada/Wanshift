#include "resources.hlsl"

Texture2D tex01	: register( t1 );

struct PS_Input
{
	float4 pos		: SV_POSITION;
	float2 texCoord	: TEXCOORD;
	float alpha		: ALPHA;
};

float4 main(PS_Input _input) : SV_TARGET0
{
	float4 output;
	output =  tex01.Sample(samLinear, _input.texCoord);

	output.w *= _input.alpha;

	if(output.r < 1)
		return float4(0,0,0,1);

	return output;
}