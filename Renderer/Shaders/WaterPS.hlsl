#include "resources.hlsl"

Texture2D texScene	:register( t1 );
Texture2D texDepth	:register( t2 );
Texture2D heightMap	:register( t3 );
Texture2D normalMap	:register( t4 );
Texture2D foamMap	:register( t5 );

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

static const float waterLevel = 50.0;

static const float waterFade = 0.15; //how deep anything need to be to fade out in the water
static const float normalScaling = 1.0; 
static const float maxAmplitude = 1.0; //max wave amplitude
static const float shoreTransition = 0.1; //how soft the water and ground fades

static const float refractionStrength = 0.1; //refraction strength
static const float displacementStrength = 0.7; //multiplier for the height of waves
static const float shininess = 0.0; //not in use
static const float specularIntensity = 0.32; //not in use

static const float opacity = 0.3;
static const float refractionScale = 0.005;
static const float2 scale = float2(0.0005,0.0005);

static const float4 normalModifier = float4(1.0,2.0,4.0,8.0); //multiplier for the different normals. first one is for small waves.
 
static const float4 foamOptions = float4(0.75, 1.32, 0.5, 1.0); //depth of which foam starts to fade out, depth of which foam is invisible, height of which foam appears for waves.

static const float4 waterSurfaceColor = float4(0.2156,0.4,0.047,1.0);

static const float4 waterDepthColor = float4(0.3921,0.419,0.027,1.0);

static const float4 extinction = float4(28.0, 120.0, 160, 1.0);

float3x3 computeTangentMatrix(float3 normal, float3 position, float2 texCoord)
{
	float3 dp1	= ddx(position);
	float3 dp2	= ddy(position);
	float2 duv1	= ddx(texCoord);
	float2 duv2	= ddy(texCoord);
	
	float3x3 m			= float3x3( dp1, dp2, cross( dp1, dp2 ) );
	float2x3 inverseM	= float2x3( cross( m[1], m[2] ), cross( m[2], m[0] ) );
	float3 tangent		= mul( float2( duv1.x, duv2.x ), inverseM );
	float3 biTangent	= mul( float2( duv1.y, duv2.y ), inverseM );
	
	return transpose(float3x3(normalize(tangent), normalize(biTangent), normal));
}
float fresnelTerm(float3 normal, float3 eyeDir)
{
		float angle		= 1.0f - saturate( dot( normal, eyeDir ) );
		float fresnel	= angle * angle;
		fresnel			= fresnel * fresnel;
		fresnel			= fresnel * angle;
		return saturate( fresnel * .5f + .5f - refractionStrength );
}

struct PS_Input
{
	float4 pos	: SV_POSITION;
	float2 tex	: TEXCOORD0;
};

