#pragma once
#include "Console.h"

class MainState;
/*	This is a subdomain interface for the debugger,
	a derived instance holds internal data and connections 
	to components of interest
	*/
class DebugDomain
{
public:
	DebugDomain();
	virtual ~DebugDomain();

	bool initialize(Console* _console, MainState& _mainState);



	/*	Receive a command from the debugger

		#_command: holds commandName and arg separated by ' ' delimiter
		(example: _command = "do something")

		#_console: debug text console
		#returns
		true if command is recognized
		false if command is unrecognized
		false if command arg is unrecognized*/
	virtual bool command(std::string& _command) = 0;

	/*	Update internal data and console text

		#_console: debug text console */
	virtual void update(Console* _console) = 0;

	/*	Draw domain specific data to the console
	
		#_console: debug text console */
	virtual void draw(Console* _console) = 0;

protected:

	/*	Initialize any derived data and allocate
	texts for printing
	note: I'm sorry but text allocation is a chore
	for now, will be mended

	#_console: debug text console that holds access
	to textContainer and other relevant
	printing data*/
	virtual bool init(Console* _console, MainState& _mainState) = 0;

	unsigned int	textStart;
	unsigned int	numTexts;
	Containers*		containers;

	int backBufferRTV;
};