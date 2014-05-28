#pragma once

#include "Common.h"
#include "InputInterface.h"

class MKService
	: public InputInterface
{
public:
	MKService(void);
	~MKService(void);

private:
	RAWINPUTDEVICE Rid[2];
	void initKeyMaps();
	void release()override;
	bool init(HWND _hWnd)override;
	InternalState pollInput(LPARAM _lparam)override;

	/*	update the internal keyboard
	
		#_rk: RAWKEYOBARD data */
	void addFrameKey(RAWKEYBOARD _rk);

	/* update the internal mouse 
	
		#_rm: RAWMOUSE data */
	void addFrameMouse(RAWMOUSE _rm);

	void frame()override;

	/* bind raw keys to internal keyboard
	
		#_iKey: keycode */
	void initKKey(KeyCode _iKey);

	/* initialize the mouse */
	void initMouse();

	/*	check whether the polled keys current state matches the param state &
		the old state != current state 
		
		#_key: keycode
		#_state: keystate to match 
		
		#returns
			true if current state == _state & old state != currentState
			false if else */
	bool isNewKeyPress(KeyCode _key, KeyState _state = KeyState::RELEASED)override;

	/*	check whether the polled keys current state matches the param state
	
		#_key: keycode
		#_state: keystate to match
		
		#returns
			true if current state == _state
			false if else */
	bool isContinousKeyPress(KeyCode _key)override;

	/*	check whether the polled keys current state matches the param state &
		the old state != current state 
		
		#_key: keycode
		#_state: keystate to match 
		
		#returns
			true if current state == _state & old state != currentState
			false if else */
	bool isNewMouseKeyPress(MouseKeyCode _key, KeyState _state = KeyState::PRESSED)override;

	/*	check whether the polled keys current state matches the param state
	
		#_key: keycode
		#_state: keystate to match
		
		#returns
			true if current state == _state
			false if else */
	bool isContinuousMouseKeyPress(MouseKeyCode _key, KeyState _state = KeyState::PRESSED)override;

	/*	check whether the current mouse position differs from the old mouse position
	
		#returns
			true if current relative position != 0
			false if else */
	bool isNewMousePosition()override;

	/*	get mouse wheel delta
	
		#returns wheel delta struct */
	WHEELDELTA getWheelDelta()override;
	
	/*	get mouse position
	
		#returns mouse position struct */
	MOUSEPOSITION getMousePosition()override;
};

