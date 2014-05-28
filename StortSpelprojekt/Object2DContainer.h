#pragma once
#ifdef __DEBUG
#include <gtest\gtest.h>
#endif
#include <vector>
#include <utility>
#include <numeric>
#include "Container.h"
#include <glm\glm.hpp>

typedef unsigned int uint;

/* A container for basic 2D object data */
struct Object2DContainer : Container
{
	// position data
	std::vector< glm::vec2 >	posData;
	// scale data. (1,1) being normal size, (2,2) being twice the size in both x and y
	std::vector< glm::vec2 >	scaleData;
	// rotation data
	std::vector< float >		rotData;
	// tint (x,y,z) and alpha (w) data
	std::vector< glm::vec4 >	tintAlphaData;

private:
	void addData(uint _index, uint _number)
	{
		insert<glm::vec2>(posData, _index, _number, glm::vec2(0, 0));
		insert<glm::vec2>(scaleData, _index, _number, glm::vec2(1, 1));
		insert<float>(rotData, _index, _number, 0);
		insert<glm::vec4>(tintAlphaData, _index, _number, glm::vec4(1, 1, 1, 1));
	}

	void removeData(uint _index, uint _number)
	{
		remove(posData, _index, _number);
		remove(scaleData, _index, _number);
		remove(rotData, _index, _number);
		remove(tintAlphaData, _index, _number);
	}
};