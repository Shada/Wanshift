
#include "resources.hlsl"

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;	
};

cbuffer cbHeat : register(b6)
{
	float heatDistance;
	float vp_inv_width;
	float vp_inv_height;
	float offsetScale;
	float hDt;
	float3 hPadding;
};

texture2D texture0	: register(t10);
texture3D texture1	: register(t6);
texture2D tDepth	: register(t7);
texture2D tSky		: register(t8);

float loudness(float _depth, float4 _clip)
{
	float strength;
	float4 pos = mul(mProjInv, _clip);
	pos = mul(mViewInv, pos);
	pos /= pos.w;

	float3 eyePixel		= pos.xyz - cameraPos.xyz;
	float pixelDistance	= length(eyePixel);

	float distByDistance= max( pixelDistance - 0.0, 0.0 );
	float distByHeight	= exp( -1.0 * _clip.y );
	float intensity		= distByDistance * distByHeight;

	strength = intensity;


	return strength;
}

float4 main(PS_INPUT input) : SV_TARGET
{
	float2 texCoord = input.texCoord;
	float depth		= tDepth.Sample( samLinear, texCoord ).r;

	float4 clipPos	= float4( texCoord.x * 2.0 - 1.0, (1.0 - texCoord.y) * 2.0 - 1.0, depth, 1 );

	float2 uv = input.texCoord;
	float2 c = texture1.Sample( samLinear, float3( uv, heatDistance * hPadding.x) ).xy;
	c = ( (c * 2.0) - 1.0) * offsetScale * heatDistance;
	//c *=10.5f;
	//c.y *= 0.25;
	float4 sceneDist;

	if(depth <= 0.999 || depth == 1)
		sceneDist = texture0.Sample( samLinear, uv );
	else
		sceneDist = texture0.Sample( samLinear2,uv + c.xy );

	return sceneDist;
}