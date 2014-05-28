
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

struct PS_INPUT
{
	float4 position	: SV_Position;
	float2 texCoord : COLOR;
};

PS_INPUT main(uint VertexID : SV_VERTEXID)
{
	PS_INPUT output;

	float2 xy = quad[VertexID % 6];
	float2 uv = texC[VertexID];

	output.position = float4( xy.x, xy.y, 0.0, 1.0 );
	output.texCoord = uv;

	return output;
}