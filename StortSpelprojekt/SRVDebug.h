#pragma once
#include "DebugDomain.h"
#include <iostream>
#include "ShaderClass.h"
#include "RenderInterface.h"

class SRVDebug :
	public DebugDomain
{
public:
	SRVDebug();
	~SRVDebug();

	bool init(Console* _console, MainState& _mainState);

	bool command(std::string& _command);

	void update(Console* _console);

	void draw(Console* _console);

private:
	bool onShow(const std::string& _subject, int& _index);

	int srvIndex;
	DefaultVertex input;

	RenderInterface* graphics;

	MainState*		mainState;

	ShaderClass *shaders;
	int bufferIndex;
};

