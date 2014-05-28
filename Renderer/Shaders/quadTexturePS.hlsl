#include "resources.hlsl"

struct PS_INPUT
{
	float4 position	: SV_Position;
	float2 texCoord : COLOR;
};

texture2D tScene	: register(t10);	// rendered scene before post effects
texture2D tPost		: register(t11);

float4 main(PS_INPUT input) : SV_TARGET
{

	float4 fScene = tScene.Sample(samLinear, input.texCoord);
	float4 fPost = tPost.Sample(samLinear, input.texCoord);

	float4 final = lerp(fScene, fPost, fScene.w);

	return final;
}