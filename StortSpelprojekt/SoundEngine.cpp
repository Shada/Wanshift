#include "SoundEngine.h"
#include "Utility.h"
#include <math.h>

SoundEngine* SoundEngine::instance = nullptr;

SoundEngine::SoundEngine()
{
	lastPos.x			= 0.0f;
	lastPos.y			= 0.0f;
	lastPos.z			= 0.0f;
	up.x			= 0.0f;
	up.y			= 1.0f;
	up.z			= 0.0f;
	playerChannel		= nullptr;
	birdCh				= nullptr;
	bongCh				= nullptr;
	ghostCh				= nullptr;
	fadeInProgress		= false;
	firstPlay			= true;
	energySources		= std::array<Static3DSound*, 20>();
	NPCSounds			= std::array<Moving3DSoundSource*, 5>();
	playersSoundSources	= std::array<Moving3DSoundSource*, 10>();
	playersSoundMap		= std::map<Sound, FMOD::Sound*>();
	musicQueue			= std::queue<Music>();
}

SoundEngine::~SoundEngine()
{
}

void SoundEngine::initSystem()
{
	Containers c(std::make_pair(Containers::ENEMY, READ), std::make_pair(Containers::PLAYER, READ), std::make_pair(Containers::CAMERA, READ), std::make_pair(Containers::TERRAIN, READ));

	NPCs = c.get<EnemyContainer>(Containers::ENEMY);
	players = c.get<PlayerContainer>(Containers::PLAYER);
	camera = c.get<CameraContainer>(Containers::CAMERA);
	terrain = c.get<TerrainContainer>(Containers::TERRAIN);
	bioms = c.get<BiomsContainer>(Containers::BIOMS);

	void *extradriverdata = 0;

	musicQueue = std::queue<Music>();

	result = FMOD::System_Create(&system);
	checkResult(result);

	result = system->init(100, FMOD_INIT_NORMAL, extradriverdata);
	checkResult(result);

	result = system->set3DSettings(DOPPLERSCALE, DISTANCEFACTOR, ROLLOFFSCALE);
	checkResult(result);

	currentSong = getBiomMusic(0.0f);
	musicQueue.push(currentSong);
}

void SoundEngine::initMusic()
{
	FMOD::Sound	*desert;
	result = system->createSound("Sounds\\Music\\desert.mp3", FMOD_DEFAULT, 0, &desert);
	checkResult(result);
	result = desert->setMode(FMOD_LOOP_NORMAL);
	checkResult(result);
	music[Music::BIOM_DESERT] = new StaticSound(desert);

	FMOD::Sound *forest;
	result = system->createSound("Sounds\\Music\\forest.mp3", FMOD_DEFAULT, 0, &forest);
	checkResult(result);
	result = forest->setMode(FMOD_LOOP_NORMAL);
	checkResult(result);
	music[Music::BIOM_FOREST] = new StaticSound(forest);

	FMOD::Sound *meadows;
	result = system->createSound("Sounds\\Music\\meadows.mp3", FMOD_DEFAULT, 0, &meadows);
	checkResult(result);
	result = meadows->setMode(FMOD_LOOP_NORMAL);
	checkResult(result);
	music[Music::BIOM_MEDOWS] = new StaticSound(meadows);

	FMOD::Sound *mountains;
	result = system->createSound("Sounds\\Music\\mountains.mp3", FMOD_DEFAULT, 0, &mountains);
	checkResult(result);
	result = mountains->setMode(FMOD_LOOP_NORMAL);
	checkResult(result);
	music[Music::BIOM_MOUNTAIN] = new StaticSound(mountains);

	FMOD::Sound *swamp;
	result = system->createSound("Sounds\\Music\\swamp.mp3", FMOD_DEFAULT, 0, &swamp);
	checkResult(result);
	result = swamp->setMode(FMOD_LOOP_NORMAL);
	checkResult(result);
	music[Music::BIOM_SWAMP] = new StaticSound(swamp);
}

void SoundEngine::initEffects()
{	
	initLocalEffects();
	initNonLocalEffects();
	initEnergySources();
	initNPCs();
	initPlayers();
}

