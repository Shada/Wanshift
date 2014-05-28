/*--------------------------------------------

			Global shader resources

----------------------------------------------*/
#define PI 3.14159265f

//int debugMRT;
#define debugMRT 25 //temporary solution. should be in a cbuffer that fits the update rate.
#define maxLightCount 25
#define maxSpotLightCount 5

//samplers
SamplerState samLinear			: register( s0 );
SamplerState samLinear2			: register( s1 );
SamplerState samPoint			: register( s2 );
SamplerState samAnisotropic		: register( s3 );
SamplerState PointSamplerClamp	: register( s4 );
SamplerComparisonState LinerarSamplerClampShadow	: register(s5);

//global textures
Texture2D randomSeed	: register( t0 );

//constant buffers
cbuffer cbWorld			: register(b0)
{
	matrix	mWorld;			//world matrix
	matrix	mWorldInv;		//world inverse matrix
};
cbuffer cbView			: register(b1)
{
	float4	cameraPos;		//camera position
	float4	cameraDir;		//camera direction
	matrix	mView;			//view matrix
	matrix	mViewInv;		//view inverse matrix
};
cbuffer cbOnce			: register(b2)
{
	matrix	mProj;			//projection matrix
	matrix	mProjInv;		//projection inverse matrix
	float2	resolution;		//screen resolution
	uint nSamples;			// 4
	float scaleDepth;		// 4
	float scaleOverDepth;	// 4
	float3 qsp;				// 4 - padding
};

cbuffer cbEveryFrame	: register(b3)
{
	float	timeStep;
	float	gameTime;
	float2	windDirection;
	float	timeofday;		// Value between 0 - 1
	float	energy;
	float2	efpadding;		// Används till glow pulsating
	float4	dayBool;		// Använder bara x värdet för timeoff day
};

cbuffer FogSim	: register(b5)
{
	float3	fogColor;
	float	fogStartDepth;
	float3	fogHighlightColor;
	float	fogGlobalDensity;
	float3	fogSunDir;
	float	fogHeightFalloff;
};
//the output for mutliple render targets (deferred rendering)
struct PS_MRTOutput
{
	float4 color	: SV_TARGET0;
	float4 normal	: SV_TARGET1;
	float4 specular	: SV_TARGET2;
	float4 glow		: SV_TARGET3;
};

//returns a random float between 0 and 1
float rand(float offset)
{
	float u = gameTime * 100 + offset;
	return randomSeed.SampleLevel(samPoint, u, 0).x;
}

float rand2(float2 offset)
{
	float2 u = float2(offset.x, (gameTime * 100) + offset.y);
		return randomSeed.SampleLevel(samPoint, u, 0).x;
}
float rand3(float2 offset)
{
	float2 u = float2(offset.x, offset.y);
		return randomSeed.SampleLevel(samPoint, u, 0).x;
}

float3 applyFog(float3 originalColor, float eyePosY, float3 eyeToPixel, float3 sDir)
{
	float pixelDistance		= length( eyeToPixel );
	float3 eyeToPixelNorm	= eyeToPixel / pixelDistance;

	float fogDistance		= max( pixelDistance - fogStartDepth, 0.0 );	// seen by viewer
	float fogHeightDensity	= exp( -fogHeightFalloff * eyePosY );
	float intensity			= fogDistance * fogHeightDensity;
	float eyeToPixelY		= eyeToPixel.y * ( fogDistance / pixelDistance );
	
	float t	= fogHeightFalloff * eyeToPixelY;

	const float thresholdT	= 0.001;
	float heightIntensity	= abs( t ) > thresholdT ? ( 1.0 - exp( -t ) ) / t : 1.0;

	float factor = exp( -fogGlobalDensity * intensity * heightIntensity );

	//float sunFactor = saturate( dot(eyeToPixelNorm, fogSunDir) );
	float sunFactor = saturate( dot(eyeToPixelNorm, sDir) );
	sunFactor		= pow( sunFactor, 8.0 );

	float3 finalFogColor = lerp( fogColor, fogHighlightColor, sunFactor );

	return lerp( finalFogColor, originalColor, factor );
}
