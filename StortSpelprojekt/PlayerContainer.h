#pragma once

#include <stdint.h>
#include "Container.h"
#include <array>
#include <Model.h>
#include "glm\glm.hpp"
#define MIDNODETERRAINID 4

// Have it as 15 miuntes for the time being
#define LENGTHOFDAY 900

#define IDLEFREQ 0.1f
#define RUNNINGFREQ 0.02f
#define EXTRACTFREQ 0.1f

enum AnimationState
{
	aIDLE = 0,
	aRUNNING,
	aEXTRACTING,
	aNOANIMATION,
};

struct PlayerContainer : Container
{
	Model playerModel;
	std::pair< ull, ull > id;
	//position of the player
	glm::vec3	position;
	//terrain start position
	glm::vec3	terrainStartPos;
	//player position from last time a chunk was loaded
	glm::vec3	playerCullingPos;
	//rotation of player in x and z
	glm::vec2	rotation;		//for logic
	glm::vec2	modelRotation;	//for model
	float		rotationSpeed;	//for model when falling
	glm::vec2	velocity2D;
	glm::vec2	acceleration;
	float		velocity;
	float		accelerationStrength;
	float		maxSpeed;

	bool isDraining;
	int absorbingPlayer;
	int absorbingEnergy;

	glm::vec4 adaptation1, adaptation2;

	float rot;

	AnimationState animationState;
	int animationFrame;
	int prevAnimationFrame;
	float animationInterpolation;

	int smallTerrainID;
	int mediumTerrainID;
	int largeTerrainID;

	std::array < glm::vec3, 10 > playersPosition;

	std::array < std::pair< ull, ull >, 10 > playersId;
	std::array < glm::vec2, 10 > playersRotation; // x z
	std::array < float, 10 > playersRotDegree;
	std::array < float, 10 > playersEnergy;

	std::array < AnimationState, 10 >	playersAnimationState;
	std::array < int, 10 >				playersAnimationFrame;
	std::array < int, 10 >				playersPrevAnimationFrame;
	std::array < float, 10 >			playersAnimationInterpolation;

	std::array < std::pair< ull, ull >, 20 > energysId;
	std::array< glm::vec3, 20 > energysPos;
	std::array< float, 20 > energysEnergy;

	struct EnergyInfo
	{
		float energyAmount; // total amount of energy on the player
		float playerTemperature; // the current temperature of the player
		float idealTemperature; // the temperature the player wants. This will change as the player adapts to it's environment
		float energyLossPerDegree; // how much energy do the player lose for each degree difference / second
		float surroundingTemperature; // temperature of the surrounding environment. 
		float energyLossPerSecond; // the player always lose a constant amount of energy every second. 

		glm::vec4 biomTemperatures1, biomTemperatures2;

		EnergyInfo() : energyAmount(600), playerTemperature(20),
			energyLossPerDegree(5), surroundingTemperature(20), idealTemperature(20), energyLossPerSecond(2)
		{}
	}energyInfo;

	void addData(uint _index, uint _number) {}

	// See Container.h for documentation
	void removeData(uint _index, uint _number) {}

	void reset();

	void animateModel(std::pair< ull, ull > _id, AnimationState &_state, int &_frame, int &_prevFrame, float &_timer, float &_animInterpolation, float _idleFreq = 0.1f, float _runningFreq = 0.02f, float _extractionFreq = 0.1f);
};