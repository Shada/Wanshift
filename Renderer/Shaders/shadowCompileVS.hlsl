struct VS_Input
{
	// probably a lot of unnecessary input
	float3 pos	: POSITION;
	float3 norm	: NORMAL;
	float2 tex	: TEXCOORD0;
};

float4 main(VS_Input input) : SV_POSITION
{
	// doesn't need to do anything, just wanna move on to
	// the geometry shader which will create a fullscreen quad
	return float4(0,0,0,0);
}