#include "Window.h"

/* temporary variables - move to appropriate place */
LPCSTR appName = "WAN";

Window::Window()
{
}


Window::~Window()
{
}

bool Window::createWindow(int _screenWidth, int _screenHeight, bool _fullScreen)
{
	/* fetch process hinstance */
	hInstance = GetModuleHandle(0);

	/* create & register default wndclassex */
	WNDCLASSEX wcx;
	wcx.cbClsExtra = 0;
	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.cbWndExtra = 0;
	wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcx.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcx.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wcx.hIconSm = wcx.hIcon;
	wcx.hInstance = hInstance;
	wcx.lpfnWndProc = WndProc;
	wcx.lpszClassName = appName;
	wcx.lpszMenuName = nullptr;
	wcx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	RegisterClassEx(&wcx);

	int winX, winY;
	winX = (GetSystemMetrics(SM_CXSCREEN) - _screenWidth) / 2;
	winY = (GetSystemMetrics(SM_CYSCREEN) - _screenHeight) / 2;

	hWnd = CreateWindowEx(WS_EX_APPWINDOW, appName, appName,
						  WS_OVERLAPPEDWINDOW, // WS_CLIPSIBLINGS |WS_CLIPCHILDREN | WS_POPUP,
						  winX, winY, _screenWidth, _screenHeight,
						  nullptr, nullptr, hInstance, nullptr);

	if(!hWnd)
		return false;

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	ShowCursor(true);

	return true;
}

void Window::handleMessage()
{
	MSG msg;
	while(GetMessage(&msg, nullptr, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}