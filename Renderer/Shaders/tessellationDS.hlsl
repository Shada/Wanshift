#include "resources.hlsl"

Texture2D displacementMap	: register(t1);
Texture2D blendMap1			: register(t2);

struct DS_Output
{
	float4 pos  : SV_POSITION;
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD0;
	float2 wTex : TEXCOORD1;
};

struct HS_Output
{
	float3 pos	: POSITION;
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD0;
	float2 wTex : TEXCOORD1;
};

// Output patch constant data.
struct HS_ConstantOutput
{
	float edgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float insideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_Output main( HS_ConstantOutput input,
				float3 domain : SV_DomainLocation,
				const OutputPatch<HS_Output, NUM_CONTROL_POINTS> patch)
{
	DS_Output output = (DS_Output)0;

	//determine the position, normal and texture coordinates of the new vertex
	float3 vertexPos	= patch[0].pos*domain.x + patch[1].pos*domain.y + patch[2].pos*domain.z;
	float3 vertexNorm	= normalize(patch[0].norm*domain.x + patch[1].norm*domain.y + patch[2].norm*domain.z);
	float2 vertexTex	= patch[0].tex*domain.x + patch[1].tex*domain.y + patch[2].tex*domain.z;

	// the displacement is found in the alphachannel and multiplied by some factor (5)
	// this factor is then changed depending on how close the vertex is to the camera
	// this to create a seamless transition between tessellating and not tessellating
	float dist = length(vertexPos - cameraPos.xyz);
	float height = displacementMap.SampleLevel(samLinear, vertexTex, 0).w * (500 - dist) / 500;

	float2 val1 = (patch[0].wTex) / (45900 * 3);
	//float val2 = (patch[0].wTex.x + 720) / (45900 * 3);
	//float val3 = (patch[0].wTex.y + 720) / (45900 * 3);
	//float val4 = (patch[0].wTex + 720) / (45900 * 3);

	float blendVal1 = blendMap1.SampleLevel(samLinear, val1, 0).x;
	//float blendVal2 = blendMap1.SampleLevel(samLinear, val2, 0).x;
	//float blendVal3 = blendMap1.SampleLevel(samLinear, val3, 0).x;
	//float blendVal4 = blendMap1.SampleLevel(samLinear, val4, 0).x;
	//
	//float blend1 = lerp(blendVal1, blendVal3, blendVal1 - floor(blendVal1));
	//float blend2 = lerp(blendVal2, blendVal4, blendVal2 - floor(blendVal2));
	//float blendVal = lerp(blend1, blend2, blend1 - floor(blend1));

	height *= blendVal1 * blendVal1;

	// the position is displaced along the normal
	if(dist<450)
		vertexPos += vertexNorm * height;

	// get the position into projection space
	output.pos = mul(mWorld, float4(vertexPos,1));
	output.pos = mul(mView, output.pos);
	output.pos = mul(mProj, output.pos);
	
	//send the normal to the pixel shader for normalmapping etc
	output.norm = vertexNorm;

	output.tex = vertexTex;
	output.wTex = patch[0].wTex;
	return output;
}
