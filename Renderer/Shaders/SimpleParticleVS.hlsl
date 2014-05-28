
struct VSInput
{
	float3 pos		: POSITION;
	float3 vel		: VELOCITY;
	float3 color	: COLOR;
};

VSInput main(VSInput _input)
{
	return _input;
}
