#include "resources.hlsl"
Texture2D texBlurH			: register(t1); // The rendertarget for the horizontal blur

struct PS_Input
{
	float4 pos	: SV_POSITION;
	float2 tex	: TEXCOORD0;
};

float4 main(PS_Input input) : SV_TARGET0
{
	float blurSize = (1.0 / resolution.y);
	float3 col = float3(0, 0, 0);

	// blur horizontal with 9 samples
	col += texBlurH.Sample(samLinear2, float2(input.tex.x, input.tex.y - 6.0 * blurSize)).rgb * 0.06;
	col += texBlurH.Sample(samLinear2, float2(input.tex.x, input.tex.y - 4.0 * blurSize)).rgb * 0.08;
	col += texBlurH.Sample(samLinear2, float2(input.tex.x, input.tex.y - 3.0 * blurSize)).rgb * 0.12;
	col += texBlurH.Sample(samLinear2, float2(input.tex.x, input.tex.y - 2.0 * blurSize)).rgb * 0.16;
	col += texBlurH.Sample(samLinear2, float2(input.tex.x, input.tex.y - 1.0 * blurSize)).rgb * 0.20;
	col += texBlurH.Sample(samLinear2, float2(input.tex.x, input.tex.y)).rgb * 0.24;
	col += texBlurH.Sample(samLinear2, float2(input.tex.x, input.tex.y + 1.0 * blurSize)).rgb * 0.20;
	col += texBlurH.Sample(samLinear2, float2(input.tex.x, input.tex.y + 2.0 * blurSize)).rgb * 0.16;
	col += texBlurH.Sample(samLinear2, float2(input.tex.x, input.tex.y + 3.0 * blurSize)).rgb * 0.12;
	col += texBlurH.Sample(samLinear2, float2(input.tex.x, input.tex.y + 4.0 * blurSize)).rgb * 0.08;
	col += texBlurH.Sample(samLinear2, float2(input.tex.x, input.tex.y + 6.0 * blurSize)).rgb * 0.06;

	return float4(col, 1);
}