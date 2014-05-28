#include "resources.hlsl"
Texture2D tex01	: register( t1 );
struct PS_Input
{
	float4	pos			: SV_POSITION;
	float2	tex			: TEXCOORD;
};

float4 sobelFilter(float2 texCoord)
{
	float2 xOffset	= float2( 3.0 / resolution.x, 1.0 );
	float2 yOffset	= float2( 0.0, 3.0 / resolution.y );
	float2 pp		= texCoord - yOffset;

	float4 cc;
	cc			= tex01.Sample( PointSamplerClamp, pp - xOffset );
	float g00	= dot(cc.rgb, ( (0.33333).xxx ) );
	cc			= tex01.Sample( PointSamplerClamp, pp );
	float g01	= dot(cc.rgb, ( (0.33333).xxx ) );
	cc			= tex01.Sample( PointSamplerClamp, pp + xOffset );
	float g02	= dot(cc.rgb, ( (0.33333).xxx ) );

	cc			= tex01.Sample( PointSamplerClamp, texCoord - xOffset );
	float g10	= dot(cc.rgb, ( (0.33333).xxx ) );
	cc			= tex01.Sample( PointSamplerClamp, texCoord );
	float g11	= dot(cc.rgb, ( (0.33333).xxx ) );
	cc			= tex01.Sample( PointSamplerClamp, texCoord + xOffset );
	float g12	= dot(cc.rgb, ( (0.33333).xxx ) );

	pp = texCoord + yOffset;

	cc			= tex01.Sample( PointSamplerClamp, pp - xOffset );
	float g20	= dot(cc.rgb, ( (0.33333).xxx ) );
	cc			= tex01.Sample( PointSamplerClamp, pp );
	float g21	= dot(cc.rgb, ( (0.33333).xxx ) );
	cc			= tex01.Sample( PointSamplerClamp, pp + xOffset );
	float g22	= dot(cc.rgb, ( (0.33333).xxx ) );

	float k00	= -1;
	float k01	= -2;
	float k02	= -1;
	float k10	= 0;
	float k11	= 0;
	float k12	= 0;
	float k20	= 1;
	float k21	= 2;
	float k22	= 1;

	float gx	= 0;
	float gy	= 0;

	gx	+= g00 * k00;
	gx	+= g01 * k01;
	gx	+= g02 * k02;
	gx	+= g10 * k10;
	gx	+= g11 * k11;
	gx	+= g12 * k12;
	gx	+= g20 * k20;
	gx	+= g21 * k21;
	gx	+= g22 * k22;

	gy	+= g00 * k00;
	gy	+= g01 * k01;
	gy	+= g02 * k02;
	gy	+= g10 * k10;
	gy	+= g11 * k11;
	gy	+= g12 * k12;
	gy	+= g20 * k20;
	gy	+= g21 * k21;
	gy	+= g22 * k22;

	float norm = sqrt( gx * gx + gy * gy );

	if( norm > 0.25 )
	{
		return float4( 0.0, 0.0, 0.0, 0.0 );
	}

	return float4( 1.0, 1.0, 1.0, 1.0 );
}

float4 main(PS_Input input) :SV_TARGET0
{
	float4 edge = sobelFilter(input.tex);
	if(edge.r == 0)
		discard;
	return tex01.Sample(samAnisotropic, input.tex);	//obtain color from texture
}