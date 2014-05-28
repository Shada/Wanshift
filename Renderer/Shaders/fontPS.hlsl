#include "resources.hlsl"
Texture2D tex01	: register( t1 );

cbuffer cbText			: register(b0)
{
	float4	textTintAlpha;	//text tint: xyz, text alpha: w
	float2	textPos;		//text position (top left corner)
	float2	textScale;		//text scale
	float2	imageSize;		//font image size
	//float	textRotation;	//text rotation (from middle)
	float2	padda;			//padding
};

struct PS_Input
{
	float4	pos			: SV_POSITION;
	float2	tex			: TEXCOORD;
};

float4 main(PS_Input input) :SV_TARGET0
{
	float4 texColor =  tex01.Sample(samAnisotropic, input.tex);	//obtain color from texture
	return texColor * textTintAlpha;						//multiply color with tint and alpha
}