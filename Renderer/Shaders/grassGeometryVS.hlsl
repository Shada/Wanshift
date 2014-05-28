struct VS_Input
{
	float3 pos : POSITION;
	float3 posInst	: PERINST;
};
struct VS_Output
{
	float3 pos : POSITION;
};

VS_Output main(VS_Input input)
{
	VS_Output output = (VS_Output)0;
	output.pos = input.pos + input.posInst;
	return output;
}