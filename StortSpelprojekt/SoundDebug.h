#pragma once

#include "DebugDomain.h"

class SoundDebug : public DebugDomain
{
public:
	SoundDebug();
	~SoundDebug();

	virtual bool command(std::string& _command)override;

	/*	Update internal data and console text

	#_console: debug text console */
	virtual void update(Console* _console)override;

	/*	Draw domain specific data to the console

	#_console: debug text console */
	virtual void draw(Console* _console)override;

	/*	Initialize any derived data and allocate
	texts for printing
	note: I'm sorry but text allocation is a chore
	for now, will be mended

	#_console: debug text console that holds access
	to textContainer and other relevant
	printing data*/
	virtual bool init(Console* _console, MainState& _mainState)override;
};

