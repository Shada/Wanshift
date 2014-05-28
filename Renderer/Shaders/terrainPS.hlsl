#include "resources.hlsl"

//texture maps
Texture2D tx1				: register(t1);
Texture2D tx2				: register(t2);
Texture2D tx3				: register(t3);
Texture2D tx4				: register(t4);
Texture2D tx5				: register(t5);
Texture2D map_ks			: register(t6);				//specular map
Texture2D map_normal		: register(t7);				//normal map / bump / disp
Texture2D map_blend1		: register(t8);				//blend map
Texture2D map_blend2		: register(t9);				//blend map
Texture2D tx22				: register(t10);

struct PS_Input
{
	float4 pos	: SV_POSITION;	
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD0;
	float2 wTex : TEXCOORD1;
	float3 realPos: REALPOS;
};

PS_MRTOutput main(PS_Input input)
{
	PS_MRTOutput output = (PS_MRTOutput)0;					//create an empty output

	float4 blendTexCol1 = map_blend1.Sample(samLinear, input.wTex / (45900 * 3));
	float4 blendTexCol2 = map_blend2.Sample(samLinear, input.wTex / (45900 * 3));

	float maxHeight = 1000;

	if(blendTexCol2.x + blendTexCol2.y + blendTexCol2.z + blendTexCol2.w == 0)
	{
		float inverseTotal = 1.0f / (blendTexCol1.x + blendTexCol1.y + blendTexCol1.z + blendTexCol1.w);

		// First blend map
		float4 terCol0 = tx1.Sample(samLinear, input.tex);

			float4 terCol1;
		if (input.realPos.y > 2000)
		{
			terCol1 =	(tx2.Sample(samLinear, input.tex)  *		(((maxHeight - (input.realPos.y - 2000)) / maxHeight))) +
						(tx22.Sample(samLinear, input.tex) * (1 -	 ((maxHeight - (input.realPos.y - 2000)) / maxHeight)));
			//terCol1 = tx22.Sample(samLinear, input.tex);
		}
		else
			terCol1 = tx2.Sample(samLinear, input.tex);

		float4 terCol2 = tx3.Sample(samLinear, input.tex);
		float4 terCol3 = tx4.Sample(samLinear, input.tex);

		terCol0 *= blendTexCol1.x * inverseTotal;
		terCol1 *= blendTexCol1.y * inverseTotal;
		terCol2 *= blendTexCol1.z * inverseTotal;
		terCol3 *= blendTexCol1.w * inverseTotal;

		output.color = float4((terCol0 + terCol1 + terCol2 + terCol3).xyz, 1);
	}
	else if(blendTexCol1.x + blendTexCol1.y + blendTexCol1.z + blendTexCol1.w == 0)
	{
		float inverseTotal = 1.0f / (blendTexCol2.x + blendTexCol2.y + blendTexCol2.z + blendTexCol2.w);

		// Second blend map
		float4 terCol4 = tx5.Sample(samLinear, input.tex);
		//float4 terCol5 = tx6.Sample(samLinear, input.tex);
		//float4 terCol6 = tx7.Sample(samLinear, input.tex);
		//float4 terCol7 = tx8.Sample(samLinear, input.tex);

		terCol4 *= blendTexCol2.x * inverseTotal;
		//terCol5 *= blendTexCol2.y * inverseTotal;
		//terCol6 *= blendTexCol2.z * inverseTotal;
		//terCol7 *= blendTexCol2.w * inverseTotal;

		output.color = float4((terCol4/* + terCol5 + terCol6 + terCol7*/).xyz, 1);
	}
	else
	{
		float inverseTotal = 1.0f / (blendTexCol1.x + blendTexCol1.y + blendTexCol1.z + blendTexCol1.w + blendTexCol2.x + blendTexCol2.y + blendTexCol2.z + blendTexCol2.w);

		// First blend map
		float4 terCol0 = tx1.Sample(samLinear, input.tex);
		//float4 terCol1 = tx2.Sample(samLinear, input.tex);
		//float4 terCol1 = (tx2.Sample(samLinear, input.tex) * (((maxHeight - input.realPos.y) / maxHeight))) + (tx22.Sample(samLinear, input.tex) * (1 - ((maxHeight - input.realPos.y) / maxHeight)));
		
		float4 terCol1;
		if (input.realPos.y > 2000)
		{
			terCol1 =	(tx2.Sample(samLinear, input.tex)  *		(((maxHeight - (input.realPos.y - 2000)) / maxHeight))) +
						(tx22.Sample(samLinear, input.tex) * (1 -	 ((maxHeight - (input.realPos.y - 2000)) / maxHeight)));
			//terCol1 = tx22.Sample(samLinear, input.tex);
		}
		else
			terCol1 = tx2.Sample(samLinear, input.tex);

		float4 terCol2 = tx3.Sample(samLinear, input.tex);
		float4 terCol3 = tx4.Sample(samLinear, input.tex);

		terCol0 *= blendTexCol1.x * inverseTotal;
		terCol1 *= blendTexCol1.y * inverseTotal;
		terCol2 *= blendTexCol1.z * inverseTotal;
		terCol3 *= blendTexCol1.w * inverseTotal;

		// Second blend map
		float4 terCol4 = tx5.Sample(samLinear, input.tex);
		//float4 terCol5 = tx6.Sample(samLinear, input.tex);
		//float4 terCol6 = tx7.Sample(samLinear, input.tex);
		//float4 terCol7 = tx8.Sample(samLinear, input.tex);

		terCol4 *= blendTexCol2.x * inverseTotal;
		//terCol5 *= blendTexCol2.y * inverseTotal;
		//terCol6 *= blendTexCol2.z * inverseTotal;
		//terCol7 *= blendTexCol2.w * inverseTotal;

		output.color = float4((terCol0 + terCol1 + terCol2 + terCol3 + terCol4/* + terCol5 + terCol6 + terCol7*/).xyz, 1);
	}

	//fetch the normalmap.
	float3 normal = map_normal.Sample(samLinear, input.tex).xyz;

	//right now b channel is up when g should be. this will prob
	//change when we use the generated one
	float temp = normal.b;
	normal.b = normal.g;
	normal.g = temp;

	// add the normals from the map to the original normal
	normal = normalize(normal + input.norm);
	//normal = input.norm;

	/*if (length(input.realPos - cameraPos.xyz) > 30000)
	{
		normal = normalize(input.norm);
	}*/
	output.normal = float4((normal + 1) * 0.5, 0);		//convert normal from (-1 - 1) to (0-1) to fit a texture

	float ka = 0.2;
	output.specular = float4(map_ks.Sample(samLinear, input.tex).xyz, ka);  //get specular from specular map only id there is a specular map

	output.glow = float4(0,0,0,0);

	return output;
}