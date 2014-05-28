#include "resources.hlsl"
struct VS_Input
{
	float3 vPos		: POSITION;
	float3 vNorm	: NORMAL;
	float2 vTex		: TEXCOORD0;
	float3 pos		: PERINST0;
	float3 rot		: PERINST1;
	float3 scale	: PERINST2;
};

struct VS_Output
{
	float4 pos	: SV_POSITION;
	float3 norm	: NORMAL;
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
	float scaleBackX = 1/input.scale.x;
	float scaleBackY = 1/input.scale.y;
	float scaleBackZ = 1/input.scale.z;

	float3x3 normalWorld =
	{
		scaleBackX*cosy*cosz,								-scaleBackY*cosy*sinz,								scaleBackZ*siny,
		scaleBackX*cosz*sinx*siny + scaleBackX*cosx*sinz,	scaleBackY*cosx*cosz - scaleBackY*sinx*siny*sinz,	-scaleBackZ*cosy*sinx,
		scaleBackX*sinx*sinz - scaleBackX*cosx*cosz*siny,	scaleBackY*cosz*sinx + scaleBackY*cosx*siny*sinz,	scaleBackZ*cosx*cosy
	};

	output.pos = mul( mProj, mul( mView, mul( world, float4(input.vPos,1) ) ) );
	output.norm = normalize(mul(normalWorld, input.vNorm));
	output.tex = float2(input.vTex.x, 1-input.vTex.y);
	return output;
}