void SoundEngine::initLocalEffects()
{
	result = system->createSound("Sounds\\Sounds\\biomshift.mp3", FMOD_DEFAULT, 0, &shift);
	checkResult(result);
	result = shift->setMode(FMOD_LOOP_OFF);
	checkResult(result);
	StaticSounds[Sound::BIOM_SHIFT] = new StaticSound(shift);

	result = system->createSound("Sounds\\Sounds\\absorb.mp3", FMOD_DEFAULT, 0, &absorbLocal);
	checkResult(result);
	result = absorbLocal->setMode(FMOD_LOOP_OFF);
	checkResult(result);
	StaticSounds[Sound::PLAYER_ABSORB] = new StaticSound(absorbLocal);

	result = system->createSound("Sounds\\Sounds\\walking.mp3", FMOD_DEFAULT, 0, &walkingLocal);
	checkResult(result);
	result = walkingLocal->setMode(FMOD_LOOP_OFF);
	checkResult(result);
	StaticSounds[Sound::PLAYER_WALKING] = new StaticSound(walkingLocal);
}

void SoundEngine::initNonLocalEffects()
{
	result = system->createSound("Sounds\\Sounds\\birds.mp3", FMOD_3D, 0, &birds);
	checkResult(result);
	result = birds->set3DMinMaxDistance(MINDIST * DISTANCEFACTOR, MAXDIST * DISTANCEFACTOR);
	checkResult(result);
	result = birds->setMode(FMOD_LOOP_OFF);
	checkResult(result);
	dynamicSounds[Sound::AMBIENT_BIRDS] = birds;

	result = system->createSound("Sounds\\Sounds\\bong.mp3", FMOD_3D, 0, &bong);
	checkResult(result);
	result = bong->set3DMinMaxDistance(MINDIST * DISTANCEFACTOR, MAXDIST * DISTANCEFACTOR);
	checkResult(result);
	result = bong->setMode(FMOD_LOOP_OFF);
	checkResult(result);
	dynamicSounds[Sound::AMBIENT_BONG] = bong;

	result = system->createSound("Sounds\\Sounds\\ghost.mp3", FMOD_3D, 0, &ghost);
	checkResult(result);
	result = ghost->set3DMinMaxDistance(MINDIST * DISTANCEFACTOR, MAXDIST * DISTANCEFACTOR);
	checkResult(result);
	result = ghost->setMode(FMOD_LOOP_OFF);
	checkResult(result);
	dynamicSounds[Sound::AMBIENT_GHOST] = ghost;
}

void SoundEngine::initEnergySources()
{	
	result = system->createSound("Sounds\\Sounds\\energySource.mp3", FMOD_3D, 0, &energySource);
	checkResult(result);
	result = energySource->set3DMinMaxDistance(MINDIST * DISTANCEFACTOR, MAXDIST * DISTANCEFACTOR);
	checkResult(result);
	result = energySource->setMode(FMOD_LOOP_NORMAL);
	checkResult(result);

	for (uint i = 0; i < players->energysPos.size(); i++)
	{
		Static3DSound* s = new Static3DSound();
		s->init(energySource, players->energysPos[i]);
		energySources[i] = s;
	}

	for (auto a : energySources)
	{
		result = system->playSound(a->sound, 0, false, &a->channel);
		checkResult(result);
	}
}

