#include "resources.hlsl"

struct VS_Input
{
	float3		pos				: POS;
	float2		size			: SIZE;
	float		rot				: ROT;
	float		age				: AGE;
	uint		type			: TYPE;
};

struct GS_Input
{
	float3		pos				: POS;
	float2		size			: SIZE;
	float		rot				: ROT;
	float		alpha			: ALPHA;
};

GS_Input main( VS_Input input )
{
	GS_Input output;

	output.pos		= input.pos;
	output.size		= input.size;
	output.rot		= input.rot;
	output.alpha	= saturate(input.age*0.1);// - ((uint)2<<(uint)(input.age)) * 0.001;

	return output;
}