#pragma once

#include "DebugDomain.h"
#include "RenderInterface.h"

/*	This debug domain holds access to the renderer

	all it does for now is printing its name
	and lolling everytime you try to give it a command */
class RenderDebug
	: public DebugDomain
{
public:
	RenderDebug();
	~RenderDebug();

private:
	bool init(Console* _console, MainState& _mainState)	override;
	bool command(std::string& _command)	override;
	void update(Console* _console)		override;
	void draw(Console* _console)		override;

	RenderInterface*	graphics;
	int					textStart;
};