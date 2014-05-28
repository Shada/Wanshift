#include "resources.hlsl"

Texture2D tex01	: register( t1 );
Texture2D depthSoft : register( t2 );
Texture2D depthGround : register( t3 );
Texture2D depthOpacity : register( t4 );

struct PS_Input
{
	float4		pos				: SV_Position;
	float2		tex				: TEXCOORD0;
	float		alpha			: ALPHA;
	float4		screenTex		: TEXCOORD1;
	float4		ssLightPos		: TEXCOORD2;
	float4		worldPos		: TEXCOORD3;
};

float4 main(PS_Input input) : SV_TARGET0
{
	//-----------------------------------------------------------------------------
	//deep opacity shadow map
	//-----------------------------------------------------------------------------
	float2 lightVPos = input.ssLightPos.xy / input.ssLightPos.w;
	float inputLDepth = input.ssLightPos.z / input.ssLightPos.w;
	float2 lightTexPos = (lightVPos + 1) * 0.5;
	lightTexPos.y = 1 - lightTexPos.y;
	//if(!(lightTexPos.x > 0 && lightTexPos.x <= 1))
	//	discard;
	//if(inputLDepth >= 0.99985)
	//	return float4(1,1,1,1);
	/*if(saturate(lightTexPos.x) != lightTexPos.x || saturate(lightTexPos.y) != lightTexPos.y || lightTexPos.y < 0)
		return float4(1,0,0,1);*/
	
	float lightDepth	= depthSoft.Sample( PointSamplerClamp, lightTexPos ).x;

	float resDivx = (1/resolution.x);
	float resDivy = (1/resolution.y);

	float2 xy;
	float4 temp;

	float3 op = float3(0,0,0);

	for(int i=-3; i <= 3; i++)
	{
		for(int j=-3;j <= 3;j++)
		{
			xy = float2(lightTexPos.x - j*resDivx,lightTexPos.y - i*resDivy);
			temp = depthOpacity.Sample( samAnisotropic, xy);
			op += temp.xyz;
		}
	}
	op /= 49.0;

	float finalOpacity;

	float range = 1.0 - lightDepth;
	float unit = range * 0.12;

	if(inputLDepth >= lightDepth + 1.5*unit) //layer3
	{
		finalOpacity = op.z;
		//return float4(1,0,0,1);
	}
	else if(inputLDepth > lightDepth + 0.75*unit) //layer3
	{
		finalOpacity = lerp( op.y, op.z, smoothstep( 0.0, 1.0, (inputLDepth-lightDepth)/unit ) );
//		finalOpacity = op.z*(inputLDepth-lightDepth)/unit + op.y * (unit-(inputLDepth-lightDepth))/unit;
		//return float4(0,1,0,1);
	}
	else if( inputLDepth > lightDepth+0.10*unit) //layer2 + layer3
	{
		finalOpacity = lerp( op.x, op.y, smoothstep( 0.0, 1.0, (inputLDepth-lightDepth)/unit ) );
//		finalOpacity = op.y*(inputLDepth-lightDepth)/unit + op.x*(unit-(inputLDepth-lightDepth))/unit;
		//return float4(0,0,1,1);
	}
	else //layer1 + layer2 + layer3
	{
		finalOpacity = op.x*(inputLDepth-lightDepth)/unit;
		//return float4(0,0,0,1);
	}

	float trans = saturate( exp(-1*finalOpacity) );

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

	float depthDiff = saturate( ( depthViewParticle.z/depthViewParticle.w - depthViewSample.z/depthViewSample.w) * 0.005 );
	
	//-----------------------------------------------------------------------------
	//output
	//-----------------------------------------------------------------------------
	float4 texColor =  tex01.Sample(samLinear, input.tex);	//obtain color from texture
	texColor.xyz *= trans;
	texColor.w *= input.alpha * depthDiff;
	return texColor;
}