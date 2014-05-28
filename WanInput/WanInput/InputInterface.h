#pragma once

#include <Windows.h>
#include "Common.h"
#include <map>

class InputInterface
{
public:
	InputInterface();
	virtual ~InputInterface();

	/*	Initialize windows specific input

		#_hWnd: window handle

		#returns
			true if member inits succeeds
			false if member inits fails */
	virtual bool init(HWND _hWnd) = 0;

	/*	Release member data */
	virtual void release();

	/*	Read windows message pump params */
	virtual InternalState pollInput(LPARAM _lparam) = 0;

	/*	update the states of the internal input devices
		(ie. update key status etc)
		*/
	virtual void frame() = 0;

	/*	check if polled keys current status has newly changed to the sought after state
		while its old status differs

		#_key: the polled key
		#_state: the sought after state of the polled key

		#returns:
		true if the key has newly obtained the param state
		false if param state differs or currenstate == oldstate	*/
	virtual bool isNewKeyPress(KeyCode _key, KeyState _state = KeyState::PRESSED) = 0;

	/*	check if polled keys current status has the sought after state

		#_key: the polled key
		#_state: the sought after state of the polled key

		#returns:
			true if the keys current status holds the param state
			false if not	*/
	virtual bool isContinousKeyPress(KeyCode _key) = 0;

	/*	check if polled keys current status has newly changed to the sought after state
		while its old status differs

		#_key: the polled key
		#_state: the sought after state of the polled key

		#returns:
		true if the key has newly obtained the param state
		false if param state differs or currenstate == oldstate	*/
	virtual bool isNewMouseKeyPress(MouseKeyCode _key, KeyState _state = KeyState::PRESSED) = 0;

	/*	check if polled keys current status has the sought after state

		#_key: the polled key
		#_state: the sought after state of the polled key

		#returns:
			true if the keys current status holds the param state
			false if not	*/
	virtual bool isContinuousMouseKeyPress(MouseKeyCode _key, KeyState _state = KeyState::PRESSED) = 0;

	/*	check if the mouse has changed its position since last frame

		#returns
			true if mouse relative position != 0
			false if mouse relative position == 0	*/
	virtual bool isNewMousePosition() = 0;

	/*	get the wheel delta

		#returns mouse wheel delta struct	*/
	virtual WHEELDELTA getWheelDelta() = 0;

	/*	get mouse position

		#returns mouse position struct */
	virtual MOUSEPOSITION getMousePosition() = 0;

	/*	check if any keyboard key has been pressed during the frame 
	
		#_chars: inout string to fetch frame keys, provided any
					key has been pressed during the frame 
					
		#returns
			true if any key has been pressed during the frame
			false if else */
	virtual bool InputInterface::hasFrameKeys(std::string& _chars);

protected:
	/*	translate native raw values to internal keycodes
	
		#_key: native raw value
		
		#returns matching internal keycode */
	KeyCode	getKeyFromNative(unsigned short _key);

	InternalState	internalState;
	//std::vector<KeyCode> frameKeys;
	std::string frameKeys;
};
