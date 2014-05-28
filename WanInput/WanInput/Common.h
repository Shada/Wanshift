#pragma once
#include <Windows.h>
#include <memory>
#include <vector>
#define TWOPI (3.141592 * 2)


enum class KeyState
{
	DISABLED_FUNCTIONALITY = -1,
	PRESSED = 0,
	RELEASED = 1,
	UNINITIALIZED = 2,
};

enum class MouseKeyCode
{
	UNDEFINED	= 0,
	LEFT		= 1,
	RIGHT		= 2,
	MIDDLE		= 3,
	B4			= 4,
	B5			= 5,
	WHEEL		= 6,
};

/* flag for internal state querying */
enum class InternalState
{
	UNDEFINED = -1,
	ALL_OK = 0,
	EVENT_BUFFER_OVERFLOW,
	NO_POLL_INPUT,
	FAULTY_POLL_INPUT,
	NO_FOCUS,
};

/* Type of input device(s) */
enum class IType
{
	UNDEFINED,
	RAW_KEYBOARD_MOUSE,
	RAW_GAMEPAD,
};

enum class InputEventType;
enum class KeyCode;

enum class MouseKey
{
	UNDEFINED	= 0,
	LEFT_DOWN	= 0x0001,
	LEFT_UP		= 0x0002,
	MIDDLE_DOWN	= 0x0010,
	MIDDLE_UP	= 0x0020,
	RIGHT_DOWN	= 0x0004,
	RIGHT_UP	= 0x0008,
	B4_DOWN		= 0x0040,
	B4_UP		= 0x0080,
	B5_DOWN		= 0x0100,
	B5_UP		= 0x0200,
	WHEEL		= 0x0400,
};

struct MKey
{
	MouseKeyCode key;
	KeyState	 state;
};
  
/* Raw keyboard input event */
//struct KeyEvent
//{
//	KeyCode		key;
//	KeyState	state;
//};

/* Raw gamepad move event */
struct GamepadMoveEvent
{
};

/* Raw gamepad button event */
struct GamepadButtonEvent
{
};

enum class KeyCode	//105 keyboard buttons +1(unknown)
{
	/// Unknown key. Usually an error if returned.
	UNKNOWN = -1,																							//1
	BACKSPACE = 0x08, 
	TAB, 
	RETURN = 0x0D, 
	BREAK = 0x13, 
	CAPS_LOCK, 
	ESCAPE = 0x1B, 
	SPACE = 0x20,				
	PAGE_UP, 
	PAGE_DOWN, 
	END, 
	HOME, 
	LEFT_ARROW, 
	UP_ARROW, 
	RIGHT_ARROW, 
	DOWN_ARROW ,							
	SINGLE_QUOTE = 0x27, 
	COMMA = 0x2C, 
	DASH = 0x2D, 
	PERIOD = 0x2E, 
	SLASH = 0x2F,							
	K0, K1, K2, K3, K4, K5, K6, K7, K8, K9,																	//10
	SEMICOLON = 0x3B, 
	EQUAL = 0x3D,																			
	A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,					//26
	LEFT_SQUARE_BRACKET, 
	BACK_SLASH, 
	RIGHT_SQUARE_BRACKET, 
	BACK_QUOTE, 
	DEL = 0x7F,							
	LEFT_SHIFT, 
	RIGHT_SHIFT, 
	LEFT_CTRL, 
	RIGHT_CTRL, 
	LEFT_ALT, 
	RIGHT_ALT, 
	INSERT, 
	LEFT_SUPER, 
	RIGHT_SUPER,	
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,														//12
	SYS_RQ, 
	SCROLL_LOCK, 
	NUM_LOCK, 
	WORLD_1, 
	WORLD_2, 
	MENU,													
	NP_DIVIDE, NP_MULTIPLY, NP_MINUS, NP_PLUS, NP_RETURN, NP_PERIOD,										//6
	NP0, NP1, NP2, NP3, NP4, NP5, NP6, NP7, NP8, NP9,														//10
	TILDE,
};

struct KEYSTATE
{
	KeyState	cState;// current state
	KeyState	oState;	// old state
};

struct MOUSEKEYSTATE
{
	KeyState			cState;	// current state
	KeyState			oState;	// old state
};

struct MOUSEPOSITION
{
	long crX, crY; // current relative {x:y}
	long orX, orY; // old relative {x:y}	
};

struct WHEELDELTA
{
	float cWheelDelta;	// current
	float oWheelDelta;  // old
	float coDif;		// current/old difference
};


