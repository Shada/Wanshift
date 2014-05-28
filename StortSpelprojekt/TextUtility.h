#pragma once

#include "WanInput/Common.h"

/*	Check if string is empty
	
	#_text: text-string
	
	#returns
		true if string is empty
		false if string has elements */
bool isEmpty(std::string& _text)
{
	if(_text.size() > 0)
		return false;

	return true;
}

/*	Check if keycode holds a numerical or alphabetical value

	#_key: native keycode
	
	#returns
		true if keycode is numerical or alphabetical
		false if not*/
bool isKeyChar(const KeyCode& _key)
{
	if(_key >= KeyCode::A && _key <= KeyCode::Z || _key == KeyCode::SPACE || _key >= KeyCode::K0 && _key <= KeyCode::K9)
	{
		return true;
	}
	return false;
}

/*	Check if type of inputevent match expected value

	#_e: the retrieved inputevent
	#_type: the type of event expected
	
	#returns
		true if match
		false if not */
//bool isEvent(const InputEvent& _e, InputEventType _type)
//{
//	return _e.eventType == _type;
//}

/*	pop last character in a string, provided
	it has elements
	
	#_text: text-string to pop from*/
void popLastChar(std::string& _text)
{
	if(!isEmpty(_text))
		_text.pop_back();
}

/*	push character to string
	note: character is cast to lowercase
	
	#_kCode: native keycode provided
	#_text: text-string to be appended to*/
void addChar(const KeyCode& _kCode, std::string& _text)
{
	if(isKeyChar(_kCode))
	{
		_text += tolower((char)_kCode);
	}
}