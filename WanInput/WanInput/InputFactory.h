#pragma once

#include "InputInterface.h"

/* Input domain */
namespace Input
{
	/*	This is an input interface factory
		- use this for creating and deleting input instances */
	class InputAPI
	{
	public:
		/*	Instantiate a new input by type

			#_iVersion: version type request { RAW(Mouse+Keyboard), RAW(Gamepad) }

			#returns
				InputInterface* on success
				nullptr on invalid request
				nullptr on (target environment / type rquest) incompatability
				*/
		static __declspec(dllexport) InputInterface* createInput(IType _iType);

		/*	Shut down and delete input instance

			#_input: instance to be deleted */
		static __declspec(dllexport) void deleteInput(InputInterface* _input);
	};
}