#include "resources.hlsl"

cbuffer cbLightInfo		: register(b4)
{
	int		lightCount;
	float3  lightDir;
	float4	lightPos[maxLightCount];
	float4	lightColor[maxLightCount];
	float4	lightReach[maxLightCount];
	matrix  lightView;
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

struct PS_Input
{
	float4 pos	: SV_POSITION;
	float2 tex	: TEXCOORD0;

};

Texture2D depth : register(t1);
Texture2D color : register(t2);

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


float4 main(PS_Input input) : SV_TARGET
{
	float dep = depth.Sample(samLinear2, input.tex).r;
	float4 c = color.Sample(samLinear2, input.tex);
	return c;
	if(dep >= 1)
		return c;

	// this pixels position in clip space
	float4 clipPos = float4(input.tex.x * 2.0 - 1.0,
							(1 - input.tex.y) * 2.0 - 1.0,
							dep,
							1);

	// transform it into view space
	float4 position = mul(mProjInv, clipPos);
	position = position / position.w;
	position.w = 1.0;

	float4 cPos = cameraPos;
		cPos.x = 0;
	cPos.z = 0;
	cPos.y *= innerRadius;

	float cameraHeight = innerRadius + cameraPos.y;
	float3 direction = position.xyz;
	float3 pos = clipPos.xyz;
	pos.y += innerRadius;

	float3 ray = pos;
	float far = length(ray);
	ray /= far;

	// ray start origin
	float3 start = normalize(lightPos[0].xyz);
		start = normalize(start);
	start.y = 10;

	float height = length(start);
	float depth = exp(-1.0f / scaleDepth);//
	float startAngle = dot(ray, start) / height;
	float startOffset = depth * getScale(startAngle);

	// loop variables
	float scale = 1.0f / (outerRadius - innerRadius);
	float samples = 16;//4.0;
	float sampleLength = far / samples;
	float scaledLength = sampleLength * scale;
	float3 sampleRay = ray * sampleLength;
	float3 samplePoint = start + sampleRay * 0.5f;

	float3 color;
	float4 waveLength = float4(wR, 1.0f);
	float4 lp = lightPos[0];

		// loop
	for(uint i = 0; i < nSamples; ++i)
	{
		float d = 2.5;
		float lA = dot(normalize(lp.xyz), samplePoint) / length(samplePoint);
		float cA = dot(ray, samplePoint) / length(samplePoint);

		float camScale = getScale(cA);
		float lightScale = getScale(lA);
		float camOffset = depth * camScale;
		float tOffset = lightScale + camScale;

		float scatter = d * tOffset - camOffset;
		float3 attenuate = exp(-scatter * (waveLength * kr4PI + km4PI)).xxx;

		attenuate *= (depth * scaledLength);
		color += attenuate;
		samplePoint += sampleRay;
	}

	float4 rayleigh, mie, finalColor;
	rayleigh.xyz = color * wR * krEsun;
	rayleigh.w = 1.0f;
	mie.xyz = color * wM * kmEsun;
	mie.w = 1.0f;

	float cos = dot(normalize(-lp.xyz), direction) / length(direction);
	float rf = rayleighPhase(cos);
	float mf = miePhase(cos, gM);

	finalColor = 1.0f - exp(-exposure * (rf * rayleigh + mf * mie));

	float4 cam = mul(mView, cameraPos);
		float len = length(position);

	finalColor = HDR(finalColor);

	float cutOff = 30000.0f;
	if(len > cutOff)
		finalColor = lerp(c, finalColor, 0.75);
	else
		finalColor = lerp(c, lerp(c, finalColor, smoothstep(0.0, cutOff, len)), 0.75);

	finalColor.w = 1;

	return finalColor;
}