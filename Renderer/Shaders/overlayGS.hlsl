struct VS_INPUT
{
	float2 topleft		: ANCHOR;
	float2 dimensions	: DIMENSIONS;
	float  opacity		: OPACITY;
};

struct PS_INPUT
{
	float4 p	: SV_POSITION;
	float  o	: OPACITY;
};

[maxvertexcount(4)]
void main( point VS_INPUT sprite[1], inout TriangleStream<PS_INPUT> triStream)
{
	PS_INPUT ps;

	//Bottom left
	ps.p = float4(sprite[0].topleft.x, sprite[0].topleft.y - sprite[0].dimensions.y, 0, 1); //float4( -1, -1, 0, 1 );
	ps.o = sprite[0].opacity;
	triStream.Append(ps);

	//Top left
	ps.p = float4(sprite[0].topleft.x, sprite[0].topleft.y, 0, 1);//float4( -1, 1, 0, 1 );
	ps.o = sprite[0].opacity;
	triStream.Append(ps);

	//Bottom right
	ps.p = float4(sprite[0].topleft.x + sprite[0].dimensions.x, sprite[0].topleft.y - sprite[0].dimensions.y, 0, 1);//float4( 1, -1, 0, 1);
	ps.o = sprite[0].opacity;
	triStream.Append(ps);

	//Top right
	ps.p = float4(sprite[0].topleft.x + sprite[0].dimensions.x, sprite[0].topleft.y, 0, 1);//float4( 1, 1, 0, 1 );
	ps.o = sprite[0].opacity;
	triStream.Append(ps);
}