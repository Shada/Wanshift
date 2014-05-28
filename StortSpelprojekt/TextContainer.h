#pragma once

#ifdef __DEBUG
#include <gtest\gtest.h>
#endif
#include <vector>
#include <utility>
#include <numeric>
#include "Container.h"

typedef unsigned int uint;

//each letter forms a billboard in the geometry shader
struct Letter
{
	float x;				//letter x position
	glm::vec4 texCoords;	//letter texture coordinates
};

/* Contains text string, text data, max size, actual size, and buffer index. */
struct TextContainer : Container
{
	// text string
	//std::vector< std::string >	texts;
	uint						textBufferIndex;
	// the start index for the text in the buffer
	std::vector< uint >			bufferIndices;
	// the allocated size for the text in the buffer
	std::vector< uint >			maxSizes;
	// the actual size the text uses in the buffer
	std::vector< uint >			sizes;
	/* the text data.
	(NOTICE: Not to be mapped to each text. Use bufferIndices to get the start index of letters,
	and maxSizes for the allocated size in the letters vector for each text.) */
	std::vector< Letter >		letters;
	/* the index of the font used by this text */
	std::vector< uint >			fontIndices;

private:
	// See Container.h for documentation
	void addData(uint _index, uint _number)
	{
		//insert<std::string>( texts, _index, _number, "" );
		insert<uint>(bufferIndices, _index, _number, 0);
		insert<uint>(maxSizes, _index, _number, 0);
		insert<uint>(sizes, _index, _number, 0);
		//insert<Letter>( letters, _index, _number, NULL );
		insert<uint>(fontIndices, _index, _number, 0);
	}

	// See Container.h for documentation
	void removeData(uint _index, uint _number)
	{
		//remove( texts, _index, _number );
		remove(bufferIndices, _index, _number);
		remove(maxSizes, _index, _number);
		remove(sizes, _index, _number);
		//remove( letters, _index, _number );
		remove(fontIndices, _index, _number);
	}
};