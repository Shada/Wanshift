#define _VARIADIC_MAX 10
#ifdef __DEBUG
#include <gtest/gtest.h>
#endif
#include "MainState.h"

#define versionDev

/* this hides the main(...) */
#ifdef versionDev
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif


/* Forward Declarations */
LRESULT CALLBACK WndProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);
int mainLoop();

/* Global Variables */
MainState ms;
#ifdef __DEBUG
/* Tests */
TEST(main, trivial)
{
	EXPECT_EQ(true, true);
}
#endif

//TEST(ms, stateTest)
//{
//	int t = ms.init();
//	ASSERT_EQ(t,1);
//}

typedef void(*dllptr)();
/* entry point for application */
int main(int argc, char* argv[])
{
#ifdef __DEBUG
	if( argc != 0 && argv[0] != "--run" )
	{
		testing::InitGoogleTest(&argc, argv);
		int t = RUN_ALL_TESTS();
		if (t == 1)
		{
			std::getchar();
			return 1;
		}
	}
#endif

	// fire up system main loop

	bool result = ms.init();
	if(!result)
		return 1;

	int exitCode = mainLoop();

	ms.shutDown();

	return exitCode;
}

/* Main loop - will keep spinning until WM_QUIT message occurs */
int mainLoop()
{
	printf("start application\n");

	MSG msg;
	msg.message = WM_NULL;
	PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);

	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			ms.run();
		}
	}

	printf("close application\n");

	return (int)msg.wParam;
}

/* System message pump */
LRESULT CALLBACK WndProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
{
	switch(_msg)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		default:
		{
			return DefWindowProc(_hwnd, _msg, _wparam, _lparam);
		}
	}
}
