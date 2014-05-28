#include "SkyDome.h"
#include <glm/glm.hpp>

struct DomeVDesc
{
	glm::vec3 p;
};

SkyDome::SkyDome()
	: radius(250000)
{
}


SkyDome::~SkyDome()
{
}

int SkyDome::getVIndex()
{
	return vIndex;
}

int SkyDome::getBIndex()
{
	return bIndex;
}

int SkyDome::getNumIndices()
{
	return nIndex;
}

bool SkyDome::init(int _rings, int _segments, RenderInterface* _graphics)
{

	int nVertices = (_rings + 1) * (_segments + 1);
	int nIndices = 2 * _rings * (_segments + 1);

	float mPI = 3.14159265359f;
	float deltaRangle = (mPI / _rings);
	float deltaSangle = (2.0f * mPI / _segments);

	DomeVDesc*		vertices = new DomeVDesc[nVertices];
	unsigned int*	indices = new unsigned int[nIndices];

	unsigned int cIndex = 0;
	unsigned int iIndex = 0;
	for(int ring = 0; ring < _rings + 1; ++ring)	// divide sphere in rings
	{
		float r0 = sinf(ring * deltaRangle);
		float y0 = cosf(ring * deltaRangle);

		for(int segment = 0; segment < _segments + 1; ++segment)	// divide ring in segments
		{
			float x0 = r0 * sinf(segment * deltaSangle);

			float z0 = r0 * cosf(segment * deltaSangle);

			vertices[cIndex].p = glm::vec3(x0 * radius, z0 * radius, y0 * radius);
			cIndex++;

			if(ring != _rings)
			{
				indices[iIndex] = cIndex + (_segments);
				iIndex++;
				indices[iIndex] = cIndex;
				iIndex++;
			}

		}
	}

	nIndex = iIndex;
	printf("SkyDome!\n");
	vIndex = _graphics->createVBufferImmutable((void**)&vertices[0], cIndex, sizeof(DomeVDesc));
	bIndex = _graphics->createIndexBufferImmutable((void**)&indices[0], nIndex);


	delete[] vertices;
	delete[] indices;

	if(vIndex < 0 || bIndex < 0)
		return false;

	return true;
}

void SkyDome::draw(RenderInterface* _graphics)
{
	if(!_graphics)
		return;
}