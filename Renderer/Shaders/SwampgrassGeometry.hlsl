#include "resources.hlsl"

static const int maxNumSegments = 6;
static const int numBladesPerInput = 3;
static const float maxBladeHeight = 12.0;

Texture2D heightMap[9]	: register(t1);
Texture2D blendMap		: register(t10);
Texture2D biomBlendMap	: register(t11);
Texture2D windMap		: register(t12);	

cbuffer cbTerrain : register(b5)
{
	float2	tStartPos;
	float2  tLocalHeightPos;
	float	tPointStep;
	float3	tPadding;
	float4	tinyChunkPos[9];
};

cbuffer cbShifting : register(b6)
{
	float interpVal;
	float3 shiftpadd;
};

struct GS_Input
{
	float3 pos		: POSITION;
};
struct GS_Output
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL;
	float2 tex			: TEXC;
	float random		: RAND;
	float brightness	: BRIGHT;
	float height		: HEIGHT;
};
void generateNormals(inout GS_Output v0, inout GS_Output v1, inout GS_Output v2 )
{
	float3 vec0 = normalize(v0.pos.xyz - v1.pos.xyz);
	float3 vec1 = normalize(v0.pos.xyz - v2.pos.xyz);
	
	float3 normal = normalize( cross( vec0, vec1 ) );
	
	//find out if this normal is facing the camera, if it is not, negate it
	//because we want the one facing the camera
	float3 dir = normalize(v0.pos.xyz - cameraPos.xyz);
	if(dot(dir,normal) > 0)
	{
		normal = -normal;
	}

	v0.normal += normal;
	v1.normal += normal;
	v2.normal += normal;
}
void generateBlade(float3 rootPos, inout TriangleStream<GS_Output> triStream, const int numSegments)
{
	//construct one blade

	float3 p = rootPos;
	const float blend = blendMap.SampleLevel(samLinear2,frac(p.xz/5100.0),0).r;
	const float blend2 = biomBlendMap.SampleLevel(samLinear2, frac((p.xz - tStartPos) / (45900 * 3)), 0).a;
	const float random = rand3(p.xz);
	const float dist = length(p - cameraPos.xyz);
	const float originalBladeHeight = (maxBladeHeight * abs(random * 0.3 + 0.7)) * (blend2) * (blend2);
	const float bladeSegHeight = originalBladeHeight/(float)numSegments * max(0,(3000-dist)/3000);
	//if zero no need to create blade since its not gonna be visible anyway
	if(bladeSegHeight == 0)// || (i == 1 && dist > 600) || (i == 2 && dist > 300)) //should prob not be culled that way
		return;
	
	//one blade will consist of a triangle strip
	const int numVertsPerBlade = (numSegments + 1) * 2 -1;
	const int maxNumVertsPerBlade = (maxNumSegments + 1) * 2 -1;
	const float bladeWidth = .2*originalBladeHeight + (maxNumSegments - numSegments )* originalBladeHeight * 0.04; //2.0;
	
	//determine which terrain chunks heightmap to sample from
	float2 terrainchunkPos = floor(p.xz / 5100.0) * 5100.0;
	[unroll]
	for(int j = 0; j < 9; j++)
	{
		if(terrainchunkPos.x == tinyChunkPos[j].x && terrainchunkPos.y == tinyChunkPos[j].z)
		{
			p.y = heightMap[j].SampleLevel(samLinear2,frac((p.xz) /5100.0),0).r;
			break;
		}
	}

	//dont like this, allocates more than necessary
	GS_Output vertices[maxNumVertsPerBlade];
	[unroll]
	for(int y = 0; y < numVertsPerBlade; y++)
	{
		vertices[y] = (GS_Output)0;
	}

	//create the world matrix		
	float rot = rand3(p.xz+0.004)*PI*2;
	float4x4 W =	{	cos(rot),	0,		sin(rot),	p.x,
						0,			1,		0,			p.y,
						-sin(rot),	0,		cos(rot),	p.z,
						0,		0,	0,		1
					};

	//first vertices in the strip are put at the root
	vertices[0].pos = mul(W,float4(0,0,0,1));
	vertices[1].pos = mul(W,float4(bladeWidth,0,0,1));

	vertices[0].tex = float2(0,1);
	vertices[1].tex = float2(1,1);
	//add the rest of the segments except the last
	float segHeight;
	float3 wind = float3(0.005,0.005,0.005);
	float windCoeff;
	float segDiv;
	float segDivSqrd;
	float widthOffset;
	float presin = 0.025 * sin( random*8.5 + gameTime*0.9);
	[unroll]
	for(y = 1; y < numSegments + 0; y++)
	{
		segHeight = y*bladeSegHeight;
		//wind = float3(0.01,0,0);
		windCoeff = segHeight*segHeight*segHeight * presin;
		//add the wind to the translation in the matrix
		W._14 = p.x + wind.x * windCoeff; 
		W._34 = p.z + wind.z * windCoeff;
		W._24 = p.y - length(wind) * abs(windCoeff) * 0.5;
		segDiv= y*(1.0/numSegments);
		segDivSqrd = segDiv*segDiv;
		widthOffset = segDivSqrd * bladeWidth * 0.499;
		vertices[y * 2].pos		= mul(W, float4(widthOffset,segHeight,0,1));
		vertices[y * 2 + 1].pos = mul(W, float4(bladeWidth - widthOffset,segHeight,0,1));

		//the 1-y*(1.0/numSegments) calc could be pre calculated for every segment
		float texy = 1-segDiv;
		vertices[y * 2].tex		= float2(segDivSqrd*0.499,texy);
		vertices[y * 2 + 1].tex = float2(1-segDivSqrd*0.499,texy);
	}
	//add the last segement. cant be part of the loop because it only consist of one triangle
	segHeight = y*bladeSegHeight;
	//wind = float3(0.01,0,0);
	windCoeff = segHeight*segHeight*segHeight * presin;
	//add the wind to the translation in the matrix
	W._14 = p.x + wind.x * windCoeff; 
	W._34 = p.z + wind.z * windCoeff;
	W._24 = p.y - length(wind) * abs(windCoeff) * 0.5;
	widthOffset =  bladeWidth * 0.499;
	vertices[y*2].pos		= mul(W, float4(widthOffset,segHeight,0,1));
	vertices[y*2].tex		= float2(0.499,0);

	//calculate normals, last segment first, because if not some kind of cache coherence thing makes life difficult
	//at least on this hardware. lets check on different. it also seems to be sovled by not unrolling this loop
	//but that produces a lot of warnings. including one that says its forcing unroll, so thats weird
	uint k;
	k = (numSegments-1) * 2;
	generateNormals( vertices[k],vertices[k+1],vertices[k+2] );
	[unroll]
	for(y = 0; y < numSegments-1; y++)
	{
		k = y * 2;
		generateNormals( vertices[k],vertices[k+1],vertices[k+2] );
		generateNormals( vertices[k+1],vertices[k+2],vertices[k+3] );
	}
	
	// unroll fixes warning about not emitting all system interpreted values, which is
	// most likely caused by having non-static values in the loops
	const float brightness = (float)maxNumSegments/(float)numSegments*0.01;
	//float random2 = rand3(p.xz+0.008);
	[unroll]
	for(y = 0; y < numVertsPerBlade; y++)
	{
		vertices[y].pos = mul(mView,vertices[y].pos);
		vertices[y].pos = mul(mProj,vertices[y].pos);

		vertices[y].normal = normalize(vertices[y].normal);

		vertices[y].random = random;
		vertices[y].brightness = brightness;
		vertices[y].height = segHeight;
		triStream.Append(vertices[y]);
	}
}