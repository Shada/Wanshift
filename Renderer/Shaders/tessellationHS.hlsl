#include "resources.hlsl"

// Input control point
struct HS_Input
{
	float3 pos	: POSITION;
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD0;
	float2 wTex : TEXCOORD1;
};

//add output control point struct here if necessary

// Output patch constant data.
struct HS_ConstantOutput
{
	float edgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float insideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
};

#define NUM_CONTROL_POINTS 3

// Patch Constant Function
HS_ConstantOutput calcPatchConstants(
	InputPatch<HS_Input, NUM_CONTROL_POINTS> inputPatch,
	uint patchID : SV_PrimitiveID)
{
	HS_ConstantOutput output = (HS_ConstantOutput)0;

	// calculate tessellation factor
	int tessFactor = 5;
	float dist = length(inputPatch[0].pos - cameraPos.xyz);
	if(dist>500 )
		tessFactor = 1;/*
	else if(dist>700)
		tessFactor = 3;
	else if(dist>500)
		tessFactor = 4;
	else if(dist>300)
		tessFactor = 5;
	else if(dist>200)
		tessFactor = 6;
	else if(dist>100)
		tessFactor = 7;*/

	output.edgeTessFactor[0] = 
	output.edgeTessFactor[1] = 
	output.edgeTessFactor[2] = 
	output.insideTessFactor = tessFactor; 

	return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("calcPatchConstants")]
// now input and output struct is the same
HS_Input main(	InputPatch<HS_Input, NUM_CONTROL_POINTS> inputPatch, 
				uint pointID : SV_OutputControlPointID,
				uint patchID : SV_PrimitiveID )
{
	HS_Input output = (HS_Input)0;

	output.pos = inputPatch[pointID].pos;
	output.norm = inputPatch[pointID].norm;
	output.tex = inputPatch[pointID].tex;
	output.wTex = inputPatch[pointID].wTex;

	return output;
}
