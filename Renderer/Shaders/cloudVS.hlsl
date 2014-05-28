
struct VS_INPUT
{
	float size : SIZE;
};

struct GS_INPUT
{
	float size : SIZE;
	uint  id   : ID;
};

GS_INPUT main( VS_INPUT input, uint VertexID : SV_VERTEXID )
{
	GS_INPUT output;

	output.size = input.size;
	output.id	= VertexID;

	return output;
}