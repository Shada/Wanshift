#pragma once

#include <vector>
#include "Container.h"
#include "glm\glm.hpp"

namespace Biom
{
	const byte Desert = 1;
	const byte Mountain = 2;
	const byte Meadow = 3;
};
struct BiomsContainer : Container
{
	std::vector < glm::vec2 > desertPoints, mountainPoints, meadowPoints, swampPoints, forestPoints;

	/* Storms, there can only be two */
	struct
	{
		glm::vec2 position, direction;
		float speed, size;
	} rainStorm, desertStorm;

	enum { cloudSize = 64*3 };
	/* The cloud map to send to the graphic card for imposter generation */
	std::array< std::array< float, cloudSize >, cloudSize > bufferCloudMap;

	/* Contains values between 0 and 1 where 0 is no cloud and 1 is largest possible cloud.
		It covers the entire map */
	std::array< std::array< float, cloudSize >, cloudSize > cloudMap;

	/* A non cropped perlin map for clouds, just used to generate bufferCloudMap depending on bioms and weather */
	std::array< std::array< float, cloudSize >, cloudSize > baseCloudMap;
	glm::vec2 cloudMapPosition;

	struct Clouds
	{
		glm::vec2 position;
		float radius;
		/* Between 0 and 1 where 0 is no strength and 1 is max */
		float strenght;
		bool raining;
	};
	/* A list of points where there are clouds */
	std::array< Clouds, 10 > clouds;

	void addData(uint _index, uint _number) override {}
	void removeData(uint _index, uint _number) override {}
};
