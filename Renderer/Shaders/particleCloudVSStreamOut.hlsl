#include "resources.hlsl"

struct VS_Input
{
	float3		pos				: POS;
	float2		size			: SIZE;
	float		rot				: ROT;
	float		age				: AGE;
	uint		type			: TYPE;
};

VS_Input main( VS_Input input )
{
	return input;
}