#pragma once
#include "Container.h"

struct ShadowContainer : public Container
{
public:
	void addData(uint _index, uint _number) override {}
	void removeData(uint _index, uint _number) override {}

	int gridWidth;
	int gridHeight;
	std::string	textureName;
};

