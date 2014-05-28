#include "MKService.h"
#include <map>


typedef std::map<KeyCode, KEYSTATE> KEYBOARD;
KEYBOARD keyboard;

typedef std::map<MouseKeyCode, KEYSTATE> MKEYS;

typedef struct
{
	MKEYS			keys;
	WHEELDELTA		wheelDelta;
	MOUSEPOSITION	mousePosition;
}MOUSE;

MOUSE mouse;

MouseKeyCode nativeToInternalMouseKey(USHORT _key)
{
	MouseKeyCode key;
	switch(_key)
	{
	case 0x0001:
	case 0x0002:
		{
			key = MouseKeyCode::LEFT;
		}
		break;
	case 0x0004:
	case 0x0008:
		{
			key = MouseKeyCode::RIGHT;
		}
		break;
	case 0x0010:
	case 0x0020:
		{
			key = MouseKeyCode::MIDDLE;
		}
		break;
	case 0x0040:
	case 0x0080:
		{
			key = MouseKeyCode::B4;
		}
		break;
	case 0x0100:
	case 0x0200:
		{
			key = MouseKeyCode::B5;
		}
		break;
	case 0x0400:
		{
			key = MouseKeyCode::WHEEL;
		}
		break;
	default:
		{
			key = MouseKeyCode::UNDEFINED;
		}
		break;
	}
	return key;
}
void initMKey(USHORT _iKey)
{
	KEYSTATE k;
	k.cState			= KeyState::RELEASED;
	k.oState			= KeyState::UNINITIALIZED;
	
	MouseKeyCode key = nativeToInternalMouseKey(_iKey);
	mouse.keys[key] = k;
}


MKService::MKService(void)
{
	
}

MKService::~MKService(void)
{
}

void MKService::release()
{

}

void MKService::initKKey(KeyCode _iKey)
{
	KEYSTATE k;
	k.cState		= KeyState::RELEASED;
	k.oState		= KeyState::PRESSED;

	keyboard[_iKey]	= k;
}

void MKService::initMouse()
{
	mouse.mousePosition.crX		= 0;
	mouse.mousePosition.crY		= 0;
	mouse.mousePosition.orX		= 0;
	mouse.mousePosition.orY		= 0;

	mouse.wheelDelta.cWheelDelta= 0;
	mouse.wheelDelta.oWheelDelta= 0;
	mouse.wheelDelta.coDif		= 0;

	for(USHORT i = 0; i < 12; i++)
	{
		initMKey(i);
	}
}

bool MKService::isNewKeyPress(KeyCode _key, KeyState _state)
{
	return keyboard[_key].cState == _state && keyboard[_key].cState != keyboard[_key].oState;
}

bool MKService::isContinousKeyPress(KeyCode _key)
{
	bool res = keyboard[_key].cState == KeyState::PRESSED;
	return res;
}

WHEELDELTA MKService::getWheelDelta()
{
	return mouse.wheelDelta;
}

bool MKService::isNewMouseKeyPress(MouseKeyCode _key, KeyState _state)
{
	bool res = mouse.keys[_key].cState == _state && mouse.keys[_key].cState != mouse.keys[_key].oState;
	return res;
}

bool MKService::isContinuousMouseKeyPress(MouseKeyCode _key, KeyState _state)
{
	bool res = mouse.keys[_key].cState == _state ;//&& mouse.keys[_key].cState == mouse.keys[_key].oState;
	return res;
}

bool MKService::isNewMousePosition()
{
	bool res = mouse.mousePosition.crX != 0 && mouse.mousePosition.crY != 0;
	return res;
}

MOUSEPOSITION MKService::getMousePosition()
{
	return mouse.mousePosition;
}

bool MKService::init(HWND _hWnd)
{
	/* register mouse + keyboard  */

	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x06;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = _hWnd;
	Rid[1].usUsagePage = 0x01;
	Rid[1].usUsage = 0x02;
	Rid[1].dwFlags = RIDEV_INPUTSINK;
	Rid[1].hwndTarget = _hWnd;

	if(RegisterRawInputDevices(Rid,2,sizeof(Rid[0])) != 0)
	{
		initKeyMaps();
		initMouse();
		return true;
	}

	return false;
}