void SoundEngine::initNPCs()
{
	std::map<Sound, FMOD::Sound*> tmp = std::map<Sound, FMOD::Sound*>();
	result = system->createSound("Sounds\\Sounds\\NPC_Idle.mp3", FMOD_3D, 0, &npcIdle);
	checkResult(result);
	result = npcIdle->set3DMinMaxDistance(MINDIST * DISTANCEFACTOR, MAXDIST * DISTANCEFACTOR);
	checkResult(result);
	result = npcIdle->setMode(FMOD_LOOP_NORMAL);
	checkResult(result);
	tmp[Sound::NPC_IDLE] = npcIdle;

	result = system->createSound("Sounds\\Sounds\\NPC_angry.mp3", FMOD_3D, 0, &npcAngry);
	checkResult(result);
	result = npcAngry->set3DMinMaxDistance(MINDIST * DISTANCEFACTOR, MAXDIST * DISTANCEFACTOR);
	checkResult(result);
	result = npcAngry->setMode(FMOD_LOOP_NORMAL);
	checkResult(result);
	tmp[Sound::NPC_ANGRY] = npcAngry;

	for(uint i = 0; i < NPCs->pos.size(); i++)
	{
		FMOD_VECTOR pos = { NPCs->pos[i].x, NPCs->pos[i].y, NPCs->pos[i].z };
		NPCSounds[i] = new Moving3DSoundSource(tmp, pos, NPCs->ids[i]);
	}

	for(auto a : NPCSounds)
	{
		result = system->playSound(a->sound[Sound::NPC_IDLE], 0, false, &a->channel);
		checkResult(result);
	}
}

void SoundEngine::initPlayers()
{
	result = system->createSound("Sounds\\Sounds\\walking.mp3", FMOD_3D, 0, &walking);
	checkResult(result);
	result = walking->set3DMinMaxDistance(MINDIST * DISTANCEFACTOR, MAXDIST * DISTANCEFACTOR);
	checkResult(result);
	result = walking->setMode(FMOD_LOOP_OFF);
	checkResult(result);
	playersSoundMap[Sound::PLAYER_WALKING] = walking;

	result = system->createSound("Sounds\\Sounds\\absorb.mp3", FMOD_3D, 0, &absorb);
	checkResult(result);
	result = absorb->set3DMinMaxDistance(MINDIST * DISTANCEFACTOR, MAXDIST * DISTANCEFACTOR);
	checkResult(result);
	result = absorb->setMode(FMOD_LOOP_OFF);
	checkResult(result);
	playersSoundMap[Sound::PLAYER_ABSORB] = absorb;

	for (uint i = 0; i < players->playersPosition.size(); i++)
	{
		FMOD_VECTOR pos = { players->playersPosition[i].x, players->playersPosition[i].y, players->playersPosition[i].z };
		playersSoundSources[i] = new Moving3DSoundSource(playersSoundMap, pos, players->playersId[i]);
	}
}

void SoundEngine::update(float _dt)
{
	setPlayerSoundPos(_dt);

	Music m = getBiomMusic(_dt);

	if (musicQueue.size() < 1 && currentSong != m)
	{
		currentSong = m;
		musicQueue.push(currentSong);
	}

	if (!fadeInProgress && musicQueue.size() > 0)
	{
		swapSong();
	}

	if (firstPlay && musicQueue.size() > 0)
	{
		fadeInProgress = false;
		firstPlay = false;
	}

	fadeMusic();
	updateEnergySouces();
	updateNPCs(_dt);
	updatePlayers(_dt);

	result = system->update();
	checkResult(result);
}

void SoundEngine::playPlayerSound(Sound _sound)
{
	result = system->playSound(StaticSounds[_sound]->sound, 0, false, &playerChannel);
	checkResult(result);
}

void SoundEngine::playOtherPlayerSound(Sound _sound, std::pair<ull, ull> _id)
{
	int index = 0;

	for (uint i = 0; i < players->playersId.size(); i++)
	{
		if (players->playersId[i] == _id)
		{
			index = i;
			break;
		}
	}

	result = system->playSound(playersSoundSources[index]->sound[_sound], 0, false, &playersSoundSources[index]->channel);
	checkResult(result);
}

void SoundEngine::playNPCSound(Sound _sound, std::pair<ull, ull> _id)
{
	int index = 0;

	for (uint i = 0; i < NPCs->ids.size(); i++)
	{
		if (NPCs->ids[i] == _id)
		{
			index = i;
			break;
		}
	}

	NPCSounds[index]->channel->stop();

	result = system->playSound(NPCSounds[index]->sound[_sound], 0, false, &NPCSounds[index]->channel);
	checkResult(result);
}

