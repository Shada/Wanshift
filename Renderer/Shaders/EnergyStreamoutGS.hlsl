#include "resources.hlsl"

cbuffer cbParticleEnergy	: register( b0 )
{
	float4 pos[20];
	float4 goal[20];
	float time;
	float lifeTime;
	float radie;
	float pad;
};

struct VSInput
{
	float3 pos		: POSITION;
	int index : INDEX;
	float3 goal		: GOAL;
	float timer : TIMER;
	float lifeTime : LIFETIME;
	float radie : radie;
	int type : TYPE;
	int pad : PAD;
};

void launcher(VSInput _input, inout PointStream<VSInput> _pStream )
{
	if (_input.lifeTime > time)
	{
		VSInput output;
		output.timer = time;
		output.lifeTime = 0;
		output.radie = radie;
		output.type = 1;
		output.pad = 0;

		for (int i = 0; i < 20; i++)
		{
			if (pos[i].x != 0)
			{
				output.pos = pos[i].xyz;
				output.goal = pos[i].xyz;

				output.goal.x += (rand((float)i / 256) * 24 - 12);
				output.goal.y += (rand((float)(i + 100) / 256) * 24 - 12);
				output.goal.z += (rand((float)(i + 200) / 256) * 24 - 12);
				output.index = i;

				_pStream.Append(output);
			}
		}
		_input.lifeTime = 0;
	}

	_input.lifeTime += 1;
	_pStream.Append(_input);

}

void typeOne(VSInput _input, inout PointStream<VSInput> _pStream)
{
	if (_input.lifeTime < _input.timer)
	{
		_input.lifeTime += timeStep;
		if (goal[_input.index].x == 0)
		{
			_input.pos += (_input.goal - _input.pos) * timeStep;
		}
		else
		{
			_input.pos += (goal[_input.index].xyz - _input.pos) * timeStep +(((_input.goal - _input.pos) * timeStep) * ((_input.timer - _input.lifeTime) / 2));
		}
		_pStream.Append(_input);
	}
}



[maxvertexcount(80)]
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

