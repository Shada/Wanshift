#include "resources.hlsl"
Texture2D tex01	: register( t1 );
struct PS_Input
{
	float4	pos			: SV_POSITION;
	float2	tex			: TEXCOORD;
};

float4 main(PS_Input input) :SV_TARGET0
{
	return tex01.Sample(samAnisotropic, input.tex);	//obtain color from texture
}