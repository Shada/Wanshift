#pragma once

#include <Pdh.h>
#include "DebugDomain.h"

/*	This debug domain queries the system fir
	RAM usage and FPS
	
	it also lols if you try to give it a command */
class SystemQueries
	: public DebugDomain
{
public:
	SystemQueries();
	~SystemQueries();

private:

	int getFpsCount();
	int getCpuPercentage();
	int getTotalProcessRam();
	int getPeakProcessRam();

	bool init(Console* _console, MainState& _mainState)		override;
	bool command(std::string& _command)	override;
	void update(Console* _console)		override;
	void draw(Console* _console)		override;

/* FPS Counters */
	int				fps;
	int				count;
	unsigned long	startTime;

/* RAM Counters */
	int ramTotal;
	int ramPeak;
	int ramSet;

	friend class Debugger;
};
