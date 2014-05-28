#include "resources.hlsl"

cbuffer cbHeat : register(c6)
{
	float heatDistance;
	float vp_inv_width;
	float vp_inv_height;
	float offsetScale;
	float hDt;
	float3 hPadding;;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;	
};

static const float2 quad[6] =
{
	float2( -1.0, 1.0),
	float2( -1.0, -1.0),
	float2(  1.0, -1.0),
	float2( -1.0, 1.0),
	float2( 1.0, -1.0),
	float2( 1.0, 1.0)
};

static const float2 texC[6] =
{
	float2( 0.0, 0.0 ),
	float2( 0.0, 1.0 ),
	float2( 1.0, 1.0 ),
	float2( 0.0, 0.0 ),
	float2( 1.0, 1.0 ),
	float2( 1.0, 0.0 ),
};



PS_INPUT main(uint VertexID : SV_VERTEXID)
{
	PS_INPUT output;

	//output.position		= float4(float2(0,0), heatDistance, 1);
	//float2 xy = quad[VertexID % 6];
	//output.position = float4( xy.x, xy.y, heatDistance, 1.0 );
	//output.texCoord.x	= 0.5f * (1 + 0 + vp_inv_width);
	//output.texCoord.y	= 0.5f * (1 - 0 + vp_inv_height);

	float2 xy = quad[VertexID];
	float2 uv = texC[VertexID];
	
	output.position = float4( xy.x, xy.y, heatDistance, 1.0 );
	output.texCoord = uv;

	return output;
}