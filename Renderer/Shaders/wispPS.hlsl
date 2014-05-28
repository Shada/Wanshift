#include "resources.hlsl"

Texture2D tex01	: register( t1 );
Texture2D tex02 : register( t2 );
Texture2D tex03 : register( t3 );
Texture2D depthGround : register( t4 );

struct PS_Input
{
	float4		pos				: SV_Position;
	float2		tex				: TEXCOORD0;
	float		alpha			: ALPHA;
	float4		screenTex		: TEXCOORD1;
	float4		worldPos		: TEXCOORD2;
};

cbuffer cbWisp : register(b5)
{
	float4 hordepos;
	float4 hordecolor;
}

float4 main(PS_Input input) : SV_TARGET0
{

	//-----------------------------------------------------------------------------
	//soft depth
	//-----------------------------------------------------------------------------

	float2 screenVPos = input.screenTex.xy / input.screenTex.w;
	float inputDepth = input.screenTex.z / input.screenTex.w;
	float2 screenPos = (screenVPos + 1) * 0.5;
	screenPos.y = 1 - screenPos.y;

	float depth = depthGround.Sample( samPoint, screenPos ).r;
	
	float4 depthViewParticle = mul(mProjInv, float4( screenVPos, inputDepth, 1 ) );
	float4 depthViewSample = mul(mProjInv, float4( screenVPos, depth, 1 ) );

	float depthDiff = saturate( ( depthViewParticle.z/depthViewParticle.w - depthViewSample.z/depthViewSample.w) * 0.05 );
	
	//-----------------------------------------------------------------------------
	//output
	//-----------------------------------------------------------------------------
	float4 texColor =  tex01.Sample(samLinear, input.tex);	//obtain color from texture
	texColor.w *= input.alpha * depthDiff;
	return texColor*hordecolor;
}