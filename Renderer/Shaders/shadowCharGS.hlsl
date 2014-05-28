#include "resources.hlsl"

cbuffer cbLightInfo		: register(b4)
{
	int		lightCount;
	float3  lightDir;
	float4	lightPos[maxLightCount];
	float4	lightColor[maxLightCount];
	float4	lightReach[maxLightCount];
	matrix  lightView;
	matrix  lightProj;
	matrix	toShadowSpace;
	matrix	CascadeViewProj[4];
};

cbuffer AnimationMatrices : register(b5)
{
	float4 interpolation;
	matrix animMatrixFrame1[15];
	matrix animMatrixFrame2[15];
};

struct GS_Input
{
	float4 pos	: SV_POSITION;
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD0;
	float weight[15] : TYNGD;
};

struct GS_OUTPUT
{
	float4 Pos		: SV_Position;
	uint RTIndex	: SV_RenderTargetArrayIndex;
};

[maxvertexcount(12)]
void main(triangle GS_Input input[3], inout TriangleStream<GS_OUTPUT> OutStream)
{

	GS_OUTPUT output = (GS_OUTPUT)0;

	for (int k = 0; k < 4; k++)
	{
		for (int m = 0; m < 3; m++)
		{
			float totweight = 0;
			matrix influence;

			for (int i = 0; i < 15; i++)
			{
				matrix temp = animMatrixFrame1[i];
				if (input[m].weight[i] == 0.0f)
					continue;
				[unroll]
				for (int j = 0; j < 4; j++)
				{
					temp[j][0] *= input[m].weight[i];
					temp[j][1] *= input[m].weight[i];
					temp[j][2] *= input[m].weight[i];
					temp[j][3] *= input[m].weight[i];
				}
				[unroll]
				for (int k = 0; k < 4; k++)
				{
					influence[k][0] += temp[k][0];
					influence[k][1] += temp[k][1];
					influence[k][2] += temp[k][2];
					influence[k][3] += temp[k][3];
				}

				totweight += input[m].weight[i];
			}

			float4 animPos = input[m].pos;

			if (totweight != 0.0f)
			{
				animPos = mul(influence, input[m].pos);
				animPos /= totweight;
			}
			//convert from model space to world, view then screen space.


			output.RTIndex = k;
			output.Pos = mul(CascadeViewProj[k], mul(mWorld, animPos));
			//convert from model space to world view space.
			OutStream.Append(output);


		}
		OutStream.RestartStrip();

	}

	//convert from model space to world, view then screen space.



	////convert from model space to world view space.

	//for (int iFace = 1; iFace < NROFCASCADES; iFace++)
	//{
	//	GS_OUTPUT output;

	//	output.RTIndex = iFace;

	//	for (int v = 0; v < 3; v++)
	//	{
	//		output.Pos = mul(mWorld, InPos[v].pos);
	//		output.Pos = mul(CascadeViewProj[iFace], output.Pos);

	//		OutStream.Append(output);
	//	}
	//	OutStream.RestartStrip();
	//}

}