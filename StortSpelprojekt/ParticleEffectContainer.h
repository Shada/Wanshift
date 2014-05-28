#pragma once
#ifdef __DEBUG
#include <gtest\gtest.h>
#endif
#include "Container.h"

struct Particle
{
	Particle()
	{
		pos = glm::vec3(0, 0, 0);
		size = glm::vec2(0, 0);
		rot = 0;
		alpha = 0;
	}
	glm::vec3	pos;
	glm::vec2	size;
	float		rot;
	float		alpha;
};

struct ParticleEffectContainer : Container
{
	// position data
	std::vector< glm::vec2 >	posData;
	std::vector< UINT >			vertexIndex;
	std::vector< UINT >			vertexAmount;

	//not mapped
	std::vector< Particle >		particleData;
	std::vector< UINT >			EffectIndex;

private:
	void addData(uint _index, uint _number)
	{
		insert<glm::vec2>(posData, _index, _number, glm::vec2(0, 0));
		insert< UINT >(vertexIndex, _index, _number, 0 );
		insert< UINT >(vertexAmount, _index, _number, 0 );
	}

	void removeData(uint _index, uint _number)
	{
		remove(posData, _index, _number);
		remove(vertexIndex, _index, _number);
		remove(vertexAmount, _index, _number);
	}
};