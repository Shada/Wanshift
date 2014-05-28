
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

VSInput main(VSInput _input)
{
	return _input;
}
