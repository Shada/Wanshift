#include "resources.hlsl"

Texture2D tex01	: register( t1 );

struct PS_Input
{
	float4		pos				: SV_Position;
	float2		tex				: TEXCOORD0;
	float		alpha			: ALPHA;
	//float4		screenTex		: TEXCOORD1;
};

float4 main(PS_Input input) : SV_TARGET0
{
	float4 texColor =  tex01.Sample(samLinear, input.tex);	//obtain color from texture
	texColor.w *= input.alpha;
	if(texColor.w <= 0)
		discard;
	return float4(0,0,0,0);
}