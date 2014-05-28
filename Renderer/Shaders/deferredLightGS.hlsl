struct PS_Input
{
	float4 pos	: SV_POSITION;
	float2 tex	: TEXCOORD0;
};

[maxvertexcount(4)]
// will create a fullscreen quad
void main( point float4 input[1] : SV_POSITION, inout TriangleStream<PS_Input> triStream )
{
	PS_Input output = (PS_Input)0;

	output.pos = float4( -1, -1, 0, 1 );
	output.tex = float2( 0, 1 );
	triStream.Append(output);

	output.pos = float4( -1, 1, 0, 1 );
	output.tex = float2( 0, 0 );
	triStream.Append(output);

	output.pos = float4( 1, -1, 0, 1);
	output.tex = float2( 1, 1 );
	triStream.Append(output);

	output.pos = float4( 1, 1, 0, 1 );
	output.tex = float2( 1, 0 );
	triStream.Append(output);
}