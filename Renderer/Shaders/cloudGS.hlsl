#include "resources.hlsl"

struct GS_INPUT
{
	float size	: SIZE;
	uint  id	: ID;
};

struct PS_INPUT
{
	float4 position	: SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float  size		: SIZE0;
	uint   id		: ID0;
};

cbuffer cbCloud : register(b10)
{
	float2	offset;
	uint	cloudSize;
	float	cellSize;
	uint	intervalStart;
	uint	intervalEnd;
	float2	cloudPad;
};

static const float2 tex[4] =
{
	float2(0,1),
	float2(0,0),
	float2(1,1),
	float2(1,0)
};

[maxvertexcount(4)]
void main(point GS_INPUT input[1], inout TriangleStream<PS_INPUT> output)
{
	PS_INPUT ps;
	ps.size = input[0].size;

	// center position
	float3 c;

	float scale = cellSize * cloudSize;
	c.x = offset.x + (input[0].id % cloudSize) * cellSize;
	c.y = 5000;
	c.z = offset.y + (input[0].id / cloudSize) * cellSize;

	//float hlen = input[0].size * cellSize * 2.0;
	//float hlen = 1000;

	float hlen = 0 * cellSize * 1;

	float3 up	= float3(0,1,0);
	float3 look	= cameraPos.xyz - c.xyz;
	look		= normalize(look);
	float3 right= cross(up,look);

	float4 v[4];
	v[0] = float4(c.xyz + hlen * right - hlen * up, 1.0);
	v[1] = float4(c.xyz + hlen * right + hlen * up, 1.0);
	v[2] = float4(c.xyz - hlen * right - hlen * up, 1.0);
	v[3] = float4(c.xyz - hlen * right + hlen * up, 1.0);

	[unroll]
	for(int i = 0; i < 4; ++i)
	{
		ps.position = mul(mProj, mul(mView, mul(mWorld, v[i]) ) );
		//ps.position = mul(mProj, mul(mView, v[i]) );
		ps.texCoord	= tex[i];
		ps.id = input[0].id;
		output.Append(ps);
	}
}