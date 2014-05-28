#include "resources.hlsl"

//cbuffer FogSim	: register(b5)
//{
//	float3	fogColor;
//	float	fogStartDepth;
//	float3	fogHighlightColor;
//	float	fogGlobalDensity;
//	float3	fogSunDir;
//	float	fogHeightFalloff;
//};

//cbuffer cbLightInfo		: register(b4)
//{
//	int		lightCount;
//	float3  lightDir;
//	float4	lightPos[maxLightCount];
//	float4	lightColor[maxLightCount];
//	float4	lightReach[maxLightCount];
//	matrix  lightView;
//};

struct PS_INPUT
{
	float4 position	: SV_Position;
	float2 texCoord	: COLOR;
};

Texture2D texDepth			: register(t1);
Texture2D texColor			: register(t2);

float4 main(PS_INPUT input) : SV_TARGET
{
	return float4(0,1,0,1);
//	float4 fogTint;
//
//	float c = texDepth.Sample(samPoint, input.texCoord).r;
//
//	float4 currentPosition;
//	currentPosition.x = input.texCoord.x * 2.0f - 1.0f;
//	currentPosition.y = -(input.texCoord.y * 2.0f - 1.0f);
//	currentPosition.z = c;
//	currentPosition.w = 1.0f;
//
//	float3 eyePixel = currentPosition.xyz - cameraPos.xyz;
//
//	fogTint = texColor.Sample(samLinear,input.texCoord);
//
//	//float3 applyFog(float3 originalColor, float eyePosY, float3 eyeToPixel, float sDir)
//	float3 eyeToPixel	= eyePixel;
//	float  eyePosY		= cameraPos.y;
//	float3 sDir			= lightDir;
//	float3 originalColor= fogColor.xyz;
//
//	float pixelDistance		= length( eyeToPixel );
//	float3 eyeToPixelNorm	= eyeToPixel / pixelDistance;
//
//	float fogDistance		= max( pixelDistance - fogStartDepth, 0.0 );	// seen by viewer
//	float fogHeightDensity	= exp( -fogHeightFalloff * eyePosY );
//	float intensity			= fogDistance * fogHeightDensity;
//	float eyeToPixelY		= eyeToPixel.y * ( fogDistance / pixelDistance );
////	
//	float t	= fogHeightFalloff * eyeToPixelY;
////
//	const float thresholdT	= 0.001;
//	float heightIntensity	= abs( t ) > thresholdT ? ( 1.0 - exp( -t ) ) / t : 1.0;
////
//	float factor = exp( -fogGlobalDensity * intensity * heightIntensity );
////
////	//float sunFactor = saturate( dot(eyeToPixelNorm, fogSunDir) );
//	float sunFactor = saturate( dot(eyeToPixelNorm, sDir) );
//	sunFactor		= pow( sunFactor, 8.0 );
////
//	float3 finalFogColor = lerp( fogColor, fogHighlightColor, sunFactor );
////
//	finalFogColor = lerp( finalFogColor, originalColor, factor );
//////}
//	//if(c > fogStartDepth)
//		fogTint.xyz = finalFogColor;
//	fogTint.w = 1;
//	return fogTint;
}