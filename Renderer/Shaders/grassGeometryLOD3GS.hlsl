#include "resources.hlsl"
struct GS_Input
{
	float3 pos		: POSITION;
};
struct GS_Output
{
	float4 pos		: SV_POSITION;
	float3 normal	: NORMAL;
	float2 tex		: TEXC;
	float random	: RAND;
};

//this since one triangle is taken in as input. dont really know
// if this is better than using point instead
static const uint numImpostorsPerInput = 3;
static const uint verticesPerQuad = 4;
static const uint QuadsPerImpostor = 4;
Texture2D heightMap : register( t1 );
Texture2D blendMap	: register( t2 );

void appendVertex(inout GS_Output vertex, inout TriangleStream< GS_Output > triStream )
{
	vertex.pos = mul(mView,vertex.pos);
	vertex.pos = mul(mProj,vertex.pos);

	vertex.random = 1.0;
	triStream.Append(vertex);
}

[maxvertexcount(numImpostorsPerInput * verticesPerQuad *QuadsPerImpostor)]
void main(triangle GS_Input input[3], inout TriangleStream< GS_Output > triStream)
{
	[unroll]
	for (uint i = 0; i < numImpostorsPerInput; i++)
	{
		float3 p = input[i].pos;
		const float blend = blendMap.SampleLevel(samLinear2,frac(p.xz/5100.0),0).r;
		const float random = rand3(p.xz);

		p.y = heightMap.SampleLevel(samLinear2,frac(p.xz / 5100.0),0).r;
		
		GS_Output vertices[verticesPerQuad];
		for(uint y = 0; y < verticesPerQuad; y++)
		{
			vertices[y] = (GS_Output)0;
		}

		float rot = random*PI;
		float4x4 W =	{	cos(rot),	0,		sin(rot),	0,
							0,			1,		0,			0,
							-sin(rot),	0,		cos(rot),	0,
							p.x,		p.y,	p.z,		1
						};
		W = transpose(W);

		float width= 40.0;
		float height = 20.0;
		//create front quad
		vertices[0].pos = mul(W,float4(width,0,0,1));
		vertices[1].pos = mul(W,float4(0,0,0,1));
		vertices[2].pos = mul(W,float4(width,height,0,1));
		vertices[3].pos = mul(W,float4(0,height,0,1));

		vertices[0].tex = float2(1,1);
		vertices[1].tex = float2(0,1);
		vertices[2].tex = float2(1,0);
		vertices[3].tex = float2(0,0);
		
		[unroll]
		for(y = 0; y < verticesPerQuad; y++)
		{
			vertices[y].pos = mul(mView,vertices[y].pos);
			vertices[y].pos = mul(mProj,vertices[y].pos);

			vertices[y].random = 1.0;
			triStream.Append(vertices[y]);
		}
		triStream.RestartStrip();

		//create right quad
		vertices[0].pos = mul(W,float4(width,0,width,1));
		vertices[1].pos = mul(W,float4(width,0,0,1));
		vertices[2].pos = mul(W,float4(width,height,width,1));
		vertices[3].pos = mul(W,float4(width,height,0,1));

		vertices[0].tex = float2(1,1);
		vertices[1].tex = float2(0,1);
		vertices[2].tex = float2(1,0);
		vertices[3].tex = float2(0,0);
		
		[unroll]
		for(y = 0; y < verticesPerQuad; y++)
		{
			vertices[y].pos = mul(mView,vertices[y].pos);
			vertices[y].pos = mul(mProj,vertices[y].pos);

			vertices[y].random = 1.0;
			triStream.Append(vertices[y]);
		}
		triStream.RestartStrip();

		//create back quad
		vertices[0].pos = mul(W,float4(0,0,width,1));
		vertices[1].pos = mul(W,float4(width,0,width,1));
		vertices[2].pos = mul(W,float4(0,height,width,1));
		vertices[3].pos = mul(W,float4(width,height,width,1));

		vertices[0].tex = float2(1,1);
		vertices[1].tex = float2(0,1);
		vertices[2].tex = float2(1,0);
		vertices[3].tex = float2(0,0);
		
		[unroll]
		for(y = 0; y < verticesPerQuad; y++)
		{
			vertices[y].pos = mul(mView,vertices[y].pos);
			vertices[y].pos = mul(mProj,vertices[y].pos);

			vertices[y].random = 1.0;
			triStream.Append(vertices[y]);
		}
		triStream.RestartStrip();

		//create left quad
		vertices[0].pos = mul(W,float4(0,0,0,1));
		vertices[1].pos = mul(W,float4(0,0,width,1));
		vertices[2].pos = mul(W,float4(0,height,0,1));
		vertices[3].pos = mul(W,float4(0,height,width,1));

		vertices[0].tex = float2(1,1);
		vertices[1].tex = float2(0,1);
		vertices[2].tex = float2(1,0);
		vertices[3].tex = float2(0,0);
		
		[unroll]
		for(y = 0; y < verticesPerQuad; y++)
		{
			vertices[y].pos = mul(mView,vertices[y].pos);
			vertices[y].pos = mul(mProj,vertices[y].pos);

			vertices[y].random = 1.0;
			triStream.Append(vertices[y]);
		}
		triStream.RestartStrip();
	}
}
