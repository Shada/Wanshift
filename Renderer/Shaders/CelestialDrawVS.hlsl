
struct VSInput
{
	float3 pos		: POSITION;
	float radie : radie;
	int tex : TEXTURE;
};

VSInput main(VSInput _input)
{
	return _input;
}
