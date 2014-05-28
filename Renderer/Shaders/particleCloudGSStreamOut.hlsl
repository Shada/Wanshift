#include "resources.hlsl"

#define			PT_EMITTER (0)
#define			PT_SMOKE (1)

struct VS_Input
{
	float3		pos				: POS;
	float2		size			: SIZE;
	float		rot				: ROT;
	float		age				: AGE;
	uint		type			: TYPE;
};

cbuffer cbSandStorm : register( b5 )
{
	float4x4	sandStormDepthView;
	float3		sandStormPos;
	float		sandAge;
};

[maxvertexcount(100)]
void main(point VS_Input input[1], inout PointStream<VS_Input> ptStream)
{
	//gIn[0].age += timeStep;
	input[0].age += timeStep;
	if(input[0].type == PT_EMITTER)
	{
		if(input[0].age > 1 )
		{
			float		scatter;
			float		scatter2;
			float		scatter3;
			float2		windAlignment;
			float temp;
			float lower;
			for(int i = 1; i < 50; i++)
			{
				temp = (float)i * 0.00390625;
				VS_Input	output;
				scatter			= 30000 * rand2( float2(temp, sandAge)) - 15000;
				lower			= 1-abs(scatter)/15000;
				scatter2		= 800 * rand2( float2(temp, (float)(i+50) / 256)) - 400;
				scatter3		= 2500 * rand2( float2(temp, (float)(i+90) / 256)) - 100;

				windAlignment.x = windDirection.y - input[0].age * 70;//gameTime*70;//* scatter;
				windAlignment.y = -windDirection.x * scatter;
				output.pos.x	= input[0].pos.x + windAlignment.x + windDirection.x * scatter2;
				output.pos.y	= input[0].pos.y + scatter3*lower + sandAge*0.07*lower;
				output.pos.z	= input[0].pos.z + windAlignment.y + windDirection.y * scatter2;
				output.size		= (input[0].size + 1000 + scatter2) + 500*lower;
				output.rot		= scatter2;
				output.age		= .0;
				output.type		= PT_SMOKE;

				ptStream.Append(output);
			}
			input[0].age = 0;
		}
		ptStream.Append(input[0]);
	}
	else
	{
		VS_Input	output;
		input[0].pos.x += timeStep * -windDirection.x * 100; // 10
		input[0].pos.z += timeStep * -windDirection.y * 100; // 10
		input[0].rot += sin(input[0].pos.z) * timeStep *0.1;
		
		if(input[0].age < 100)
			ptStream.Append(input[0]);
	}
}