#include "SwampgrassGeometry.hlsl"

static const int numSegments = 3;

//takes in 3 points at a time from the vbuffer, this because it is (probably, not tested) 
//better to calculate 3 blades in one go instead of taking one at a time. Hence the
//triangle input, when it should logically be point
[maxvertexcount((numSegments + 1) * numBladesPerInput * 2 -1)]
void main(triangle GS_Input input[3], inout TriangleStream<GS_Output> triStream)
{
	for(int i = 0; i < numBladesPerInput; i++)
	{
		const float random = rand3(input[i].pos.xz);
		const float dist = length(input[i].pos - cameraPos.xyz);
		if(random>max(0,(3000-dist)/3000))
			continue;
		generateBlade(input[i].pos,triStream,numSegments);
		triStream.RestartStrip();
	}
}