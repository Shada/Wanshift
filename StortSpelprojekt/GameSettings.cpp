#include "GameSettings.h"
#include "MainState.h"

GameSettings::GameSettings(void)
{
}


GameSettings::~GameSettings(void)
{
}

bool GameSettings::init(Console* _console, MainState& _mainState)
{
	containersPtr = &_mainState.c;
	graphicsPtr = _mainState.graphics;

	return true;
}

bool GameSettings::command(std::string& _command)
{
	bool isMyCommand = false;
	char delimiter = ' ';
	int sep = _command.find_first_of(delimiter);
	if(sep <= 0)
		return false;

	std::string key, val;
	key = _command.substr(_command.find_first_not_of(delimiter), sep);
	val = _command.substr(sep+1, _command.find_last_not_of(delimiter));

	if(key == "reset")
	{
		isMyCommand = true;

		if(val == "atm")
		{
			AtmosphereContainer& ac = *containersPtr->get<AtmosphereContainer>(Containers::ATMOSPHERE_SIM);
			ac.setData("WorldVariables");	
			printf("atmosphere reset\n");
		}
		else if(val == "fog")
		{
			FogContainer& fc = *containersPtr->get<FogContainer>(Containers::FOG_SIM);
			fc.setData("WorldVariables");
			printf("fog reset\n");
		}

		
	}

	return isMyCommand;
}

void GameSettings::update(Console* _console)
{
	AtmosphereContainer& ac = *containersPtr->get<AtmosphereContainer>(Containers::ATMOSPHERE_SIM);
	ac.setData("WorldVariables");

	FogContainer& fc = *containersPtr->get<FogContainer>(Containers::FOG_SIM);
	fc.setData("WorldVariables");
}

void GameSettings::draw(Console* _console)
{

}

