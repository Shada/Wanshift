#include "resources.hlsl"

Texture2D tex01	: register( t1 );
Texture2D depthSoft : register( t2 );
//Texture2D depthOpacity : register( t3 );

struct PS_Input
{
	float4		pos				: SV_Position;
	float2		tex				: TEXCOORD0;
	float		alpha			: ALPHA;
	float4		screenTex		: TEXCOORD1;
};

float4 main(PS_Input input) : SV_TARGET0
{
	float4 output;
	float depth = input.pos.z;
	float2 tc = float2( input.pos.x / resolution.x, input.pos.y / resolution.y );
	float mappedDepth = depthSoft.Sample(samPoint, tc).x;
	float range = 1 - mappedDepth;
	float unit = range * 0.05;
	float texAlpha =  input.alpha * tex01.Sample(samLinear, input.tex).w;
	float op = texAlpha * 0.1;

	if(depth >= mappedDepth + 3 * unit) //layer3
		output = float4(0,0,op,1);
	else if( depth > mappedDepth + 2 * unit) // layer3
		output = float4(0,0,op,1);
	else if( depth > mappedDepth + unit) //layer2 + layer3
		output = float4(0,op,op,1);
	else
		output = float4(op,op,op,1);

	return output;//float4(1,1,1,1);//float z = 
}