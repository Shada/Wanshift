
struct VS_Input
{
	float2	topleft		: ANCHOR;
	float2	dimensions	: DIMENSIONS;
	float	opacity		: OPACITY;
};

VS_Input main(VS_Input input)
{
	return input;
}