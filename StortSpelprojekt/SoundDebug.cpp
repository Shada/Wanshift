#include "SoundDebug.h"
#include "MainState.h"
#include "SoundEngine.h"

SoundDebug::SoundDebug()
{
}


SoundDebug::~SoundDebug()
{
}

bool SoundDebug::init(Console* _console, MainState& _mainState)
{
	return true;
}

bool SoundDebug::command(std::string& _command)
{
	char delimiter = ' ';
	int sep = _command.find_last_of(delimiter);
	std::string key, val;
	key = _command.substr(_command.find_first_not_of(delimiter), sep);
	val = _command.substr(sep + 1, _command.find_last_not_of(delimiter));

	if (key == "sound")
	{
		//if (val == "desert")
		//{
		//	SoundEngine::getInstance()->fadeIntoMusic(SoundEngine::Music::BIOM_DESERT);
		//}
		//else if (val == "meadows")
		//{
		//	SoundEngine::getInstance()->fadeIntoMusic(SoundEngine::Music::BIOM_MEDOWS);
		//}
		//else if (val == "forest")
		//{
		//	SoundEngine::getInstance()->fadeIntoMusic(SoundEngine::Music::BIOM_FOREST);
		//}
		//else if (val == "swamp")
		//{
		//	SoundEngine::getInstance()->fadeIntoMusic(SoundEngine::Music::BIOM_SWAMP);
		//}
		//else if (val == "mountains")
		//{
		//	SoundEngine::getInstance()->fadeIntoMusic(SoundEngine::Music::BIOM_MOUNTAIN);
		//}
		if(val == "es")
		{
			SoundEngine::getInstance()->switchEnergySourcesSound();
		}
		else if (val == "bong")
		{
			SoundEngine::getInstance()->playAtmosphericEffect(SoundEngine::Sound::AMBIENT_BONG);
		}
		else if (val == "birds")
		{
			SoundEngine::getInstance()->playAtmosphericEffect(SoundEngine::Sound::AMBIENT_BIRDS);
		}
		else if (val == "ghost")
		{
			SoundEngine::getInstance()->playAtmosphericEffect(SoundEngine::Sound::AMBIENT_GHOST);
		}
		else if (val == "mute")
		{
			SoundEngine::getInstance()->muteSwitchMusic();
		}
		else if (val == "pos")
		{
			SoundEngine::getInstance()->printChannelPos();
		}
		else
		{
			printf("Unknown command for sound debug");
		}
		return true;
	}

	return false;
}

void SoundDebug::update(Console* _console)
{

}

void SoundDebug::draw(Console* _console)
{

}