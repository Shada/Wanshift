#include "resources.hlsl"

struct VSInput
{
	float3 pos		: POSITION;
	float3 vel		: VELOCITY;
	float timer		: TIMER;
	int type		: TYPE;
	float lifeTime	: LIFETIME;
	float radie		: radie;
};

struct PS_Input
{
	float4 pos		: SV_POSITION;
	float2 texCoord	: TEXCOORD0;
	float alpha		: ALPHA;
	float4 screenTex: TEXCOORD1;
};

[maxvertexcount(4)]
void main(point VSInput _input[1] : SV_POSITION, inout TriangleStream< PS_Input > _triStream)
{
	if(_input[0].type != 0)
	{
		PS_Input output;

		float radie = 40;

		float4 positions[4];
		positions[0] = float4(-_input[0].radie, _input[0].radie, 0, 1);
		positions[1] = float4(_input[0].radie, _input[0].radie, 0, 1);
		positions[2] = float4(-_input[0].radie, -_input[0].radie, 0, 1);
		positions[3] = float4(_input[0].radie, -_input[0].radie, 0, 1);


		float2 texCoord[4] =
		{
			float2(0.0f, 0.0f ),
			float2(1.0f, 0.0f ),
			float2(0.0f, 1.0f ),
			float2(1.0f, 1.0f )
		};

		float3 look = normalize(cameraPos.xyz - _input[0].pos);
		float3 right = normalize(cross(float3(0, 1, 0), look));
		float3 up = float3(0, 1, 0);

		float4x4 W;
		W[0] = float4(right,	0.0f);
		W[1] = float4(up,		0.0f);
		W[2] = float4(look,		0.0f);
		W[3] = float4(_input[0].pos, 1.0f);

		float4x4 wvp = mul(W,mul(mView,mProj));

		float alpha;
		alpha = 0.1 * timeofday;

		output.alpha = alpha;

		[unroll]
		for(int i=0; i<4; i++)
		{
			float4 position = positions[i];
			position = mul(position, W);
			position = mul(mView, position);
			position = mul(mProj, position);
			output.pos = position;
        
			output.texCoord = texCoord[i];
			output.screenTex = output.pos;
		
			_triStream.Append(output);
		}
		_triStream.RestartStrip();
	}
}