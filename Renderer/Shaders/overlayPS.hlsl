struct PS_INPUT
{
	float4	p	: SV_POSITION;
	float	o	: OPACITY;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	//return float4(1,0,0,1);

	float4 color = float4(0,0,0, input.o);
	return color;
}