#pragma once

#include "AspectInterface.h"
#include "Containers.h"
#include "WanInput/InputInterface.h"
#include "NetworkAspect.h"
#include "TerrainAspect.h"

enum PlayerKeys
{
	FORWARD = 0,
	BACKWARDS = 1,
	LEFT = 2,
	RIGHT = 3,
	JUMP = 4
};

class PlayerAspect :public AspectInterface
{
public:
	PlayerAspect();
	~PlayerAspect();

	void runAspect(float _dt) override;
	void init(MainState& _mainState)override;
	/* Set a force to launch the player into the air
	*
	* #_force will affect how far up in the air the player is sent
	*/
	void			launchPlayer(float _force);
	// #TODO: make it a direction in which to launch the player. May be used by monsters who fling the player

	/* Move across the landscape, called from the messagePump
	*
	* #_dt: delta time
	*/
	void			move(float _dt);


	/*resets the player to its original values */
	void reset()override;
private:
	Containers				c;
	PlayerContainer			&player;
	TerrainContainer		*terrain;
	NetworkAspect			*network;
	TerrainAspect			*terrainAspect;
	BiomsContainer			&bioms;

	InputInterface			*input;

	glm::vec2				spawnPos;
	glm::vec3				terrainStartPos;

	const glm::vec3			*lookAt;

	int						updatesPerAdaptationSum,
							cbSpotLightIndex;

	float					verticalForce,
							adaptUpdateFreq,
							dtSum;

	float					animationTimer,
							idleFreq,
							runningFreq,
							extrationFreq,
							deathDuration,
							deathCutofTime,
							speedDecrease;

	SingleBiom				adaptation,
							currentAdaptation;

	bool					inTheAir,
							playerBoost,
							lastFrameGlide,
							threadLock,
							threadLockTiny;

	std::array<float, CHUNKSIZE * CHUNKSIZE> *terrainHeights;

	// These keys are used for the players movement and can be changed in the options menu
	KeyCode					forwardKey,
							backwardKey,
							leftKey,
							rightKey,
							jumpKey;

	bool					forwardKeyDown,
							backwardKeyDown,
							leftKeyDown,
							rightKeyDown,
							jumpKeyDown,
							autorun,
							attackKey,
							flashlight;

	static RenderInterface *graphics;

	/* Loose traction and slide downwards when attempting to climb too steep hills
	*
	* #_dt: delta time
	* #_terrainNormal is the normal of the triangle you are standing in
	*/
	void glide(float _dt, glm::vec3 _terrainNormal);
	/* Apply gravitational force to the player
	*
	* #_dt: delta time
	*/
	void applyGravity(float _dt);
	/* Adjust the movement to the terrain
	*
	* #_dt: delta time
	*/
	void followTerrain(float _dt);
	/* The player jumps up into the air
	*/
	void jump();

	void absorb(float _dt);

	void slowDeath(float _dt);

	void updateChunk(glm::vec3 _newPos);

	void reCreateTerrain();
	void reCreateTinys();

	/* Used to remove redundant code from the loop interpreting the input
	* #_dir the resulting speed that will have the moving direction added to it
	* #_x 1 or -1 depending on direction
	* #_z 1 or -1 depending on direction
	*/
	void checkKeyAndDirection(KeyState _state, bool &_keyDown);

	/* in this function, the player's anergy is consumed to keep a good temperature */
	void consumeEnergy(float _dt);

	/* when the player collects energy it is absorbed and added to the total energy amount */
	void absorbEnergy(float _energyAmount);
};