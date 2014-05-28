#include "resources.hlsl"

cbuffer cbParticleStart	: register( b0 )
{
	float3 pos;
	int effectType;
	float3 dir;
	float pad;
};

struct VSInput
{
	float3 pos		: POSITION;
	float3 vel		: VELOCITY;
	float timer		: TIMER;
	int type		: TYPE;
	float lifeTime	: LIFETIME;
	float radie		: radie;
};

void launcher(VSInput _input, inout PointStream<VSInput> _pStream )
{
	if(effectType == 1)
	{
		VSInput output;
		_input.timer -= timeStep;
		output.type = 1;
		output.timer = _input.lifeTime;
		output.lifeTime = _input.lifeTime;
		
		for (int i = 0; i < 5; i++)
		{
			output.radie = rand((float)i *10) * 100;

			output.pos = pos;
			output.pos.x += rand((float)i / 256) * 100;
			output.pos.y += rand((float)(i + 100) / 256) * 100;
			output.pos.z += rand((float)(i + 200) / 256) * 100;

			output.vel = dir * 10;
			output.vel.x += rand((float)(i + 50) / 256) * 5;
			output.vel.y += rand((float)(i + 150) / 256) * -50;
			output.vel.z += rand((float)(i + 250) / 256) * 5;

			_pStream.Append(output);
		}
		_input.timer = 3;
	}
	_pStream.Append(_input);

}

void typeOne(VSInput _input, inout PointStream<VSInput> _pStream)
{
	if (_input.timer > 0)
	{
		_input.timer -= timeStep;
		_input.vel.y -= 2 * timeStep;
		_input.pos += _input.vel * timeStep;
		_pStream.Append(_input);
	}
}



[maxvertexcount(100)]
void main(point VSInput _input[1] : SV_POSITION, inout PointStream< VSInput > _pStream)
{
	if (_input[0].type == 0)
	{
		launcher(_input[0], _pStream);
	}
	else
	{
		typeOne(_input[0], _pStream);
	}
}

