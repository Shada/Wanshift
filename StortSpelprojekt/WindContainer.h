#pragma once

#include <array>
#include "Container.h"
#include "glm\glm.hpp"

#define CLOUD_CELLSIZE QUADSIZE * 3 * 3 * 4

struct WindContainer : Container
{
	glm::vec2 globalWindDirection;
	float globalWindForce;
	static const uint mapSize = 128;
	std::array< std::array< glm::vec3, mapSize >, mapSize > windMap;
	int windMapTexture;
	int windMapShaderResource;
	float cellSize;
	glm::vec2 mapPosition;

	std::array< std::array< float, 128 >, 128 > cloudMap;

	struct Particle
	{
		Particle(glm::vec3 _startPosition, glm::vec3 _direction) : startPosition(_startPosition), direction(_direction)
		{}
		glm::vec3 startPosition, direction;
	};
	std::vector<Particle> windParticlesQue;
private:
	void addData(uint _index, uint _number) override {}
	void removeData(uint _index, uint _number) override {}
};