void MKService::initKeyMaps()
{
	for( USHORT i = 0; i < 105; i++)
	{
		KeyCode key = InputInterface::getKeyFromNative(i);
		initKKey(key);
	}

	for( USHORT i = 0; i < 12; i++)
	{
		//MouseKey key = static_cast<MouseKey>(i);
		initMKey(i);
	}
}

void MKService::frame()
{
	/* iterate over keyboard setting old to current */
	KEYBOARD::iterator kIt = keyboard.begin();
	while(kIt != keyboard.end())
	{
		KEYSTATE k = kIt->second;
		k.oState = k.cState;
		kIt->second = k;

		kIt++;
	}

	/* iterate over mouse setting old to current */
	MKEYS::iterator mIt = mouse.keys.begin();
	while(mIt != mouse.keys.end())
	{
		KEYSTATE k = mIt->second;
		k.oState = k.cState;
		mIt->second = k;

		mIt++;
	}

	/* reset wheel */
	WHEELDELTA wd	= mouse.wheelDelta;
	wd.coDif		= wd.oWheelDelta - wd.cWheelDelta;
	wd.oWheelDelta	= wd.cWheelDelta;
	wd.cWheelDelta	= 0;
	mouse.wheelDelta= wd;

	/* reset mouse position */
	MOUSEPOSITION mp	= mouse.mousePosition;
	mp.orX = mp.crX;
	mp.orY = mp.crY;
	mp.crX = 0;
	mp.crY = 0;
	mouse.mousePosition = mp;

	frameKeys.clear();
}

void MKService::addFrameKey(RAWKEYBOARD _rk)
{
	KeyCode key = InputInterface::getKeyFromNative(_rk.VKey);
	KEYSTATE c = keyboard[key];
	c.oState = c.cState;
	c.cState = (KeyState)_rk.Flags;
	keyboard[key] = c;

	if(c.cState == KeyState::PRESSED)
	{
		if(keyboard[KeyCode::LEFT_SHIFT].cState == KeyState::PRESSED)
		{
			frameKeys += (char)key;
		}
		else
		{
			frameKeys += (char)tolower((char)key);
		}
	}
}

void MKService::addFrameMouse(RAWMOUSE _rm)
{
	mouse.mousePosition.orX = mouse.mousePosition.crX;
	mouse.mousePosition.orY = mouse.mousePosition.crY;
	mouse.mousePosition.crX += _rm.lLastX;
	mouse.mousePosition.crY += _rm.lLastY;

	if(_rm.usButtonFlags > 0)
	{
		if(_rm.usButtonFlags & RI_MOUSE_WHEEL)
		{
			short d = ((short)(unsigned short)_rm.usButtonData)/WHEEL_DELTA;
			mouse.wheelDelta.coDif = mouse.wheelDelta.oWheelDelta - mouse.wheelDelta.cWheelDelta;
			mouse.wheelDelta.oWheelDelta = mouse.wheelDelta.cWheelDelta;
			mouse.wheelDelta.cWheelDelta = (float)d;
		}
		else
			mouse.wheelDelta.cWheelDelta = 0.0f;

		MouseKeyCode key = nativeToInternalMouseKey(_rm.usButtonFlags);
		mouse.keys[key].oState = mouse.keys[key].cState;
		mouse.keys[key].cState = (KeyState)_rm.usButtonFlags;
	}
}


InternalState MKService::pollInput(LPARAM _lparam)
{
	if( !GetFocus() )
		return InternalState::NO_FOCUS;

	UINT dwSize = 0;

    GetRawInputData((HRAWINPUT)_lparam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
    LPBYTE lpb = new BYTE[dwSize];
    if (lpb == nullptr) 
    {
		return InternalState::NO_POLL_INPUT;	// no input
    } 
	
    if (GetRawInputData((HRAWINPUT)_lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize )
		return InternalState::FAULTY_POLL_INPUT;	// faulty size

    RAWINPUT* raw = (RAWINPUT*)lpb;

    if (raw->header.dwType == RIM_TYPEKEYBOARD) 
    {	
		addFrameKey(raw->data.keyboard);
    }
    else if (raw->header.dwType == RIM_TYPEMOUSE) 
    {
		addFrameMouse(raw->data.mouse);
    } 

	 delete[] lpb; 

	 return InternalState::ALL_OK;
}