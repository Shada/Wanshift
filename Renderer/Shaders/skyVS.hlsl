#include "resources.hlsl"

struct VS_INPUT
{
	float3 position	: POSITION;
};

struct PS_INPUT
{
	float4 position		: SV_Position;
	float3 direction	: POSITION0;
};

matrix rotateX(float radian)
{
	matrix m = {	1,	0,				0,				0,
					0,	cos(radian),	-sin(radian),	0,	
					0,	sin(radian),	cos(radian),	0,
					0,	0,				0,				1
				};

	return m;
}

matrix rotateZ(float radian)
{
	matrix m =	{	cos(radian),	-sin(radian),	0,	0,
					sin(radian),	cos(radian),	0,	0,
					0,				0,				1,	0,
					0,				0,				0,	1
				};

	return m;
}

matrix rotateY(float radian)
{
	matrix m =	{	cos(radian),	0,	sin(radian),	0,
					0,				1,	0,				0,
					-sin(radian),	0,	cos(radian),	0,
					0,				0,	0,				1
				};

	return m;
}

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT ps;
	
	float3 p = input.position;
	p.x += cameraPos.x;
	p.z += cameraPos.z;
	ps.direction = input.position;

	//ps.position = float4( mul(mProj, mul(mView, mul( mWorld,  float4(p,1) ) ) ) );
	ps.position = float4( mul(mProj, mul(mView,  float4(p,1) ) ) );
	return ps;
}