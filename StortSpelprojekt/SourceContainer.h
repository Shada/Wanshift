#pragma once

#include "Container.h"

struct EnergySourceContainer : Container
{
	std::vector<float> energy;
	std::vector<glm::vec3> position;
	std::vector<glm::vec3> rotation;
	std::vector<glm::vec3> scale;

	void addData(uint _index, uint _number) override
	{
		insert<float>(energy, _index, _number, 25.f);
		insert<glm::vec3>(position, _index, _number, glm::vec3());
		insert<glm::vec3>(rotation, _index, _number, glm::vec3());
		insert<glm::vec3>(scale, _index, _number, glm::vec3());
	}

	void removeData(uint _index, uint _number) override
	{
		remove(energy, _index, _number);
		remove(position, _index, _number);
		remove(rotation, _index, _number);
		remove(scale, _index, _number);
	}
};