bool SoundEngine::playAtmosphericEffect(Sound _sound, glm::vec3 _pos)
{
	FMOD_VECTOR	pos, vel;
	vel.x = 0.0f;
	vel.y = 0.0f;
	vel.z = 0.0f;

	if (_pos == glm::vec3())
	{
		pos.x = players->position.x;
		pos.y = players->position.y;
		pos.z = players->position.z;
	} 
	else
	{
		pos.x	= _pos.x;
		pos.y	= _pos.y;
		pos.z	= _pos.z;
	}	

	switch (_sound)
	{
	case SoundEngine::Sound::AMBIENT_GHOST:
		return atmosphereHelper(_sound, pos, vel, ghostCh);

	case SoundEngine::Sound::AMBIENT_BIRDS:
		return atmosphereHelper(_sound, pos, vel, birdCh);

	case SoundEngine::Sound::AMBIENT_BONG:
		return atmosphereHelper(_sound, pos, vel, bongCh);

	}

	return false;
}

void SoundEngine::checkResult(FMOD_RESULT _result)
{
	if (result != FMOD_OK)
	{
		//printf(FMOD_ErrorString(_result));
	}
}

void SoundEngine::destroy()
{
	for (auto a : music)
	{
		result = a.second->sound->release();
		checkResult(result);
	}

	for (auto a : StaticSounds)
	{
		result = a.second->sound->release();
		checkResult(result);
	}

	energySource->release();
	npcIdle->release();
	npcAngry->release();
	walking->release();
	absorb->release();

	playersSoundMap.clear();
	
	for (auto a : playersSoundSources)
	{
		a->sound.clear();
	}

	for (auto a : energySources)
	{
		 a->sound = nullptr;
	}

	for (auto a : NPCSounds)
	{
		a->sound.clear();
	}

	result = system->close();
	checkResult(result);

	result = system->release();
	checkResult(result);
}

void SoundEngine::swapSong()
{
	for (auto a : music)
	{
		if (a.second->active)
		{
			a.second->active = false;
		}

		if (a.first == musicQueue.front())
		{
			a.second->active = true;

			if (!a.second->channelInit)
			{
				a.second->channelInit = true;
				result = system->playSound(a.second->sound, 0, false, &a.second->channel);
				checkResult(result);
				result = a.second->channel->setVolume(0.0f);
				checkResult(result);
			}
		}
	}
	musicQueue.pop();
}

void SoundEngine::updateEnergySouces()
{
	for(uint i = 0; i < energySources.size(); i++)
	{
		if(energySources[i]->getGlmVec3Pos() != players->energysPos[i])
		{
			energySources[i]->updatePos(players->energysPos[i]);
		}
	}
}

void SoundEngine::updateNPCs(float _dt)
{
	for(uint i = 0; i < NPCSounds.size(); i++)
	{
		if(NPCSounds[i]->getGlmVec3CurPos() != NPCs->pos[i])
		{
			NPCSounds[i]->updatePos(NPCs->pos[i], _dt);
		}
	}
}

void SoundEngine::fadeMusic()
{
	for (auto a : music)
	{
		if (a.second->active && a.second->volume < MUSICVOLUME)
		{
			if (a.second->volume <= 0.0f)
			{
				//result = a.second->channel->setPaused(false);
				//checkResult(result);
				fadeInProgress = true;
			}

			a.second->volume += FADESPEED;

			result = a.second->channel->setVolume(std::powf(a.second->volume, 2.0f));
			checkResult(result);
		}
		else if (!a.second->active && a.second->volume > 0.0f)
		{
			a.second->volume -= FADESPEED;
			result = a.second->channel->setVolume(std::powf(a.second->volume, 2.0f));
			checkResult(result);

			if (a.second->volume <= 0.0f)
			{
				result = a.second->channel->stop();
				checkResult(result);
				fadeInProgress = false;
				a.second->channelInit = false;
			}
		}
	}
}

void SoundEngine::updatePlayers(float _dt)
{
	for(uint i = 0; i < playersSoundSources.size(); i++)
	{
		if(playersSoundSources[i]->getGlmVec3CurPos() != players->playersPosition[i])
		{
			playersSoundSources[i]->updatePos(players->playersPosition[i], _dt);
		}
	}
}

