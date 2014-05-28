#pragma once

#include "TimerInterface.h"
#include "WanInput\InputInterface.h"
#include "Containers.h"
#include "RenderInterface.h"
#include "GameplayVariables.h"
#include "Console.h"
#include "DebugDomain.h"
#include "SRVDebug.h"

class MainState;

typedef std::map<std::string,DebugDomain*> DDomain;

/**/
class Debugger
{
public:
	Debugger(void);
	~Debugger(void);

	/*	Initialize the debugger
		note: all params are required, no nulls!

		#_graphics: Renderinterface instance
		#_input: InputInterface instance

		#returns
		true if all params are valid
		false if any param is invalid*/
	bool init(MainState& _mainState);

	/*	Poll input and update all debug domains
		note: * no commands will be processed if debugger
		isn't set to visible.
		* only minimal update if accumulated time
		is less than updateThreshold

		#_dt: delta time

		#returns
		false when updateThreshold isn't met (minimal update)
		true when full update is performed */
	bool update(float _dt);

	/*	Debugger draws every frame it's visible */
	void draw();

private:

	/*	Switch between debug domains

		#_text: name of target domain

		#returns
			true if domain exist
			false if domain doesn't exist*/
	bool onDomainSwitch(std::string& _domainName);

	/*	Broadcast command to the domains
		note: should be unique, if any two, or more, domains share
		the same [command/arg] setup only the first one to catch the command
		will act on it

		#_command: holds commandName and arg separated by ' ' delimiter
		(example: _command = "make cake")
		#*/
	void onDomainCommand(std::string& _command);

	/*	Check if the polled key holds a command on release

		#_kCode: the native keycode provided by the	input poll

		cases:	* tilde - toggle visibility
		* return - send command
		* backspace - pop text
		* default - push text */
	void onKeyRelease(const KeyCode& _kCode);

	///* Process the key depending on its state (pressed/released)
	//
	//	#_key: the native keycode provided by the input poll*/
	//void onState(const KeyEvent& _key);

	/*	Determine whether a command is for the debug framework
		or the domains

		#_text: possible command, holds commandName and arg
		separated by ' ' delimiter
		(example: _text = "switch domainName") */
	void delegateCommand(std::string& _text);

	/*Adds text to backlog, if backlog is full it will be emptied at the end*/
	void addToBackLog(std::string _cmd);

	/**/
	bool onCommand(const std::string& _command, const std::string& _subject);

	bool			isVisible;
	Console			console;
	DebugDomain*	activeDomain;
	DDomain			domains;
	InputInterface* input;
	int				commandTextIndex;
	unsigned int	backLogIndex;
	float			updateThreshold;
	std::string		text;
	std::array<std::string, 10> backLog;
};
