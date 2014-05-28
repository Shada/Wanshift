
struct PS_Input
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR;
};

float4 main(PS_Input _input) : SV_TARGET0
{
	float4 output;
	output = _input.color;

	return output;
}