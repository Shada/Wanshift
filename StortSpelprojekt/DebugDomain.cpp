#include "DebugDomain.h"
#include "MainState.h"

DebugDomain::DebugDomain()
{
}


DebugDomain::~DebugDomain()
{
}

bool DebugDomain::initialize(Console* _console, MainState& _mainState)
{	
	containers = &_mainState.c;

	backBufferRTV = _mainState.graphics->getHandleIndex("backBufferRTV",ResourceType::RenderTargetView);

	return init(_console, _mainState);
}