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

Texture2D<float4>	shadowCompile	: register(t1);
Texture2D<float4>	texNormal		: register(t2);
RWTexture2D<float4> output : register(u1);


static const int g_iNumDispatch = 32;
static const int g_iNumThreads = 32;
static const int g_iDimSize = g_iNumDispatch * g_iNumThreads;

static const int blurWidth = 2;
static const int blurHeight = 2;


[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
	float4 outColor = float4(0, 0, 0, 1);
	float4 black = float4(0, 0, 0, 1);

	for (int i = 0; i < blurWidth; i++)
	{
		for (int j = 0; j < blurHeight; j++)
		{
			//black += shadowCompile[float2(threadID.x - ((blurWidth * 2)) + (i * 4), threadID.y - ((blurHeight * 2)) + (j * 4))] >= float4(1, 1, 1, 1);
			//outColor += shadowCompile[float2(threadID.x - ((blurWidth * 2) / 2) + (i * 2), threadID.y - ((blurHeight * 2) / 2) + (j * 2))];
			//outColor += shadowCompile[float2(threadID.x - ((blurWidth * 2) / 2) + (i * 2), threadID.y - ((blurHeight * 2) / 2) + (j * 2))] / (blurWidth*blurHeight);
			outColor += (shadowCompile[float2(threadID.x - (blurWidth)+(i * 2), threadID.y - ((blurHeight)+(j * 2)))] + 0.3f) / (blurWidth*blurHeight); // Ordinary blur

		}
	}

//	outColor = float4(1, 1, 1, 1);

	/*float3 normal = texNormal[threadID.xy].xyz;

	float angle = dot(normalize(normal), normalize(lightDir));

	if (abs(angle) < 0.1)
	{
		outColor = float4(0.1, 0.1, 0.1, 1);
	}*/

	//outColor = float4(abs(lightDir), 1);
	//outColor = normal;

	/*if (black.x >(blurWidth*blurHeight * 0.5))
		outColor = float4(1, 1, 1, 1);*/

	//outColor += shadowCompile[float2(threadID.x - ((blurWidth)+(i * 2), threadID.y - ((blurHeight)+(j * 2))] / (blurWidth*blurHeight); // Ordinary blur

	//outColor = shadowCompile[float2(threadID.x , threadID.y )];

	outColor.w = 1;
	output[threadID.xy] = outColor;
}