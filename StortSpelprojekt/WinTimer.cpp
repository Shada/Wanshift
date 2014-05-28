#include "WinTimer.h"
#ifdef __DEBUG
#include <gtest/gtest.h>
#endif

WinTimer::WinTimer()
	: TimerInterface()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	ticksPerMs = (float)frequency / 1000.f;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
}

WinTimer::~WinTimer()
{
}

void WinTimer::frame()
{
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	timeDifference = (float)(currentTime - startTime);
	frameTime = timeDifference / ticksPerMs;
	startTime = currentTime;
}

float WinTimer::getTime()
{
	return frameTime;
}