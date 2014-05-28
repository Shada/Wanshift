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

cbuffer WorldAttributes	: register( b9 )
{
	/*	Terrain	*/
	float	innerRadius;	// 4
	float	outerRadius;	// 4
	float	reflectance;	// 4
	/*	Sun	*/
	float	brightness;		// 4
};	


cbuffer ScatterAttributes : register( b10 )
{
	float	kr4PI;		// 4
	float	km4PI;		// 4
	float	krEsun;		// 4
	float	kmEsun;		// 4

	float	gM;			// 4
	float3	wM;			// 12	

	float	exposure;	// 4
	float3	wR;			// 12	
};	

struct PS_INPUT
{
	float4 position		: SV_Position;
	float3 direction	: POSITION0;
};

float miePhase(float _cos, float _g)
{
	float a = 1.5f;
	float b = 1.0f;
	float c = 2.0f;
	return a * ((b - _g * _g) / (c + _g * _g)) * 
			(b + _cos * _cos) / 
			pow(abs(b + _g * _g - c * _g *  _cos), a);
}

float rayleighPhase(float _cos)
{
	float a = 0.75;
	float b = 2.0;
	float c = 0.5;
	return a * (b + c * _cos * _cos);
	//return 0.75f * (1.0 + _cos * _cos);
}

float getScale(float _cos)
{
	float x = 1.0f - _cos;
	return scaleDepth * exp( -0.00287f + x * ( 0.459f + x * ( 3.83f + x * (-6.80f + x * 5.25f) ) ) );
}

float4 HDR(float4 _color)
{
	return pow(abs(_color), 1.0f / 2.2f);
	//return 1.0f - exp( -2.0f * _color);
}

float4 main(PS_INPUT input) : SV_TARGET
{
	//return float4(1,0,0,1);
	float cameraHeight = innerRadius + cameraPos.y;

	float3 pos = input.direction / outerRadius;
	pos.y += innerRadius;
	float3 ray = pos - cameraPos.xyz;

	float far = length(ray);
	ray /= far;

	// ray start origin
	float3 start = cameraPos.xyz;

	start = normalize(start);
	//start.y = 1000;

	float3 cPos			= cameraPos.xyz + float3(0, innerRadius, 0);
	float height		= length(start);
	float depth			= exp(-1.0f / scaleDepth);//
	float startAngle	= dot(ray,start) / height;
	float startOffset	= depth * getScale(startAngle);

	// loop variables
	float scale			= 1.0f / (outerRadius - innerRadius);
	float samples		= nSamples;//4.0;
	float sampleLength	= far / samples;
	float scaledLength	= sampleLength * scale;
	float3 sampleRay = ray * sampleLength;
	float3 samplePoint = start + sampleRay * 0.5f;

	float3 color = float3(0, 0, 0);
	float4 waveLength = float4(wR, 1.0f);

	float3 lp = -lightDir;
	//lp.y += 100;

	// loop
	for(uint i = 0; i < nSamples; ++i)
	{
		float d	= exp(scaleOverDepth * (innerRadius - length(samplePoint)));
		//float d	= 2.5;//magic constant //exp(scaleOverDepth * (innerRadius - length(samplePoint)));
		float lA= dot(normalize(lp),samplePoint) / length(samplePoint);
		float cA= dot(ray, samplePoint) / length(samplePoint);

		float camScale	= getScale(cA);
		float lightScale= getScale(lA);
		float camOffset = 1.0;// depth * camScale;
		float tOffset	= lightScale + camScale;

		float scatter	= d * tOffset - camOffset;
		float3 attenuate= exp(-scatter * (waveLength.xyz * kr4PI + km4PI));

		attenuate	*= (depth * scaledLength);
		color += attenuate;
		samplePoint += sampleRay;
	}

	float intensity = timeofday >= 0.25 ? timeofday : smoothstep(0,1,timeofday);
	color *= intensity;

	float4 rayleigh, mie, finalColor;
	rayleigh.xyz= color * wR * krEsun;
	rayleigh.w	= 1.0f;
	mie.xyz		= color * wM * kmEsun;
	mie.w		= 1.0f;

	float cos	= dot(normalize(-lp), input.direction) / length(input.direction);
	float rf	= rayleighPhase(cos);
	float mf	= miePhase(cos, gM);

	finalColor	= 1.0f - exp(-exposure * (rf * rayleigh + mf * mie));

	float3 eyePixel = input.direction - cameraPos.xyz;
	float  eyePosY	= cameraPos.y;
	float3 sDir		= lightDir;
	
	finalColor.xyz = applyFog(finalColor.xyz, eyePosY, eyePixel, sDir);

	finalColor.w = intensity + 0.25f * timeofday;// +smoothstep(0, 0.25, intensity);
	finalColor	= HDR(finalColor);

	

	return finalColor;
}