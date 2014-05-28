#include "resources.hlsl"

cbuffer RainConstants : register( b13 )
{
	matrix	rViewProjMatrix	;	// 64
	float3	viewDir			;	// 12
	float	rScale			;	// 4
	float4	ambientColor	;	// 16
};								// = 96


struct PS_Input
{
	float4	position	: SV_Position;
	float	clip		: FOO;//SV_ClipDistance0;
	float2	texCoord	: TEXCOORD0;
};

Texture2D rainStreakTexture : register( t1 );

float4 main(PS_Input input) : SV_TARGET
{
	//return float4(1,0,0,1);
	float texAlpha = rainStreakTexture.Sample(samLinear, input.texCoord).r;

	return float4(ambientColor.rgb, ambientColor.a * texAlpha);
}