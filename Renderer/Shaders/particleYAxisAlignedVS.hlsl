struct VS_Input
{
	float3		pos				: POS;
	float2		size			: SIZE;
	float		alpha			: ALPHA;
};
struct GS_Input
{
	float3		pos				: POS;
	float2		size			: SIZE;
	float		alpha			: ALPHA;
};

GS_Input main( VS_Input input )
{
	GS_Input output;
	output.pos		= input.pos;
	output.size		= input.size;
	output.alpha	= input.alpha;

	return output;
}