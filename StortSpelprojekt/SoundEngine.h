#pragma once
#include <glm/glm.hpp>
#include <fmod/fmod.hpp>
//#include <fmod/fmod_errors.h>
#include <map>
#include <queue>
#include <array>
#include "Containers.h"
#include "EnemyContainer.h"
#include "CameraContainer.h"
#include "PlayerContainer.h"
#include "TerrainContainer.h"

const float DISTANCEFACTOR	= 1.0f;
const float DOPPLERSCALE	= 1.0f;
const float ROLLOFFSCALE	= 1.0f;
const float MINDIST			= 0.5f;
const float MAXDIST			= 100.0f;
const float FADESPEED		= 0.001f;
const float MUSICVOLUME		= 0.5f;
const float EFFECTVOLUME	= 1.0f;

class SoundEngine
{
public:

	enum class Sound
	{
		BIOM_SHIFT,
		PLAYER_WALKING,
		PLAYER_ABSORB,
		NPC_IDLE,
		NPC_ANGRY,
		AMBIENT_GHOST,
		AMBIENT_BIRDS,
		AMBIENT_BONG,
		AMBIENT_ENERGYSOURCE,	
	};

	enum class Music
	{
		BIOM_DESERT,
		BIOM_MEDOWS,
		BIOM_FOREST,
		BIOM_SWAMP,
		BIOM_MOUNTAIN,
	};
	
	~SoundEngine();

	static SoundEngine* getInstance()
	{
		if (!instance)
		{
			instance = new SoundEngine();
		}

		return instance;
	}

	void initSystem();
	void initMusic();
	void initEffects();
	void playPlayerSound(Sound _sound);
	void playOtherPlayerSound(Sound _sound, std::pair<ull, ull> _id);
	void playNPCSound(Sound _sound, std::pair<ull, ull> _id);
	bool playAtmosphericEffect(Sound _sound, glm::vec3 _pos = glm::vec3());	

	void update(float _dt);
	void destroy();

	void switchEnergySourcesSound();
	void muteSwitchMusic();
	void printChannelPos();

private:

	SoundEngine();
	SoundEngine(SoundEngine const& copy);
	SoundEngine& operator=(SoundEngine const& copy);

	struct Moving3DSoundSource
	{
		std::pair<ull, ull>				id;
		FMOD::Channel					*channel;
		std::map<Sound, FMOD::Sound*>	sound;
		FMOD_VECTOR						lastPos, currPos, vel;
		float							volume;

		Moving3DSoundSource(std::map<Sound, FMOD::Sound*> _sounds, FMOD_VECTOR _pos, std::pair<ull, ull> id)
		{
			vel.x = 0.0f;
			vel.y = 0.0f;
			vel.z = 0.0f;
			channel->set3DAttributes(&_pos, &vel);
			sound = _sounds;
			lastPos = currPos = _pos;
			volume = MUSICVOLUME;
		}

		glm::vec3 getGlmVec3CurPos()
		{
			return glm::vec3(currPos.x, currPos.y, currPos.z);
		}

		void updatePos(glm::vec3 _pos, float _dt)
		{
			lastPos = currPos;
			currPos.x = _pos.x;
			currPos.y = _pos.y;
			currPos.z = _pos.z;

			vel.x = (currPos.x - lastPos.x) * _dt;
			vel.y = (currPos.y - lastPos.y) * _dt;
			vel.z = (currPos.z - lastPos.z) * _dt;

			channel->set3DAttributes(&currPos, &vel);
		}
	};

	struct Static3DSound
	{
		FMOD::Channel*	channel;
		FMOD::Sound*	sound;
		FMOD_VECTOR		pos, vel;
		float			volume;

		Static3DSound()
		{
			channel		= 0;
			sound		= 0;
			vel.x		= 0.0f;
			vel.y		= 0.0f;
			vel.z		= 0.0f;
			volume		= EFFECTVOLUME;
		}

		void init(FMOD::Sound* _sound, glm::vec3 _pos)
		{			
			pos.x		= _pos.x;
			pos.y		= _pos.y;
			pos.z		= _pos.z;
			sound		= _sound;
			channel->set3DAttributes(&pos, &vel);					
		}

		glm::vec3 getGlmVec3Pos()
		{
			return glm::vec3(pos.x, pos.y, pos.z);
		}

		void updatePos(glm::vec3 _pos)
		{
			pos.x		= _pos.x;
			pos.y		= _pos.y;
			pos.z		= _pos.z;
			channel->set3DAttributes(&pos, &vel);
		}
	};

	struct StaticSound
	{
		FMOD::Channel	*channel;
		FMOD::Sound		*sound;
		float			volume;
		bool			active, channelInit;

		StaticSound(FMOD::Sound* _sound)
		{
			sound = _sound;
			volume = 0.0f;
			active = false;
			channelInit = false;
		}
	};

	std::map<Sound, StaticSound*>			StaticSounds;
	std::map<Sound, FMOD::Sound*>			dynamicSounds;
	std::map<Sound, FMOD::Sound*>			playersSoundMap;
	std::map<Music, StaticSound*>			music;
	std::array<Static3DSound*, 20>			energySources;
	std::array<Moving3DSoundSource*, 5>		NPCSounds;
	std::array<Moving3DSoundSource*, 10>	playersSoundSources;
	FMOD::Channel*							playerChannel, *birdCh, *bongCh, *ghostCh;
	FMOD::Sound								*energySource, *birds, *bong, *ghost, *shift, *absorbLocal, *walkingLocal, *walking, *absorb, *npcIdle, *npcAngry;
	FMOD::System*							system;
	FMOD_RESULT								result;
	FMOD_VECTOR								lastPos, up;
	std::queue<Music>						musicQueue;
	bool									fadeInProgress, firstPlay;
	Music									currentSong;

	EnemyContainer*							NPCs;
	PlayerContainer*						players;
	CameraContainer*						camera;
	TerrainContainer*						terrain;
	BiomsContainer							*bioms;

	static SoundEngine*						instance;

	void initLocalEffects();
	void initEnergySources();
	void initNPCs();
	void initPlayers();
	void initNonLocalEffects();
	void checkResult(FMOD_RESULT _result);
	void swapSong();
	void updateEnergySouces();
	void updateNPCs(float _dt);
	void updatePlayers(float _dt);
	void fadeMusic();
	void setPlayerSoundPos(float _dt);
	Music getBiomMusic(float _dt);
	bool atmosphereHelper(Sound _sound, FMOD_VECTOR pos, FMOD_VECTOR vel, FMOD::Channel* _ch);
	void playAtmospher(Sound _sound, FMOD_VECTOR pos, FMOD_VECTOR vel, FMOD::Channel* _ch);
};

