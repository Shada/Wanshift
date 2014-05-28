#include "resources.hlsl"

Texture2D depthTex	: register(t1);
Texture2D alphaTex	: register(t2);
Texture2D tex01		: register(t3);
Texture2D tex02		: register(t4);

struct PS_Input
{
	float4 pos		: SV_POSITION;
	float2 texCoord	: TEXCOORD0;
	float4 screenTex: TEXCOORD1;
	float2 alphaTex : TEXCOORD2;
	int tex			: TEXTURE1;
};

float4 main(PS_Input _input) : SV_TARGET0
{
	float2 screenVPos = _input.screenTex.xy / _input.screenTex.w;
	float inputDepth = _input.screenTex.z / _input.screenTex.w;
	float2 screenPos = (screenVPos + 1) * 0.5;
	screenPos.y = 1 - screenPos.y;

	float depth = depthTex.Sample(samPoint, screenPos).r;
	float4 depthViewParticle = mul(mProjInv, float4(screenVPos, inputDepth, 1));
	float4 depthViewSample = mul(mProjInv, float4(screenVPos, depth, 1));

	float depthDiff = saturate((depthViewParticle.z / depthViewParticle.w - depthViewSample.z / depthViewSample.w) * 0.005);

	float4 texColor = float4(0, 0, 0, 0);

	if (_input.tex == 0)
	{
		texColor = tex01.Sample(samLinear, _input.texCoord); //obtain color from texture
	}
	else if (_input.tex == 1)
	{
		texColor = tex02.Sample(samLinear, _input.texCoord); //obtain color from texture
	}

	float alpha = alphaTex.Sample(samLinear, _input.alphaTex).r; 

	if(timeofday * 12 < 4)
	{

	}
	else if (timeofday * 12 < 6)
	{
		alpha *= 1 - ((timeofday * 12) - 4) / 2;
	}
	else
	{
		alpha = 0;
	}

	texColor.w *= alpha * depthDiff;

	return texColor;
}