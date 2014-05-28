#include "SkysphereAspect.h"
#include <glm/gtc/matrix_transform.hpp>

SkysphereAspect::SkysphereAspect()
{
	vertexBufferID = -1;
	indexBufferID = -1;
	amount = 0;
	generateSphere();
}

SkysphereAspect::~SkysphereAspect()
{}

void SkysphereAspect::init(MainState& _mainState)
{}

void SkysphereAspect::generateSphere()
{
	vertices.resize(8);
	indices.resize(36);

	vertices[0].pos = glm::vec3(-1, -1, -1);
	vertices[1].pos = glm::vec3(-1, 1, -1);
	vertices[2].pos = glm::vec3(1, 1, -1);
	vertices[3].pos = glm::vec3(1, -1, -1);
	vertices[4].pos = glm::vec3(-1, -1, 1);
	vertices[5].pos = glm::vec3(1, -1, 1);
	vertices[6].pos = glm::vec3(1, 1, 1);
	vertices[7].pos = glm::vec3(-1, 1, 1);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;
	indices[6] = 4;
	indices[7] = 5;
	indices[8] = 6;
	indices[9] = 4;
	indices[10] = 6;
	indices[11] = 7;
	indices[12] = 2;
	indices[13] = 6;
	indices[14] = 5;
	indices[15] = 2;
	indices[16] = 5;
	indices[17] = 3;
	indices[18] = 0;
	indices[19] = 4;
	indices[20] = 7;
	indices[21] = 0;
	indices[22] = 7;
	indices[23] = 1;
	indices[24] = 1;
	indices[25] = 7;
	indices[26] = 6;
	indices[27] = 1;
	indices[28] = 6;
	indices[29] = 2;
	indices[30] = 0;
	indices[31] = 3;
	indices[32] = 5;
	indices[33] = 0;
	indices[34] = 5;
	indices[35] = 4;

	amount = 36;
}

void SkysphereAspect::runAspect(float _dt)
{
	return;
}