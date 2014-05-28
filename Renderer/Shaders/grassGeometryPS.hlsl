#include "resources.hlsl"

Texture2D kd		: register(t1);

static const float maxBladeHeight = 17.0;

cbuffer cbShifting : register(b6)
{
	float interpVal;
	float3 shiftpadd;
};

struct PS_Input
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL;
	float2 tex			: TEXC;
	float random		: RAND;
	float brightness	: BRIGHT;
	float height		: HEIGHT;
};

PS_MRTOutput main(PS_Input input)
{
	PS_MRTOutput output = (PS_MRTOutput)0;
	float color = kd.Sample(samPoint, input.tex).r;
	float ka = .2+0.8 * (timeofday);
	// make pixels close to the root darker and add a random tint to the over all color
	float col;
	if(interpVal < 0)
		col = abs(1 - input.random * (input.height / maxBladeHeight) * (input.height / maxBladeHeight));
	else
		col = abs(1 - input.random);

	output.color = saturate(color * float4(col, 1, 0, 0) * ((1 - input.tex.y) +  + input.brightness));
	output.normal = float4((input.normal + 1) * 0.5, 0);
	output.specular = float4(output.color.xyz, ka);
	output.glow = float4(0, 0, 0, 0);

	return output;
}