#include "resources.hlsl"

Texture2D texDepth			: register(t1);
Texture2DArray shadowMap	: register(t2);

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

struct PS_Input
{
	float4 pos	: SV_POSITION;
	float2 tex	: TEXCOORD0;
};

float CascadedShadow(float3 position, float2 texcoord)
{
	// Transform the world position to shadow space
	//float4 posShadowSpace = mul(float4(position, 1.0), toShadowSpace);
	float4 posShadowSpace = mul(toShadowSpace, float4(position, 1.0));
		float bias[4];

	bias[0] = 0.005;
	bias[1] = 0.005;
	bias[2] = 0.001;
	bias[3] = 0.003;

	float4 posCascadeSpace[4];
	float4 inCascadeX;
	float4 inCascadeY;
	float4 inCascadeZ;
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		posCascadeSpace[i] = mul(CascadeViewProj[i],float4(position,1.0f));

		inCascadeX[i] = abs(posCascadeSpace[i].x) <= 1.0;
		inCascadeY[i] = abs(posCascadeSpace[i].y) <= 1.0;
		inCascadeZ[i] = abs(posCascadeSpace[i].z) <= 1.0;
	}

	float4 inCascade = inCascadeX * inCascadeY * inCascadeZ;

	// Prepare a mask for the highest quality cascade the position is in
	float4 bestCascadeMask = inCascade;
	bestCascadeMask.yzw = (1.0 - bestCascadeMask.x) * bestCascadeMask.yzw;
	bestCascadeMask.zw = (1.0 - bestCascadeMask.y) * bestCascadeMask.zw;
	bestCascadeMask.w = (1.0 - bestCascadeMask.z) * bestCascadeMask.w;
	float bestCascade = dot(bestCascadeMask, float4(0.0, 1.0, 2.0, 3.0));

	// Pick the position in the selected cascade
	float3 UVD;

	UVD.x = posCascadeSpace[bestCascade].x;
	UVD.y = posCascadeSpace[bestCascade].y;
	UVD.z = posCascadeSpace[bestCascade].z;

	// Convert to shadow map UV values
	UVD.xy = 0.5 * UVD.xy + 0.5;
	UVD.y = 1.0 - UVD.y;

	float3 temp = float3(UVD.xy, bestCascade);


	//// Compute the hardware PCF value
	float shadow = shadowMap.SampleCmpLevelZero(LinerarSamplerClampShadow, temp, UVD.z - bias[temp.z]);
	//float charShadow = 1.0;
	//if (bestCascade == 0)
	//{
	//	charShadow = shadow;

	//	bestCascade += 1;

	//	UVD.x = posCascadeSpace[bestCascade].x;
	//	UVD.y = posCascadeSpace[bestCascade].y;
	//	UVD.z = posCascadeSpace[bestCascade].z;

	//	// Convert to shadow map UV values
	//	UVD.xy = 0.5 * UVD.xy + 0.5;
	//	UVD.y = 1.0 - UVD.y;

	//	temp = float3(UVD.xy, bestCascade);

	//	shadow = shadowMap.SampleCmpLevelZero(LinerarSamplerClampShadow, temp, UVD.z - bias[temp.z]);

	//	//if (shadow < 1)
	//	//{

	//	//	UVD.x = posCascadeSpace[bestCascade].x;
	//	//	UVD.y = posCascadeSpace[bestCascade].y;
	//	//	UVD.z = posCascadeSpace[bestCascade].z;

	//	//	// Convert to shadow map UV values
	//	//	UVD.xy = 0.5 * UVD.xy + 0.5;
	//	//	UVD.y = 1.0 - UVD.y;

	//	//	temp = float3(UVD.xy, bestCascade);

	//	//	float4 charShadow = shadowMap.SampleCmpLevelZero(LinerarSamplerClampShadow, temp, UVD.z - bias[temp.z]);


	//	//}
	//}

	//if (bestCascade == 1)
	//{
	//	if (shadow == 1)
	//	{
	//		//bestCascade += 1;

	//		UVD.x = posCascadeSpace[bestCascade].x;
	//		UVD.y = posCascadeSpace[bestCascade].y;
	//		UVD.z = posCascadeSpace[bestCascade].z;

	//		// Convert to shadow map UV values
	//		UVD.xy = 0.5 * UVD.xy + 0.5;
	//		UVD.y = 1.0 - UVD.y;

	//		temp = float3(UVD.xy, bestCascade);

	//		shadow = shadowMap.SampleCmpLevelZero(LinerarSamplerClampShadow, temp, UVD.z - bias[temp.z]);
	//	}
	//}

	//if (bestCascade == 2)
	//{
	//	if (shadow == 1)
	//	{
	//		//bestCascade += 1;

	//		UVD.x = posCascadeSpace[bestCascade].x;
	//		UVD.y = posCascadeSpace[bestCascade].y;
	//		UVD.z = posCascadeSpace[bestCascade].z;

	//		// Convert to shadow map UV values
	//		UVD.xy = 0.5 * UVD.xy + 0.5;
	//		UVD.y = 1.0 - UVD.y;

	//		temp = float3(UVD.xy, bestCascade);

	//		shadow = shadowMap.SampleCmpLevelZero(LinerarSamplerClampShadow, temp, UVD.z - bias[temp.z]);
	//	}
	//}

