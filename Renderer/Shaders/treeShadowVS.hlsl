struct VS_Input
{
	float3 vPos		: POSITION;
	float3 vNorm	: NORMAL;
	float2 vTex		: TEXCOORD0;
	float3 pos		: PERINST0;
	float3 rot		: PERINST1;
	float3 scale	: PERINST2;
};

struct GS_Input
{
	float4 vPos		: SV_POSITION;
	float3 vNorm	: NORMAL;
	float2 vTex		: TEXCOORD0;
	float3 pos		: PERINST0;
	float3 rot		: PERINST1;
	float3 scale	: PERINST2;
};

GS_Input main(VS_Input input)
{
	GS_Input output;

	output.vPos = float4(input.vPos,1);
	output.vNorm = input.vNorm;
	output.vTex = input.vTex;
	output.pos = input.pos;
	output.rot = input.rot;
	output.scale = input.scale;

	return output;
}