// the warning: gradient-based operations must be moved out of flow control to prevent divergence. Performance may be improved
// by using non-gradient operation
// is most likely caused be ddx() and ddy(). Everything that uses something that has used something dependent on those functions
// for texture lookup causes that warning. Don't know what to do about it
float4 main(PS_Input input) : SV_TARGET0
{
	float4 originalColor	= texScene.Sample( samLinear, input.tex );

	float depth				= texDepth.Sample( samLinear, input.tex ).r;
	float4 clipPos = float4(input.tex.x * 2.0 - 1.0,
						(1-input.tex.y) * 2.0 - 1.0,
						depth, 1);
	float4 viewPos = mul(mProjInv, clipPos);
	float4 worldPos = mul(mViewInv, float4( (viewPos / viewPos.w).xyz,1) );

	float dist = length(cameraPos.xyz-worldPos.xyz);
	if(dist > 70000.0)
		return originalColor;

	//find the position of the point where the pixel meets the surface of the water
	float wLevel = waterLevel;
	float3 eyeDir = normalize(worldPos - cameraPos).xyz;
	float distanceFromSurface = wLevel - worldPos.y;
	float distanceFromCamera = cameraPos.y - worldPos.y;

	float t = (wLevel - cameraPos.y) / eyeDir.y;
	float3 surfacePos = cameraPos.xyz + eyeDir * t;

	float2 wdir = float2(0,1); //windDirection;

	float2 texCoord;
	float2 adder = gameTime * 0.005f * wdir;
	int temp;
	float bias;
	//calculate the position in regards to waves.
	[unroll]
	for(int i = 0; i < 10; i++)
	{
		texCoord = (surfacePos.xz + eyeDir.xz * 0.1f) * scale + adder;

		bias = heightMap.Sample( samLinear, texCoord ).r;

		bias *= 0.1f;
		wLevel += bias * maxAmplitude;
		t = (wLevel - cameraPos.y) / eyeDir.y;
		surfacePos = cameraPos.xyz + eyeDir * t;
	}

	depth = abs(surfacePos.y - worldPos.y);
	eyeDir = normalize(cameraPos.xyz - surfacePos);

	float normal1 = heightMap.Sample( samLinear, (texCoord + float2(-1,0)*wdir) / 256).r;
	float normal2 = heightMap.Sample( samLinear, (texCoord + float2(1,0)*wdir) / 256).r;
	float normal3 = heightMap.Sample( samLinear, (texCoord + float2(0,-1)*wdir) / 256).r;
	float normal4 = heightMap.Sample( samLinear, (texCoord + float2(0,1)*wdir) / 256).r;

	float3 surfaceNormal = normalize( float3( (normal1 - normal2) *  maxAmplitude ,
											normalScaling,
											(normal3 - normal4) *  maxAmplitude ) );

	//calculate tangent
	texCoord			= (surfacePos.xz * 0.016 + wdir * gameTime * 0.016);
	float3x3 mTangent	= computeTangentMatrix(surfaceNormal, eyeDir, texCoord);
	float3 normal0a		= normalize( mul(mTangent, 2.0f * normalMap.Sample( samLinear, texCoord ).xyz - 1.0 ) );

	texCoord			= (surfacePos.xz * 0.008 + wdir * gameTime * 0.008);
	 mTangent			= computeTangentMatrix(surfaceNormal, eyeDir, texCoord);
	float3 normal1a		= normalize( mul(mTangent, 2.0f * normalMap.Sample( samLinear, texCoord ).xyz - 1.0 ) );

	texCoord			= (surfacePos.xz * 0.004 + wdir * gameTime * 0.004);
	 mTangent			= computeTangentMatrix(surfaceNormal, eyeDir, texCoord);
	float3 normal2a		= normalize( mul(mTangent, 2.0f * normalMap.Sample( samLinear, texCoord ).xyz - 1.0 ) );

	texCoord			= (surfacePos.xz * 0.001 + wdir * gameTime * 0.002);
	 mTangent			= computeTangentMatrix(surfaceNormal, eyeDir, texCoord);
	float3 normal3a		= normalize( mul(mTangent, 2.0f * normalMap.Sample( samLinear, texCoord ).xyz - 1.0 ) );

	float3 normal		= normalize(normal0a * normalModifier.x + normal1a * normalModifier.y + normal2a * normalModifier.z + normal3a * normalModifier.w);

	//calculate refraction
	texCoord = input.tex;
	texCoord.x	+= sin(gameTime * 2.0 + 1.0 * abs(worldPos.y)) * (refractionScale * min(depth, 1.0));

	float3 refraction	= texScene.Sample( samLinear, texCoord ).rgb;
	float depthRefr		= texDepth.Sample( samLinear, texCoord ).r;

	float4 clipPosRefr	= float4(texCoord.x * 2.0 - 1.0,
						(1-texCoord.y) * 2.0 - 1.0,
						depthRefr, 1);
	float4 viewPosRefr	= mul(mProjInv, clipPosRefr);
	float4 worldPosRefr	= mul(mViewInv, float4( (viewPosRefr / clipPosRefr.w).xyz,1) );

	if( worldPosRefr.y < wLevel)
		refraction = originalColor.xyz;

	float3 waterColor = saturate(length(float3(1.0f,1.0f,1.0f)) / 3.0f );
	refraction = lerp(lerp(refraction, waterSurfaceColor.xyz * waterColor, saturate((depth * waterFade) *opacity)),
						waterDepthColor.xyz * waterColor, saturate(depth / extinction.xyz));


	//save work if submerged
	float3 refraction2 = lerp(lerp(texScene.Sample( samLinear, texCoord ).rgb, waterSurfaceColor.xyz * waterColor, 1-saturate((depth * waterFade) )),
						waterDepthColor.xyz * waterColor, 1-saturate(depth / float3(56.0, 240.0, 320)));

	float2 texTest = input.tex;
	texTest += sin(gameTime * 0.002 + 6.0 * abs(worldPos.y)) * (0.5 * min(depth, 1.0));
	if(cameraPos.y < waterLevel) 
	{
		if(worldPos.y > waterLevel + maxAmplitude)	//if this test is not used then this scope should be move to the top
			return saturate( lerp( texScene.Sample( samLinear, texCoord )*float4(waterColor,1), float4( refraction2, 1.0 ) ,1- saturate(depth * shoreTransition) ) );					//but it remains here for now if we want to see the surfice from underneath the water
		else
		{
			float fogFactor = saturate((300 - depth) / (300));
			return texScene.Sample( samLinear2, float2( input.tex.x + 2*sin(gameTime*4.9 + 3.0f * abs(input.tex.y)) * (refractionScale * min(input.tex.y, 1.0f)), input.tex.y ) )
				*waterSurfaceColor * fogFactor + waterDepthColor * (1-fogFactor);
		}
	}
	//if the pixel is above the water surface, then discard.
	if(worldPos.y > waterLevel + maxAmplitude)
		return originalColor;


	//reflection
	// need to draw everything that is the be reflected but with mirrored camera
	// this will take a shit-ton of preformance

	//foam
	float4 foam = float4(0,0,0,0);

	texCoord			= (surfacePos.xz + eyeDir.xz * 0.1) * 0.001 + gameTime * 0.001f * wdir + sin(gameTime * 0.001 + worldPos.x) * 0.00005;
	float2 texCoord2	= (surfacePos.xz + eyeDir.xz * 0.1) * 0.001 + gameTime * 0.002f * wdir + sin(gameTime * 0.001 + worldPos.z) * 0.00005;
	if(depth < foamOptions.x)
		foam = ( foamMap.Sample(samLinear, texCoord) + foamMap.Sample(samLinear, texCoord2) ) * 0.05f;
	else if(depth < foamOptions.y)
	{
		foam = lerp((foamMap.Sample(samLinear,texCoord) + foamMap.Sample(samLinear,texCoord2)) * 0.05f, 0.0f, (depth - foamOptions.x) / (foamOptions.y - foamOptions.x));
	}
	if(maxAmplitude - foamOptions.z > 0.0001f)
	{
		foam += (foamMap.Sample(samLinear, texCoord) + foamMap.Sample(samLinear, texCoord2)) * 0.1f * saturate((wLevel - (waterLevel + foamOptions.z)) / (maxAmplitude - foamOptions.z));
	}


	//specular
	float3 lightVector = -normalize(lightDir);
	float3 dirToCam = normalize(cameraPos.xyz - worldPosRefr.xyz);
	float3 specular = pow(max(dot(normalize(-lightVector + 2 * dot(lightVector, normal) * normal), dirToCam),0.0),40);

	//put everything together
	//return saturate( lerp( originalColor, float4( refraction, 1.0 ) + float4(specular,1) +foam, saturate(depth * shoreTransition) ) ) * (timeofday);

	float4 finalColor;
	float3 ld = float3(0,1,0);
	//finalColor = saturate( lerp( originalColor, float4( refraction, 1.0 ) + float4(specular,1) +foam, saturate(depth * shoreTransition) ) ) * (timeofday);
	finalColor = saturate(lerp(originalColor, float4(refraction, 1.0) + float4(specular, 1) + foam, saturate(depth * shoreTransition))) * (1);
	finalColor.xyz = applyFog(finalColor.xyz, cameraPos.y,(worldPos.xyz - cameraPos.xyz), lightDir);
	//finalColor.w = originalColor.w;
	return finalColor;
}
