#include "resources.hlsl"
struct VS_Input
{
	float3 vPos		: POSITION;
	float2 vTex		: TEXCOORD0;
	float3 pos		: PERINST0;
	float3 rot		: PERINST1;
	float3 scale	: PERINST2;
};

struct VS_Output
{
	float4 pos	: SV_POSITION;
	float2 tex	: TEXCOORD1;
};
VS_Output main(VS_Input input)
{
	VS_Output output = (VS_Output)0;
	float cosx = cos(input.rot.x);
	float cosy = cos(input.rot.y);
	float cosz = cos(input.rot.z);
	float sinx = sin(input.rot.x);
	float siny = sin(input.rot.y);
	float sinz = sin(input.rot.z);

	float4x4 world =
	{
		input.scale.x*cosy*cosz,								-input.scale.x*cosy*sinz,								input.scale.x*siny,			input.pos.x,
		input.scale.y*cosz*sinx*siny + input.scale.y*cosx*sinz,	input.scale.y*cosx*cosz - input.scale.y*sinx*siny*sinz,	-input.scale.y*cosy*sinx,	input.pos.y,
		input.scale.z*sinx*sinz - input.scale.z*cosx*cosz*siny,	input.scale.z*cosz*sinx + input.scale.z*cosx*siny*sinz,	input.scale.z*cosx*cosy,	input.pos.z,
		0,														0,														0,							1
	};

	output.pos = mul( mProj, mul( mView, mul( world, float4(input.vPos,1) ) ) );
	output.tex = float2(input.vTex.x, 1-input.vTex.y);
	return output;
}