#include "resources.hlsl"

cbuffer RainSimulationConstants : register( b12 )
{
	//matrix	matToHeight			;	// 64
	float3	boundCenter			;	// 12
	float	rDt					;	// 4
	float3	boundHalfSize		;	// 12
	float	windVariation		;	// 4
	float2	windForce			;	// 8
	float	verticalSpeed		;	// 4
	float	heightMapHalfSize	;	// 4
	float	stretch				;	// 4
	float3	rscpadding			;	// 12
};									// 64

cbuffer RainConstants : register( b13 )
{
	matrix	rViewProjMatrix	;	// 64
	float3	viewDir			;	// 12
	float	rScale			;	// 4
	float4	ambientColor	;	// 16
};								// = 96


static const float2 arrUV[6] = 
{ 
	float2(1.0, 0.0),
	float2(1.0, 1.0),
	float2(0.0, 0.0),

	float2(0.0, 0.0),
	float2(1.0, 1.0),
	float2(0.0, 1.0),
};

static const float2 arrBasePos[6] =
{
 	float2( 1.0, -1.0 ),
 	float2( 1.0, 0.0 ),
 	float2( -1.0, -1.0 ),

 	float2( -1.0, -1.0 ),
	float2( 1.0, 0.0 ),
	float2( -1.0, 0.0 ),
};

struct CSBuffer
{
	float3	position;
	float3	velocity;
	float	state;
};

StructuredBuffer<CSBuffer> RainData : register( t1 );

struct PS_Input
{
	float4	position	: SV_Position;
	float	clip		: FOO;//SV_ClipDistance0;
	float2	texCoord	: TEXCOORD0;
};

PS_Input main(uint VertexID : SV_VERTEXID)
{
	PS_Input ps;

	// fetch current drop
	CSBuffer drop	= RainData[VertexID / 6];

	// remove after debug
	float3	bCenter = boundCenter;	// 12
	float	rrdt = rDt					;	// 4
	float3	bhSize = boundHalfSize		;// 12
	float	windw = windVariation		;	// 4
	float2	wForce = windForce			;// 8
	float	vspeed = verticalSpeed		;// 4
	float	hmhSize = heightMapHalfSize	;	// 4

	float3 dropPos	= drop.position;

	// find extension direction
	float3 rainDir		= normalize(drop.velocity);
	float3 rainRight	= normalize(cross(viewDir, rainDir));

	// extend drop position
	float rainScale = rScale;
	float2 offsets	= arrBasePos[VertexID % 6];
	dropPos			+= rainRight * offsets.x * rainScale * stretch;// * 0.025;
	dropPos			+= rainDir * offsets.y * rainScale;

	float4 drpPs = mul(float4(dropPos,1), rViewProjMatrix);
	//float4 drpPs = mul(mView, float4(dropPos,1.0));
	//drpPs = normalize(drpPs);
	//drpPs.z = 0.95;
	ps.position = drpPs;
	ps.texCoord = arrUV[VertexID % 6];
	ps.clip		= drop.state;

	// error testing
	//ps.position = float4( arrBasePos[VertexID % 6], 0, 1 );

	return ps;
}