#pragma once

#include <vector>
#include <utility>
#include <numeric>
#include "Container.h"
#include <glm\glm.hpp>

typedef unsigned int uint;

struct CloudParticle
{
	glm::vec3	pos;
	glm::vec2	size;
	float		rot;
	float		age;
	UINT		type;
};


/* Contains smoke particle data, max size, actual size, and buffer index. */
struct SandStormContainer : Container
{
	// image indices
	std::vector< uint >			imageIndices;
	// the allocated size for the text in the buffer
	std::vector< uint >			maxSizes;
	// the actual size the text uses in the buffer
	std::vector< glm::vec3 >	emitterPositions;
	//is it the first time the particle is uptated?
	std::vector< bool >			firstTime;

	/* (NOTICE: Not to be mapped to each particle emitter. Use bufferIndices to get the start index of letters,
	and maxSizes for the allocated size in the particleData vector for each particle emitter.) */
	std::vector< CloudParticle > particleData;

	bool insideSandstorm, isMultiPlayer;
	int bufferIndex;
private:
	// See Container.h for documentation
	void addData(uint _index, uint _number)
	{
		insert<uint>(imageIndices, _index, _number, 0);
		insert<uint>(maxSizes, _index, _number, 0);
		insert<glm::vec3>(emitterPositions, _index, _number, glm::vec3(0, 0, 0));
		insert<bool>(firstTime, _index, _number, true);
	}

	// See Container.h for documentation
	void removeData(uint _index, uint _number)
	{
		remove(imageIndices, _index, _number);
		remove(maxSizes, _index, _number);
		remove(emitterPositions, _index, _number);
		remove(firstTime, _index, _number);
	}
};