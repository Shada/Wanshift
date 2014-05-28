#pragma once
#include "timerinterface.h"
#include <Windows.h>
/*	High resolution windows timer */
class WinTimer :
	public TimerInterface
{
public:
	WinTimer();
	~WinTimer();

	void	frame()override;
	float	getTime()override;

private:
	INT64	frequency;
	INT64	startTime;
	float	ticksPerMs;
	float	frameTime;

};