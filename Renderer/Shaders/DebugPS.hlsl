#include "resources.hlsl"

Texture2D tex0	: register(t1);
Texture2DArray tex1	: register(t2);


struct PS_Input
{
	float4 pos		: SV_POSITION;
	float2 texCoord	: TEXCOORD0;
	float2 type		: TEXTYPE;
};

float4 main(PS_Input _input) : SV_TARGET
{

	int temp = _input.type.y;

	//return float4(0, 1, 1, 1);
	if (_input.type.x == 0)
	{
		float4 final = tex0.Sample(samLinear, _input.texCoord);
			return float4(final.x, final.y, final.z, 1);
	}
	else if (_input.type.x == 1)
	{
		float4 final = tex1.Sample(samLinear, float3(_input.texCoord, temp));

		/*if (final.x < 1)
		{
			return float4(0, 0, 0, 1);

		}*/

		if (temp == 1)
		{
			return float4(final.x, final.y + 0.5, final.z, 1);

		}
		else if (temp == 2)
		{
			return float4(final.x, final.y, final.z + 0.5, 1);

		}
		else if (temp == 3)
		{
			return float4(final.x, final.y + 0.5, final.z + 0.5, 1);

		}



		return float4(final.x, final.y, final.z, 1);
	}
	return float4(0, 0, 0, 1);

}