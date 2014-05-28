
struct VSInput
{
	float3 pos		: POSITION;
	float timer : TIMER;
	float radie : radie;
	int to : TO;
	int type : TYPE;
};

VSInput main(VSInput _input)
{
	return _input;
}
