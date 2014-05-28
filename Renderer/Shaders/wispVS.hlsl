struct VS_Input
{
	float3		pos				: POS;
	float2		size			: SIZE;
	float		rot				: ROT;
	float		alpha			: ALPHA;
};
struct GS_Input
{
	float3		pos				: POS;
	float2		size			: SIZE;
	float		rot				: ROT;
	float		alpha			: ALPHA;
};

cbuffer cbWisp : register(b5)
{
	float4 hordepos;
	float4 hordecolor;
}

GS_Input main( VS_Input input )
{
	GS_Input output;
	output.pos		= input.pos;
	output.size		= input.size;
	output.rot		= input.rot;
	output.alpha	= input.alpha;

	return output;
}