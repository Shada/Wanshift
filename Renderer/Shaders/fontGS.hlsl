#include "resources.hlsl"

cbuffer cbText			: register(b0)
{
	float4	textTintAlpha;	//text tint: xyz, text alpha: w
	float2	textPos;		//text position (top left corner)
	float2	textScale;		//text scale
	float2	imageSize;		//font image size
	//float	textRotation;	//text rotation (from middle)
	float2	padda;			//padding
};

struct VS_Input
{
	float pos	: POSITION;
	float4 tex	: TEXCOORD0;
};

struct PS_Input
{
	float4	pos			: SV_POSITION;
	float2	tex			: TEXCOORD0;
};

[maxvertexcount(4)]
void main( point VS_Input input[1], inout TriangleStream<PS_Input> triStream )
{
	PS_Input output = (PS_Input)0;
	float2 pos =  textPos + float2( input[0].pos, 0 ) * textScale; //text position + letter position
	//get the 4 different texture coordinate edges used for this letter
	float4 texCoords = float4(	input[0].tex.x / imageSize.x,							//left
								input[0].tex.y / imageSize.y,							//top
								( input[0].tex.x + input[0].tex.z ) / imageSize.x,		//right
								( input[0].tex.y + input[0].tex.w ) / imageSize.y );	//bottom

	//set positions and texture coordinates of each corner of the letter to be drawn.
	output.pos = float4(	( pos.x / resolution.x ) * 2 - 1,
							( ( resolution.y - pos.y - ( input[0].tex.w * textScale.y ) ) / resolution.y ) * 2 - 1,
							0,
							1 );
	output.tex = float2( texCoords.x, texCoords.w );
	triStream.Append(output);

	output.pos = float4(	( pos.x / resolution.x ) * 2 - 1,
							( ( resolution.y - pos.y ) / resolution.y ) * 2 - 1,
							0,
							1 );
	output.tex = float2( texCoords.x, texCoords.y );
	triStream.Append(output);

	output.pos = float4(	( (pos.x + ( input[0].tex.z * textScale.x ) ) / resolution.x) * 2 - 1,
							( (resolution.y - pos.y - ( input[0].tex.w * textScale.y ) ) / resolution.y ) * 2 - 1,
							0,
							1 );
	output.tex = float2( texCoords.z, texCoords.w );
	triStream.Append(output);

	output.pos = float4(	( ( pos.x + ( input[0].tex.z * textScale.x ) ) / resolution.x ) * 2 - 1,
							( ( resolution.y - pos.y ) / resolution.y ) * 2 - 1,
							0,
							1 );
	output.tex = float2( texCoords.z, texCoords.y );
	triStream.Append(output); 
}