#pragma once

#include <Windows.h>

class Window
{
public:
	Window();
	~Window();

	/*	registers a window handle

		#_screenWidth:	width of the window
		#_screenHeight:	height of the window 

		#returns
			true for success
			false for failing to register class */
	bool createWindow(int _screenWidth, int _screenHeight, bool _fullScreen);

	/*	updates the windows message pump */
	void handleMessage();

	static LRESULT CALLBACK WndProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);

protected:
	HINSTANCE	hInstance;
	HWND		hWnd;
};