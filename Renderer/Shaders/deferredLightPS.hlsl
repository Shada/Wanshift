#include "resources.hlsl"

Texture2D texDepth			: register(t1);
Texture2D texColor			: register(t2);
Texture2D texNormal			: register(t3);
Texture2D texSpecular		: register(t4);
Texture2D texGlowMask		: register(t5);
Texture2D texGlow			: register(t6);
Texture2D shadowMap			: register(t7);


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
cbuffer cbSpotLight		:register(b6)
{
	int spotlightCount;
	float3 reach_cosTheta_cosPhi;  //will have to be restructured if different sources should have these different
	float4 spotLightPos[maxSpotLightCount];
	float4 spotLightColor[maxSpotLightCount];
	float4 spotLightDir[maxSpotLightCount];
}
struct PS_Input
{
	float4 pos	: SV_POSITION;
	float2 tex	: TEXCOORD0;
};

// draws a lit scene to a full screen quad
float4 main(PS_Input input) : SV_TARGET0
{
	float depth = texDepth.Sample(samPoint, input.tex).r;

	//the color of the pixel, before it is lit.
	float3 color = texColor.Sample(samPoint, input.tex).xyz;

	float4 glowMask = texGlowMask.Sample(samLinear2, input.tex);
	float3 glowColor = texGlow.Sample(samLinear2, input.tex).xyz;

	if(depth == 1)
	{
		return float4(saturate(color + glowColor), 1);
	}

	float4 normal = texNormal.Sample(samPoint, input.tex);

	float4 specular = texSpecular.Sample(samPoint, input.tex);

	// was saved in values between 0-1 in the shader resource, now
	// it needs to be converted to the range -1 - 1
	normal.xyz = (normal.xyz - 0.5) * 2;

	// so these values should be unique for every material and not hardcoded
	float ka = specular.w;
	float3 kd = float3(1, 1, 1);//float3(0.9280,0.9280,0.9280);
	float3 outColor = float3(0, 0, 0);

	//-------------------------------------------------------------------------
	// add code for the shadow
	bool inLight = true;

	//inLight = Shadow(input);

	float4 currentPosition;

	// this pixels position in clip space
	currentPosition.x = input.tex.x * 2.0f - 1.0f;
	currentPosition.y = -(input.tex.y * 2.0f - 1.0f);
	currentPosition.z = depth;
	currentPosition.w = 1.0f;

	// Transform the screen space position in to world-space
	currentPosition = mul(mProjInv, currentPosition);
	currentPosition = mul(mViewInv, currentPosition);
	currentPosition /= currentPosition.w;

	float3 lightVector = -normalize(lightDir);
	float NdotL = max(0, dot(normal.xyz, lightVector));
	float3 diffuseLight = NdotL * color;
	
	float3 reflectionVector = normalize(reflect(lightVector, normal.xyz));
	float3 dirToCam = normalize(cameraPos.xyz - currentPosition.xyz);
	float3 specLight = specular.xyz * color.xyz * lightColor[0].xyz * pow(max(dot(normalize(-lightVector + 2 * dot(lightVector, normal.xyz) * normal.xyz), dirToCam), 0.0), 40);
	outColor = diffuseLight + specLight + ka * color;

	
	//return float4(outColor,1);
	float3 eyePixel = currentPosition.xyz - cameraPos.xyz;
	float  eyePosY = cameraPos.y;
	float3 sDir = lightDir;

	outColor = applyFog(outColor, eyePosY, eyePixel, sDir);
	//return float4(outColor,1);

	float4 shadowAtt = shadowMap.Sample(samPoint, input.tex);

	float minLight = 0.3f;

	shadowAtt.x = max(minLight, shadowAtt.x);

	float sadowLow = 0.3;
	float shadowHigh = 0.5;

	if (timeofday < shadowHigh && timeofday > sadowLow)
	{
		if (shadowAtt.x < 1)
		{
			float fade = (1 - ((timeofday - sadowLow) * 5));

			shadowAtt.x = max(fade, shadowAtt.x);

			outColor *= shadowAtt.x;
		}
	}
	else if (timeofday < sadowLow)
	{

	}
	else
	{
		outColor *= shadowAtt.x;
	}

	outColor += glowColor;

	// point light
	// add every light to the pixel
	float lightDist;
	for(int i = 0; i < lightCount; i++)
	{
		//float3 lightPosWV = mul(mView, lightPos[i]).xyz;
		lightVector = currentPosition.xyz - lightPos[i].xyz;
		// the distance between the pixel and the lights position
		lightDist = length(lightVector);
		// if the pixel is within reach of this light source
		// start apply lighting
		if(lightDist < lightReach[i].x)
		{
			float3 s = -normalize(lightVector);
			float lightFade = (lightReach[i].x - lightDist) / lightReach[i].x;

			outColor += (kd * color * lightColor[i].rgb * abs(dot(s, normal.xyz))
						 + specular.rgb * color * lightColor[i].rgb * pow(max(dot(normalize(-s + 2 * dot(s, normal.xyz) * normal.xyz),
						 normalize(currentPosition.xyz - cameraPos.xyz)), 0.0), 40))
						 *	lightFade;
		}
	}

	[unroll]
	for(i = 0; i < spotlightCount; i++)
	{
		lightVector = spotLightPos[i].xyz - currentPosition.xyz;
		lightDist = length(lightVector);
		float lightReach = reach_cosTheta_cosPhi.x;
		if(lightDist < lightReach)
		{
			float3 s = normalize(lightVector);
			float attenuation = 0.0;
			//cameraDirWV is in this case the direction of the light
			float cosAlpha = max(dot(-s, normalize(spotLightDir[i].xyz)), 0.0);
			if(cosAlpha > reach_cosTheta_cosPhi.y)
				attenuation = 1.0;
			else if( cosAlpha > reach_cosTheta_cosPhi.z )
				attenuation = pow( abs((cosAlpha - reach_cosTheta_cosPhi.z) / (reach_cosTheta_cosPhi.y - reach_cosTheta_cosPhi.z )),20.0 ); 
			float lightFade = (lightReach - lightDist) / lightReach;
			outColor += (kd * color * float3(1.0, 1.0, 1.0) * max(dot(s, normal.xyz), 0.0)
						 + specular.rgb * color * float3(1.0, 1.0, 1.0) * pow(max(dot(normalize(-s + 2 * dot(s, normal.xyz) * normal.xyz),
						 normalize(cameraPos.xyz - currentPosition.xyz)), 0.0), 40))
						 * attenuation * lightFade;
		}
	}

	return float4(outColor, 1);
}
