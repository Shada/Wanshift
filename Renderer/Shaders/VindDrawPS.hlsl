#include "resources.hlsl"

Texture2D tex01	: register(t1);
Texture2D depthTex	: register(t2);

struct PS_Input
{
	float4 pos		: SV_POSITION;
	float2 texCoord	: TEXCOORD0;
	float alpha		: ALPHA;
	float4 screenTex		: TEXCOORD1;
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

	float4 texColor = tex01.Sample(samLinear, _input.texCoord);	//obtain color from texture

	texColor *= float4((float3(0.7, 0.5, 0) * (timeofday)), 1);

	texColor.w *= _input.alpha * depthDiff;
	return texColor;
}