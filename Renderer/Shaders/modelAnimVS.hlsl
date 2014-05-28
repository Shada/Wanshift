#include "resources.hlsl"

cbuffer AnimationMatrices : register(b6)
{
	float4 interpolation;
	matrix animMatrixFrame1[15];
	matrix animMatrixFrame2[15];
};

struct VS_Input
{
	float3 pos	: POSITION;
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD0;
	float weight[15] : ANIM0;
};

struct PS_Input
{
	float4 pos	: SV_POSITION;
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD1;
};

PS_Input main(VS_Input input)
{
	PS_Input output = (PS_Input)0;
	float totweight = 0;
	matrix influence1;
	matrix influence2;
	for(int i = 0; i < 15; i++)
	{
		matrix temp1 = animMatrixFrame1[i];
		matrix temp2 = animMatrixFrame2[i];
		if(input.weight[i] == 0.0f)
			continue;
		[unroll]
		for(int j = 0; j < 4; j++)
		{
			temp1[j][0] *= input.weight[i];
			temp1[j][1] *= input.weight[i];
			temp1[j][2] *= input.weight[i];
			temp1[j][3] *= input.weight[i];
			temp2[j][0] *= input.weight[i];
			temp2[j][1] *= input.weight[i];
			temp2[j][2] *= input.weight[i];
			temp2[j][3] *= input.weight[i];
		}
		[unroll]
		for(int k = 0; k < 4; k++)
		{
			influence1[k][0] += temp1[k][0];
			influence1[k][1] += temp1[k][1];
			influence1[k][2] += temp1[k][2];
			influence1[k][3] += temp1[k][3];
			influence2[k][0] += temp2[k][0];
			influence2[k][1] += temp2[k][1];
			influence2[k][2] += temp2[k][2];
			influence2[k][3] += temp2[k][3];
		}

		totweight += input.weight[i];
	}

	float4 animPos1 = float4(input.pos, 1);
	float4 animPos2 = float4(input.pos, 1);

	if(totweight != 0.0f)
	{
		animPos1 = mul(influence1, animPos1);
		animPos1 /= totweight;
		animPos2 = mul(influence2, animPos2);
		animPos2 /= totweight;
	}

	animPos1 = lerp(animPos1, animPos2, float4(interpolation.x, interpolation.x, interpolation.x, 0));
	//convert from model space to world, view then screen space.
	output.pos = mul(mProj, mul(mView, mul(mWorld, animPos1)));
	//convert from model space to world view space.
	output.norm = normalize(mul((float3x3)mWorld, input.norm));
	output.tex = input.tex;
	return output;
}