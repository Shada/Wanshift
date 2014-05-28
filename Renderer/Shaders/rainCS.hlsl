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
};	

struct RainDrop
{
	float3	position;
	float3	velocity;
	float	state;
};

Texture2D<float4>	noiseTexture		: register( t1 );
//Texture2D<float4>	heightTexture		: register( t2 );
RWStructuredBuffer<RainDrop> RainData	: register( u1 );

static const int g_iNumDispatch = 4;
static const int g_iNumThreads = 32;
static const int g_iDimSize = g_iNumDispatch * g_iNumThreads;

[numthreads(4, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	// fetch the current raindrop for current thread
	uint groupIdx = DTid.x + DTid.y * g_iDimSize;
	RainDrop drop = RainData[groupIdx];

	drop.position += drop.velocity * rDt; 

	// keep the drop within the bounds
	float2 offsetAmount = (drop.position.xz - boundCenter.xz) / boundHalfSize.xz;
	drop.position.xz -= boundHalfSize.xz * ceil(0.5 * offsetAmount - 0.5);
	
	// reset origin position if the drop is outside the bounds vertically
	if( abs(drop.position.y - boundCenter.y) > boundHalfSize.y || abs(drop.position.z - boundCenter.z) > boundHalfSize.z || abs(drop.position.x - boundCenter.x) > boundHalfSize.x)
	{
		// fetch random values from noise map
		float4 noiseNorm= noiseTexture.Load( int3(DTid.xy,0) );
		float4 noise	= (noiseNorm * 2.0) - 1.0;
		
		// keep the origin within the bounds
		drop.position.xz = boundCenter.xz + boundHalfSize.xz * noise.xy;

		// randomize height at the top of the bound
		drop.position.y = boundCenter.y + boundHalfSize.y;
		drop.position.y -= dot(noiseNorm.zw, 0.2f) * boundHalfSize.y;

		// set velocity based on wind force
		drop.velocity.xz= lerp(windForce, windForce * noise.zw, windVariation);
		drop.velocity.y	= verticalSpeed;
	}

	// wrong
	drop.state = 1.0;//dropHeight.z < tHeight ? 1.0 : -1.0;

	RainData[groupIdx] = drop;
}