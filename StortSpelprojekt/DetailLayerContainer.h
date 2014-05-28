#pragma once

#include <array>
#include "Container.h"
#include "glm\glm.hpp"

struct DetailLayerContainer : Container
{
	int textureIndex;
	int shaderResourceIndex;
	static const uint sizeX = 256;
	static const uint sizeY = 256;
	// Sand
	std::array< std::array< glm::vec4, sizeY >, sizeX > detailMap;
	std::array< std::array< std::array< glm::vec4, sizeY >, sizeX >, 8 > sandMap;
private:
	void addData(uint _index, uint _number) override {}
	void removeData(uint _index, uint _number) override {}
};