/*	if (charShadow < 1)
	{
		shadow = saturate(charShadow + 1.0 - any(bestCascadeMask));

	}
	else
	{
		shadow = saturate(shadow + 1.0 - any(bestCascadeMask));
	}*/	
	shadow = saturate(shadow + 1.0 - any(bestCascadeMask));

	return shadow;
}

float4 cascadeShow(float3 position)
{
	// Transform the world position to shadow space
	//float4 posShadowSpace = mul(float4(position, 1.0), toShadowSpace);
	float4 posShadowSpace = mul(toShadowSpace, float4(position, 1.0));
	float bias[4];

	bias[0] = 0.001;
	bias[1] = 0.01;
	bias[2] = 0.05;
	bias[3] = 0.0007;

	float4 posCascadeSpace[4];
	float4 inCascadeX;
	float4 inCascadeY;
	float4 inCascadeZ;
	for (int i = 0; i < 4; i++)
	{
		posCascadeSpace[i] = mul(CascadeViewProj[i], float4(position, 1.0f));

		inCascadeX[i] = abs(posCascadeSpace[i].x) <= 1.0;
		inCascadeY[i] = abs(posCascadeSpace[i].y) <= 1.0;
		inCascadeZ[i] = abs(posCascadeSpace[i].z) <= 1.0;
	}

	float4 inCascade = inCascadeX * inCascadeY * inCascadeZ;

	// Prepare a mask for the highest quality cascade the position is in
	float4 bestCascadeMask = inCascade;
	bestCascadeMask.yzw = (1.0 - bestCascadeMask.x) * bestCascadeMask.yzw;
	bestCascadeMask.zw = (1.0 - bestCascadeMask.y) * bestCascadeMask.zw;
	bestCascadeMask.w = (1.0 - bestCascadeMask.z) * bestCascadeMask.w;
	float bestCascade = dot(bestCascadeMask, float4(0.0, 1.0, 2.0, 3.0));

	if (bestCascade == 0)
	{
		return float4(1, 0, 0, 1);
	}
	else if (bestCascade == 1)
	{
		return float4(0, 1, 0, 1);
	}
	else if (bestCascade == 2)
	{
		return float4(0, 0, 1, 1);
	}
	else if (bestCascade == 3)
	{
		return float4(1, 0, 1, 1);
	}
	else if (bestCascade == 4)
	{
		return float4(0, 1, 1, 1);
	}

	return float4(0, 0, 0, 1);
}

// draws a lit scene to a full screen quad
float4 main(PS_Input input) : SV_TARGET0
{
	float depth = texDepth.Sample(samPoint, input.tex).r;

	float4 currentPosition;

	currentPosition.x = input.tex.x * 2.0f - 1.0f;
	currentPosition.y = -(input.tex.y * 2.0f - 1.0f);
	currentPosition.z = depth;
	currentPosition.w = 1.0f;

	// Transform the screen space position in to world-space
	currentPosition = mul(mProjInv, currentPosition);
	currentPosition = mul(mViewInv, currentPosition);
	currentPosition /= currentPosition.w;

	float shadowAtt = CascadedShadow(currentPosition.xyz, input.tex);

	return float4(shadowAtt, shadowAtt, shadowAtt, 1);

}