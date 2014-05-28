#include "resources.hlsl"

cbuffer cbCloud : register(b10)
{
	float2	offset;
	uint	cloudSize;
	float	cellSize;
	uint	intervalStart;
	uint	intervalEnd;
	float2	cloudPad;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float  size		: SIZE0;
	uint   id		: ID0;
};

//texture3D tCloud : register(t9);
Texture2DArray tCloud : register(t9);

float rest(float f, float s) {return f - int(f/s)*s; }
float wrap(uint x) {return x % 24; }

float4 main( PS_INPUT input ) : SV_TARGET
{
	//return float4(1,0,0,1);
	float4 fCloud;
	float speed = 20.f;
	float numberOfFrames = 24.f;
	float offset = input.id;
	float dt = gameTime + offset;
	float4 fc1 = tCloud.Sample( samLinear, float3( input.texCoord, rest( dt, speed * numberOfFrames ) / speed ) );
	float4 fc2 = tCloud.Sample( samLinear, float3( input.texCoord, wrap( rest( dt, speed * numberOfFrames ) / speed + 1.f ) ) );

//	fCloud = lerp(fc1, fc2, smoothstep(fc1, fc2, fc1 + (fc2 - fc1) * (rest( gameTime, 5 ) / 5) ) );
	fCloud = lerp(fc1, fc2, rest( dt, speed ) / (speed/2) );
//	fCloud = lerp(tCloud.Sample( samLinear, float3( input.texCoord, 0.f ) ), tCloud.Sample( samLinear, float3( input.texCoord, 0.5f ) ), 1.f );
	//	fCloud.w = fc1.w;
	//float4 fCloud = tCloud.Sample( samLinear, float3( input.texCoord, timeStep * 1000) );
	//float4 fCloud = tCloud.Sample( samLinear, input.texCoord );
	//fCloud = float4(1,0,0,1);
	float mod = 24;//intervalEnd - intervalStart;
	float start = (float)intervalStart / mod;
	float end	= (float)intervalEnd / mod;
	float loop	= ((gameTime) % mod) / mod;
	float oZ	= lerp( start, end, loop );

	//fc1 = tCloud.Sample( samLinear, float3( input.texCoord, start ) );
	//fc2 = tCloud.Sample( samLinear, float3( input.texCoord, oZ ) );

	//fCloud = lerp(fc1,fc2, fc2.w);//smoothstep(0, 1, fc2.w ) );

	return fCloud;
}