#include "resources.hlsl"

struct GS_Input
{
	float4 pos	: POS;
};

struct PS_Input
{
	float4 pos		: SV_POSITION;
	float2 texCoord	: TEXCOORD0;
	float2 type		: TEXTYPE;
};

[maxvertexcount(16)]
void main(point GS_Input sprite[1], inout TriangleStream<PS_Input> triStream)
{
	PS_Input ps;

	float dim = 0.5f;

	float2 texCoord[4] =
	{
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f)
	};

	if (sprite[0].pos.z == 0)
	{
		ps.type.x = 0;
		ps.type.y = 0;

		//Bottom left
		ps.pos = float4(sprite[0].pos.x, sprite[0].pos.y - dim, 0, 1); //float4( -1, -1, 0, 1 );
		ps.texCoord = texCoord[2];
		triStream.Append(ps);

		//Top left
		ps.pos = float4(sprite[0].pos.x, sprite[0].pos.y, 0, 1);//float4( -1, 1, 0, 1 );
		ps.texCoord = texCoord[0];
		triStream.Append(ps);

		//Bottom right
		ps.pos = float4(sprite[0].pos.x + dim, sprite[0].pos.y - dim, 0, 1);//float4( 1, -1, 0, 1);
		ps.texCoord = texCoord[3];
		triStream.Append(ps);

		//Top right
		ps.pos = float4(sprite[0].pos.x + dim, sprite[0].pos.y, 0, 1);//float4( 1, 1, 0, 1 );
		ps.texCoord = texCoord[1];
		triStream.Append(ps);
	}
	else if (sprite[0].pos.z == 1)
	{
		for (int i = 0; i < 4; i++)
		{
			ps.type.x = 1;
			ps.type.y = i;

			//Bottom left
			ps.pos = float4(sprite[0].pos.x + (dim * i), sprite[0].pos.y - dim, 0, 1); //float4( -1, -1, 0, 1 );
			ps.texCoord = texCoord[2];
			triStream.Append(ps);

			//Top left
			ps.pos = float4(sprite[0].pos.x + (dim * i), sprite[0].pos.y, 0, 1);//float4( -1, 1, 0, 1 );
			ps.texCoord = texCoord[0];
			triStream.Append(ps);

			//Bottom right
			ps.pos = float4(sprite[0].pos.x + dim + (dim * i), sprite[0].pos.y - dim, 0, 1);//float4( 1, -1, 0, 1);
			ps.texCoord = texCoord[3];
			triStream.Append(ps);

			//Top right
			ps.pos = float4(sprite[0].pos.x + dim + (dim * i), sprite[0].pos.y, 0, 1);//float4( 1, 1, 0, 1 );
			ps.texCoord = texCoord[1];
			triStream.Append(ps);
		}
	}
}