#include "resources.hlsl"
Texture2D texSceneMask			: register(t1); // the texture to blur

struct PS_Input
{
	float4 pos	: SV_POSITION;
	float2 tex	: TEXCOORD0;
};

float4 main(PS_Input input) : SV_TARGET0
{
	float blurSize = (1.0 / resolution.x);
	float3 col = float3(0, 0, 0);

	// blur horizontal with 9 samples
	col += texSceneMask.Sample(samLinear2, float2(input.tex.x - 5.0 * blurSize, input.tex.y)).rgb * 0.06;
	col += texSceneMask.Sample(samLinear2, float2(input.tex.x - 4.0 * blurSize, input.tex.y)).rgb * 0.08;
	col += texSceneMask.Sample(samLinear2, float2(input.tex.x - 3.0 * blurSize, input.tex.y)).rgb * 0.12;
	col += texSceneMask.Sample(samLinear2, float2(input.tex.x - 2.0 * blurSize, input.tex.y)).rgb * 0.16;
	col += texSceneMask.Sample(samLinear2, float2(input.tex.x - 1.0 * blurSize, input.tex.y)).rgb * 0.20;
	col += texSceneMask.Sample(samLinear2, float2(input.tex.x, input.tex.y)).rgb * 0.28;
	col += texSceneMask.Sample(samLinear2, float2(input.tex.x + 1.0 * blurSize, input.tex.y)).rgb * 0.20;
	col += texSceneMask.Sample(samLinear2, float2(input.tex.x + 2.0 * blurSize, input.tex.y)).rgb * 0.16;
	col += texSceneMask.Sample(samLinear2, float2(input.tex.x + 3.0 * blurSize, input.tex.y)).rgb * 0.12;
	col += texSceneMask.Sample(samLinear2, float2(input.tex.x + 4.0 * blurSize, input.tex.y)).rgb * 0.08;
	col += texSceneMask.Sample(samLinear2, float2(input.tex.x + 5.0 * blurSize, input.tex.y)).rgb * 0.06;

	return float4(saturate(col), 1);
}