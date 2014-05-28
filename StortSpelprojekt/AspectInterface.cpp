#include "AspectInterface.h"
#include "MainState.h"

MessageSystem AspectInterface::messages = MessageSystem();
AspectInterface::AspectInterface(std::string _name, int _updateFrequency)
		: name(_name), updateFrequency(_updateFrequency), updateThreshold((float)_updateFrequency)
{
	timeSinceLastUpdate = 0;
}


AspectInterface::~AspectInterface()
{
}

std::string AspectInterface::getName()const
{
	return name;
}

int AspectInterface::getUpdateFrequency()const
{
	return updateFrequency;
}

bool AspectInterface::run(float _dt)
{
	updateThreshold -= _dt;
	timeSinceLastUpdate += _dt;

	if((int)updateThreshold > 0)
		return false;

	updateThreshold += updateFrequency;

	runAspect(timeSinceLastUpdate);
	timeSinceLastUpdate = 0;

	return true;
}