void SoundEngine::setPlayerSoundPos(float _dt)
{
	FMOD_VECTOR fwd = { camera->vLookDir.x, camera->vLookDir.y, camera->vLookDir.z };
	FMOD_VECTOR pos = { players->position.x, players->position.y, players->position.z };
	FMOD_VECTOR vel;

	vel.x = (players->position.x - lastPos.x) * _dt;
	vel.y = (players->position.y - lastPos.y) * _dt;
	vel.z = (players->position.z - lastPos.z) * _dt;

	lastPos = pos;

	result = system->set3DListenerAttributes(0, &pos, &vel, &fwd, &up);
	checkResult(result);
}

void SoundEngine::switchEnergySourcesSound()
{
	bool paused = false;

	for (auto a : energySources)
	{
		a->channel->getPaused(&paused);

		if (paused)
		{
			result = a->channel->setPaused(false);
			checkResult(result);
		} 
		else
		{
			result = a->channel->setPaused(true);
			checkResult(result);
		}

		printf("is paused: %s\n", !paused ? "true" : "false");
	}
}

void SoundEngine::muteSwitchMusic()
{
	bool mute = false;

	for (auto a : music)
	{
		a.second->channel->getMute(&mute);

		if (mute)
		{
			a.second->channel->setMute(false);
		} 
		else
		{
			a.second->channel->setMute(true);
		}

		printf("is muted: %s\n", !mute ? "true" : "false");
	}
}

void SoundEngine::printChannelPos()
{
	FMOD_VECTOR pos, vel;
	for (uint i = 0; i < energySources.size(); i++)
	{
		energySources[i]->channel->get3DAttributes(&pos, &vel);
		printf("channel: %i\tX:%f Y:%f Z:%f\n", i, pos.x, pos.y, pos.z);
	}
}

SoundEngine::Music SoundEngine::getBiomMusic(float _dt)
{
	SingleBiom& biom = Utility::getBlendMapValue(bioms, players->position);
	std::array<float, 5> bioms = std::array<float, 5>();

	bioms[0] = biom.map1.x;
	bioms[1] = biom.map1.y;
	bioms[2] = biom.map1.z;
	bioms[3] = biom.map1.w;
	bioms[4] = biom.map2.x;

#ifdef _DEBUG
	static float t = 0.0f;
	t += _dt;

	if (t >= 1000.0f)
	{
		t = 0.0f;
		printf("Desert:%f\tMountain:%f\tMeadows:%f\tSwamp:%f\tForest:%f\n", bioms[0], bioms[1], bioms[2], bioms[3], bioms[4]);
	}
#endif // _DEBUG

	int maxIndex = 0;
	float maxVal = -1.0f;

	for (uint i = 0; i < bioms.size(); i++)
	{
		if (bioms[i] > maxVal)
		{
			maxVal = bioms[i];
			maxIndex = i;
		}
	}

	switch (maxIndex)
	{
	case 0:
		return Music::BIOM_DESERT;
	case 1:
		return Music::BIOM_MOUNTAIN;
	case 2:
		return Music::BIOM_MEDOWS;
	case 3:
		return Music::BIOM_SWAMP;
	case 4:
		return Music::BIOM_FOREST;
	}
	return Music::BIOM_DESERT;
}

bool SoundEngine::atmosphereHelper(Sound _sound, FMOD_VECTOR pos, FMOD_VECTOR vel, FMOD::Channel* _ch)
{
	bool playing = true;

	if(_ch == nullptr)
	{
		playAtmospher(_sound, pos, vel, _ch);
		return true;
	}

	_ch->isPlaying(&playing);

	if (!playing)
	{
		playAtmospher(_sound, pos, vel, _ch);
		return true;
	}
	else
	{
		return false;
	}
}

void SoundEngine::playAtmospher(Sound _sound, FMOD_VECTOR pos, FMOD_VECTOR vel, FMOD::Channel* _ch)
{
	result = system->playSound(dynamicSounds[_sound], 0, false, &_ch);
	checkResult(result);
	result = _ch->set3DAttributes(&pos, &vel);
	checkResult(result);
}