#include "RenderInterface.h"


RenderInterface::RenderInterface(Version _dxVersion)
	: version(_dxVersion)
{

}

RenderInterface::~RenderInterface()
{
}

Version RenderInterface::getVersion()
{
	return version;
}

int RenderInterface::getAnswer()
{
	return 42;
}

float RenderInterface::getTotalVRAM()
{
	return 42;
}

float RenderInterface::getMemBuffers()
{
	return (float)getAnswer();
}

float RenderInterface::getMemShaders()
{
	return (float)getAnswer();
}

float RenderInterface::getMemTextures()
{
	return (float)getAnswer();
}

float RenderInterface::getMemBackBuffer()
{
	return (float)getAnswer();
}

float RenderInterface::memConsumption(float _size, UINT _width, UINT _height)
{
	return _size * (float)(_width * _height);
}

float RenderInterface::memConsumption(float _size, UINT _amount)
{
	return _size * (float)_amount;
}