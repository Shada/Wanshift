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

/* a character available in the font (for example ' A ')*/
struct Character
{
	uint posx;	// x coordinate in the texture in pixels
	uint posy;	// y coordinate in the texture in pixels
	uint width;	// width of the character in pixels
};

/* A container for Font data */
struct FontContainer : Container
{
	// image indices
	std::vector< uint >	imageIndices;
	//width of each image
	std::vector< uint > imageWidths;
	//height of each image
	std::vector< uint > imageHeights;
	//spacing between each character
	std::vector< int >	spacings;
	//height of letters
	std::vector< int >	letterHeights;
	/*	the different sets of characters.
		Each font has one character set, and each character set has a bunch of available characters.
		usually the layout is as following for each set:
		!”#$%&’()*+,-./0123456789:;<=>?@
		ABCDEFGHIJKLMNOPQRSTUVWXYZ
		[\]^_`abcdefghijklmnopqrstuvwxyz{|}~*/
	std::vector< std::vector< Character > > characterSets;

private:
	void addData(uint _index, uint _number)
	{
		insert<uint>(imageIndices, _index, _number, 0);
		insert<uint>(imageWidths, _index, _number, 0);
		insert<uint>(imageHeights, _index, _number, 0);
		insert<int>(spacings, _index, _number, 0);
		insert<int>(letterHeights, _index, _number, 0);
	}

	void removeData(uint _index, uint _number)
	{
		remove(imageIndices, _index, _number);
		remove(imageWidths, _index, _number);
		remove(imageHeights, _index, _number);
		remove(spacings, _index, _number);
		remove(letterHeights, _index, _number);
	}
};