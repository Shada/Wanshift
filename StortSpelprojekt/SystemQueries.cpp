#include "SystemQueries.h"
#include <Windows.h>
#include <Psapi.h>
#include "MainState.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "pdh.lib")

SystemQueries::SystemQueries()
	: DebugDomain()
{
}


SystemQueries::~SystemQueries()
{
}

bool SystemQueries::init(Console* _console, MainState& _mainState)
{
	if(_console == nullptr)
		return false;
		
	textStart = 4;
	numTexts = 4;

	for(unsigned int i = 0; i < numTexts; ++i)
		_console->createText(30, textStart + i);

	fps = 0;
	count = 0;
	startTime = timeGetTime();

	return true;
}

void SystemQueries::update(Console* _console)
{
	/* FPS Query */
	count++;
	if(timeGetTime() > (startTime + 1000))
	{
		fps = count;
		count = 0;
		startTime = timeGetTime();
	}

	/* CPU Query */
	/* lol no */

	/* RAM Query */
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T mem = pmc.PrivateUsage;
	ramTotal = (int)mem / 1024 / 1024;
	ramPeak = (int)pmc.PeakWorkingSetSize / 1024 / 1024;

	_console->loadText("System Debug", textStart, 0);
	_console->loadText("RAM Total: " + std::to_string(ramTotal) + " MB", textStart + 1, 0);
	_console->loadText("RAM Peak: " + std::to_string(ramPeak) + " MB", textStart + 2, 0);
	_console->loadText("FPS:" + std::to_string(fps), textStart + 3, 0);
}

bool SystemQueries::command(std::string& _command)
{
	printf("@System Debug\nLOL no [%s] command!\n", _command.c_str());

	return false;
}

void SystemQueries::draw(Console* _console)
{
	for(uint i = 0; i < numTexts; i++)
	{
		int index = i + textStart;
		int indy = 5 + i * 20;
		_console->draw(index, glm::vec2(5, indy), glm::vec4(1.f, 0.f, 0.f, 1.f));
	}
}