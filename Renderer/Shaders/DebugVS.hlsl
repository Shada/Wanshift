struct VS_Input
{
	float3 pos	: POSITION;
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD0;
};

struct GS_Input
{
	float4 pos	:	POS;
};

GS_Input main(VS_Input pos)
{
	GS_Input blarg;

	if (pos.pos.x == 0)
	{
		blarg.pos = float4(-1, -0.5f, 0, 1);

	}
	else
	{
		blarg.pos = float4(-1, -0.5f, 1, 1);

	}
	return blarg;
}