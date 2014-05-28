#include "resources.hlsl"

cbuffer cbLightInfo		: register(b4)
{
	int		lightCount;
	float3  lightDir;
	float4	lightPos[maxLightCount];
	float4	lightColor[maxLightCount];
	float4	lightReach[maxLightCount];
	matrix  lightView;
	matrix  lightProj;
	matrix	toShadowSpace;
	matrix	CascadeViewProj[4];
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

struct GS_OUTPUT
{
	float4 Pos		: SV_Position;
	uint RTIndex	: SV_RenderTargetArrayIndex;
};

[maxvertexcount(3)]
void main(triangle GS_Input input[3], inout TriangleStream< GS_OUTPUT > OutStream)
{
	GS_OUTPUT output = (GS_OUTPUT)0;

	output.Pos = float4(0,0,0,0);
	output.RTIndex = 0;

	OutStream.Append(output);

	//float cosx = cos(input[0].rot.x);
	//float cosy = cos(input[0].rot.y);
	//float cosz = cos(input[0].rot.z);
	//float sinx = sin(input[0].rot.x);
	//float siny = sin(input[0].rot.y);
	//float sinz = sin(input[0].rot.z);

	//float4x4 world1 = {
	//			input[0].scale.x*cosy*cosz,										-input[0].scale.x*cosy*sinz,									input[0].scale.x*siny,			input[0].pos.x,
	//			input[0].scale.y*cosz*sinx*siny + input[0].scale.y*cosx*sinz,	input[0].scale.y*cosx*cosz - input[0].scale.y*sinx*siny*sinz,	-input[0].scale.y*cosy*sinx,	input[0].pos.y,
	//			input[0].scale.z*sinx*sinz - input[0].scale.z*cosx*cosz*siny,	input[0].scale.z*cosz*sinx + input[0].scale.z*cosx*siny*sinz,	input[0].scale.z*cosx*cosy,		input[0].pos.z,
	//			0,																0,																0,								1
	//		};

	//cosx = cos(input[1].rot.x);
	//cosy = cos(input[1].rot.y);
	//cosz = cos(input[1].rot.z);
	//sinx = sin(input[1].rot.x);
	//siny = sin(input[1].rot.y);
	//sinz = sin(input[1].rot.z);

	//float4x4 world2 = {
	//	input[i].scale.x*cosy*cosz, -input[i].scale.x*cosy*sinz, input[i].scale.x*siny, input[i].pos.x,
	//	input[i].scale.y*cosz*sinx*siny + input[i].scale.y*cosx*sinz, input[i].scale.y*cosx*cosz - input[i].scale.y*sinx*siny*sinz, -input[i].scale.y*cosy*sinx, input[i].pos.y,
	//	input[i].scale.z*sinx*sinz - input[i].scale.z*cosx*cosz*siny, input[i].scale.z*cosz*sinx + input[i].scale.z*cosx*siny*sinz, input[i].scale.z*cosx*cosy, input[i].pos.z,
	//	0, 0, 0, 1
	//};

	//cosx = cos(input[2].rot.x);
	//cosy = cos(input[2].rot.y);
	//cosz = cos(input[2].rot.z);
	//sinx = sin(input[2].rot.x);
	//siny = sin(input[2].rot.y);
	//sinz = sin(input[2].rot.z);

	//float4x4 world2 = {
	//	input[i].scale.x*cosy*cosz, -input[i].scale.x*cosy*sinz, input[i].scale.x*siny, input[i].pos.x,
	//	input[i].scale.y*cosz*sinx*siny + input[i].scale.y*cosx*sinz, input[i].scale.y*cosx*cosz - input[i].scale.y*sinx*siny*sinz, -input[i].scale.y*cosy*sinx, input[i].pos.y,
	//	input[i].scale.z*sinx*sinz - input[i].scale.z*cosx*cosz*siny, input[i].scale.z*cosz*sinx + input[i].scale.z*cosx*siny*sinz, input[i].scale.z*cosx*cosy, input[i].pos.z,
	//	0, 0, 0, 1
	//};

	



	//float4x4 world[3];

	//for (int i = 0; i < 3; i++)
	//{


	//	float cosx = cos(input[i].rot.x);
	//	float cosy = cos(input[i].rot.y);
	//	float cosz = cos(input[i].rot.z);
	//	float sinx = sin(input[i].rot.x);
	//	float siny = sin(input[i].rot.y);
	//	float sinz = sin(input[i].rot.z);

	///*	float4x4 temp = {
	//		input[i].scale.x*cosy*cosz,										-input[i].scale.x*cosy*sinz,									input[i].scale.x*siny,			input[i].pos.x,
	//		input[i].scale.y*cosz*sinx*siny + input[i].scale.y*cosx*sinz,	input[i].scale.y*cosx*cosz - input[i].scale.y*sinx*siny*sinz,	-input[i].scale.y*cosy*sinx,	input[i].pos.y,
	//		input[i].scale.z*sinx*sinz - input[i].scale.z*cosx*cosz*siny,	input[i].scale.z*cosz*sinx + input[i].scale.z*cosx*siny*sinz,	input[i].scale.z*cosx*cosy,		input[i].pos.z,
	//		0,																0,																0,								1
	//	};*/

	//	world[i][0] = float4(input[i].scale.x*cosy*cosz, -input[i].scale.x*cosy*sinz, input[i].scale.x*siny, input[i].pos.x);
	//	world[i][1] = float4(input[i].scale.y*cosz*sinx*siny + input[i].scale.y*cosx*sinz, input[i].scale.y*cosx*cosz - input[i].scale.y*sinx*siny*sinz, -input[i].scale.y*cosy*sinx, input[i].pos.y);
	//	world[i][2] = float4(input[i].scale.z*sinx*sinz - input[i].scale.z*cosx*cosz*siny, input[i].scale.z*cosz*sinx + input[i].scale.z*cosx*siny*sinz, input[i].scale.z*cosx*cosy, input[i].pos.z);
	//	world[i][3] = float4(0, 0, 0, 1);
	//}

	//for (int iFace = 0; iFace < 4; iFace++)
	//{
	//	output.RTIndex = iFace;

	//	for (int v = 0; v < 3; v++)
	//	{
	//		
	//		output.Pos = mul(world[v], float4(input[v].pos,1));
	//		output.Pos = mul(CascadeViewProj[iFace], output.Pos);
	//		OutStream.Append(output);

	//	}

	//	OutStream.RestartStrip();

	//}
}