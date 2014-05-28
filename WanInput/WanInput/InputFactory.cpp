#include "InputFactory.h"
//#include "RawMK.h"
#include "MKService.h"

namespace Input
{
	InputInterface* InputAPI::createInput(IType _iType)
	{
		switch(_iType)
		{
		case IType::RAW_KEYBOARD_MOUSE:
			{
			InputInterface* input = new MKService();
			return input;
			}
		case IType::RAW_GAMEPAD:
		case IType::UNDEFINED:
		default:
			return nullptr;
		}

		/* seriously you should never reach this point */
		return nullptr;
	}

	void InputAPI::deleteInput(InputInterface* _input)
	{
		if(_input != nullptr)
		{
			_input->release();
			delete _input;
		}
	}
}