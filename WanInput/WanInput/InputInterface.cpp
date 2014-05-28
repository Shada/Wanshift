#include "InputInterface.h"
#include <map>

/* Max allowed size of input buffer */
static const unsigned int MAX_EVENT_BUFFER_SIZE = 100;

std::map<unsigned short, KeyCode> keyMapFromNative;
std::map<KeyCode, unsigned short> keyMapToNative;

void initKeyMaps();

void insertKey(unsigned short _nativeKey, KeyCode _iKey)
{
	keyMapFromNative[_nativeKey] = _iKey;
	keyMapToNative[_iKey] = _nativeKey;
}



InputInterface::InputInterface(void)
	: internalState(InternalState::ALL_OK)
{
	initKeyMaps();
}

InputInterface::~InputInterface(void)
{
	
}

void InputInterface::release()
{
	keyMapFromNative.clear();
	keyMapToNative.clear();
};

KeyCode InputInterface::getKeyFromNative(unsigned short _key)
{
	return keyMapFromNative[_key];
}

void initKeyMaps()
{
	//add alphabet
	for(char i='A'; i<= 'Z'; i++)
	{
		insertKey(i,(KeyCode)i);
	}
	//add numbers 
	//note: windows makes no different between left/right keys for certain virtuals
	//- check scancodes for left/right
	for(char i = '0'; i <= '9'; i++)
	{
		insertKey(i,(KeyCode)i);
	}

	//add virtual keys
	insertKey(VK_OEM_7,		KeyCode::SINGLE_QUOTE);
	insertKey(VK_OEM_102,	KeyCode::BACK_SLASH);
	insertKey(VK_BACK,		KeyCode::BACKSPACE);
	insertKey(VK_CAPITAL,	KeyCode::CAPS_LOCK);
	insertKey(VK_OEM_COMMA, KeyCode::COMMA);
	insertKey(VK_DELETE,	KeyCode::DEL);
	insertKey(VK_DOWN,		KeyCode::DOWN_ARROW);
	insertKey(VK_END,		KeyCode::END);
	insertKey(VK_RETURN,	KeyCode::RETURN);
	insertKey(VK_OEM_PLUS,	KeyCode::EQUAL);
	insertKey(VK_ESCAPE,	KeyCode::ESCAPE);
	insertKey(VK_F1,		KeyCode::F1);
	insertKey(VK_F2,		KeyCode::F2);
	insertKey(VK_F3,		KeyCode::F3);
	insertKey(VK_F4,		KeyCode::F4);
	insertKey(VK_F5,		KeyCode::F5);
	insertKey(VK_F6,		KeyCode::F6);
	insertKey(VK_F7,		KeyCode::F7);
	insertKey(VK_F8,		KeyCode::F8);
	insertKey(VK_F9,		KeyCode::F9);
	insertKey(VK_F10,		KeyCode::F10);
	insertKey(VK_F11,		KeyCode::F11);
	insertKey(VK_F12,		KeyCode::F12);
	insertKey(VK_OEM_3,		KeyCode::BACK_QUOTE);
	insertKey(VK_HOME,		KeyCode::HOME);
	insertKey(VK_INSERT,	KeyCode::INSERT);
	insertKey(VK_NUMPAD0,	KeyCode::NP0);
	insertKey(VK_NUMPAD1,	KeyCode::NP1);
	insertKey(VK_NUMPAD2,	KeyCode::NP2);
	insertKey(VK_NUMPAD3,	KeyCode::NP3);
	insertKey(VK_NUMPAD4,	KeyCode::NP4);
	insertKey(VK_NUMPAD5,	KeyCode::NP5);
	insertKey(VK_NUMPAD6,	KeyCode::NP6);
	insertKey(VK_NUMPAD7,	KeyCode::NP7);
	insertKey(VK_NUMPAD8,	KeyCode::NP8);
	insertKey(VK_NUMPAD9,	KeyCode::NP9);
	insertKey(VK_ADD,		KeyCode::NP_PLUS);
	insertKey(VK_DECIMAL,	KeyCode::NP_PERIOD);
	insertKey(VK_DIVIDE,	KeyCode::NP_DIVIDE); 
	//insertKey(VK_KP_ENTER, KeyCode::NP_RETURN);
	insertKey(VK_MULTIPLY,	KeyCode::NP_MULTIPLY);
	insertKey(VK_SUBTRACT,	KeyCode::NP_MINUS);
	insertKey(VK_LEFT,		KeyCode::LEFT_ARROW);
	insertKey(VK_MENU,		KeyCode::LEFT_ALT);
	insertKey(VK_OEM_4,		KeyCode::LEFT_SQUARE_BRACKET);
	insertKey(VK_CONTROL,	KeyCode::LEFT_CTRL);
	insertKey(VK_SHIFT,		KeyCode::LEFT_SHIFT);
	insertKey(VK_LWIN,		KeyCode::LEFT_SUPER);
	insertKey(VK_MENU,		KeyCode::MENU);
	insertKey(VK_OEM_MINUS, KeyCode::DASH);
	insertKey(VK_NUMLOCK,	KeyCode::NUM_LOCK);
	insertKey(VK_NEXT,		KeyCode::PAGE_DOWN);
	insertKey(VK_PRIOR,		KeyCode::PAGE_UP);
	insertKey(VK_PAUSE,		KeyCode::BREAK);
	insertKey(VK_OEM_PERIOD,KeyCode::PERIOD);
	insertKey(VK_PRINT,		KeyCode::SYS_RQ);
	insertKey(VK_RIGHT,		KeyCode::RIGHT_ARROW);
	//insertKey(VK_RIGHT_ALT,KeyCode::RIGHT_ALT);
	insertKey(VK_OEM_6,		KeyCode::RIGHT_SQUARE_BRACKET);
	//insertKey(VK_RCONTROL, KeyCode::RIGHT_CTRL);
	//insertKey(VK_RSHIFT, KeyCode::RIGHT_SHIFT);
	insertKey(VK_RWIN,		KeyCode::RIGHT_SUPER);
	insertKey(VK_SCROLL,	KeyCode::SCROLL_LOCK);
	insertKey(VK_OEM_1,		KeyCode::SEMICOLON);
	insertKey(VK_OEM_2,		KeyCode::SLASH);
	insertKey(VK_SPACE,		KeyCode::SPACE);
	insertKey(VK_TAB,		KeyCode::TAB);
	insertKey(VK_UP,		KeyCode::UP_ARROW);
	insertKey(VK_OEM_5,		KeyCode::WORLD_1);
	insertKey(VK_OEM_8,		KeyCode::WORLD_2);
	insertKey(VK_OEM_5,		KeyCode::TILDE); //not sure if kosher
}

bool InputInterface::hasFrameKeys(std::string& _chars)
{
	if(frameKeys.size() <= 0 || _chars.compare("") == 0)
		return false;

	_chars = frameKeys;

	return true;
}