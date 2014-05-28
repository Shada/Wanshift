#include "PostEffect.h"
#include "MainState.h"

PostEffect::PostEffect(std::string _shaderName)
	: name(_shaderName), active(false)
{
}


PostEffect::~PostEffect(void)
{
}

std::string PostEffect::getName()const
{
	return name;
}

void PostEffect::init(MainState& _mainState)
{
	graphics = _mainState.graphics;
	//computeShader = new ComputeShaderClass(name + "CS", graphics);
	
	//computeShaderIndex = graphics->createComputeShader(name);

	/* custom init goes in here */
	initPostEffect(_mainState);
}

void PostEffect::preProcess(float _dt)
{

}

void PostEffect::draw(float _dt, int _renderTarget)
{

}

void PostEffect::toggleActive(bool _isActive)
{
	active = _isActive;
}

bool PostEffect::isActive()
{
	return active;
}