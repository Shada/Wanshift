#include "resources.hlsl"

struct VSInput
{
	float3 pos		: POSITION;
	float3 vel		: VELOCITY;
	float3 color	: COLOR;
};

[maxvertexcount(4)]
void main(point VSInput _input[1] : SV_POSITION, inout PointStream< VSInput > _pStream)
{
	VSInput output = _input[0];

	if(output.pos.y < 0)
	{
		output.pos.y = 200;
		_pStream.Append(output);
	}

	output.pos.y -= 10;

	_pStream.Append(output);
}