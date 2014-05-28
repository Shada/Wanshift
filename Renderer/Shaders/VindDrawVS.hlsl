
struct VSInput
{
	float3 pos		: POSITION;
	float3 vel		: VELOCITY;
	float timer		: TIMER;
	int type		: TYPE;
	float lifeTime	: LIFETIME;
	float radie		: radie;
};

VSInput main(VSInput _input)
{
	return _input;
}
