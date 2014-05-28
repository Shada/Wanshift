#include "PlayerAspect.h"
#include "MainState.h"
#include "Utility.h"
#include "SoundEngine.h"

#define PI 3.14159265359

RenderInterface* PlayerAspect::graphics = nullptr;

struct AbsorbInfo
{
	glm::vec3 pos;
	int id;
	AbsorbInfo(glm::vec3 v, int i) : pos(v), id(i) {}
};

PlayerAspect::PlayerAspect() : AspectInterface("Player", 10), c(std::make_pair(Containers::TERRAIN, READ), std::make_pair(Containers::BIOMS, READ)),
player(*c.get<PlayerContainer>(Containers::PLAYER)), bioms(*c.get<BiomsContainer>(Containers::BIOMS))
{
}

void PlayerAspect::init(MainState& _mainState)
{
	input = _mainState.input;
	network = _mainState.getAspect<NetworkAspect>("network");
	terrainAspect = _mainState.getAspect<TerrainAspect>("Terrain");
	graphics = _mainState.graphics;

	CameraContainer &camera(*c.get<CameraContainer>(Containers::CAMERA));
	lookAt = &camera.vLookDirReference;

	terrain = c.get<TerrainContainer>(Containers::TERRAIN);

	// Replace this with stuff sent by the server
	//player.position = glm::vec3(100000, 0, 95000);
	player.position.y = Utility::mapToTerrain(glm::vec2(player.position.x, player.position.z), *terrain).y;

	adaptation.map1 = player.adaptation1;
	adaptation.map2 = player.adaptation2;

	deathCutofTime = 0.f;

	updatesPerAdaptationSum = 0;

	cbSpotLightIndex = graphics->getHandleIndex("cbSpotLight", ResourceType::ConstantBuffer);
	
	player.playerCullingPos = player.position;
	player.velocity = verticalForce = 0;
	dtSum = 0.f;
	adaptUpdateFreq = 1.f;

	// TODO: save and get options upon startup, if these has changed they should remain the same
	// next time the game starts
	forwardKey = KeyCode::W;
	backwardKey = KeyCode::S;
	leftKey = KeyCode::A;
	rightKey = KeyCode::D;
	jumpKey = KeyCode::SPACE;

	threadLock = threadLockTiny = false;
	playerBoost = lastFrameGlide = inTheAir = false;

	player.energyInfo.biomTemperatures1 = glm::vec4(35, 10, 15, 22);
	player.energyInfo.biomTemperatures2 = glm::vec4(17, 0, 0, 0);

	idleFreq = IDLEFREQ;
	extrationFreq = EXTRACTFREQ;
	runningFreq = RUNNINGFREQ;

	deathDuration = 6.f;

	updateChunk(player.position);

	player.animationState = aIDLE;
	player.animationFrame = 0;
	player.prevAnimationFrame = 0;
	player.animationInterpolation = 0;
	animationTimer = 0;
	player.rot = 0;

	forwardKeyDown = false;
	backwardKeyDown = false;
	leftKeyDown = false;
	rightKeyDown = false;
	jumpKeyDown = false;
	attackKey = false;
	autorun = false;
	flashlight = false;
	player.isDraining = false;
	player.absorbingEnergy = -1;
	player.absorbingPlayer = -1;
}

void PlayerAspect::runAspect(float _dt)
{
	// Use as seconds instead of milliseconds
	_dt /= 1000;
	if(_dt > 1.f)
		return;

	dtSum += _dt;
	if(player.energyInfo.energyAmount > 0 && dtSum >= adaptUpdateFreq)
	{
		currentAdaptation += Utility::getBlendMapValue(&bioms, player.position);

		dtSum = 0.f;
		updatesPerAdaptationSum++;
	}

	if(player.energyInfo.energyAmount > 0 && updatesPerAdaptationSum >= 10)
	{
		float x = currentAdaptation.map1.x, y = currentAdaptation.map1.y, z = currentAdaptation.map1.z, w = currentAdaptation.map1.w,
			x2 = currentAdaptation.map2.x;

		// will take 15 min to adapt fully to a biom
		if(x + y + z + w > 0)
			adaptation.map1 += glm::normalize(glm::vec4(x - y - z - w, y - x - z - w, z - x - y - w, w - x - y - z)) / (updatesPerAdaptationSum * 0.5f);
		if(x2 > 0)
			adaptation.map2 += glm::vec4(x2, 0, 0, 0);

		for(int i = 0; i < 4; i++)
		{
			if(adaptation.map1[i] < 0) adaptation.map1[i] = 0;
			if(adaptation.map2[i] < 0) adaptation.map2[i] = 0;
		}

		if(glm::length(adaptation.map1 + adaptation.map2) > 1)
		{
			float len = 1.f / (glm::length(adaptation.map1 + adaptation.map2));
			adaptation.map1 *= len;
			adaptation.map2 *= len;
		}
		
		currentAdaptation.map1 = currentAdaptation.map2 = glm::vec4();
		updatesPerAdaptationSum = 0;
	}

	if(input->isNewKeyPress(KeyCode::R, KeyState::PRESSED))
		autorun = !autorun;

	playerBoost = input->isContinousKeyPress(KeyCode::LEFT_SHIFT);
	forwardKeyDown = input->isContinousKeyPress(forwardKey) || autorun;
	leftKeyDown = input->isContinousKeyPress(leftKey);
	backwardKeyDown = input->isContinousKeyPress(backwardKey);
	rightKeyDown = input->isContinousKeyPress(rightKey);
	attackKey = input->isContinousKeyPress(KeyCode::E);
	jumpKeyDown = input->isNewKeyPress(jumpKey);

	if(input->isNewKeyPress(KeyCode::F))
		flashlight = !flashlight;

	if(jumpKeyDown && player.energyInfo.energyAmount > 0)
		jump();

	if(attackKey && player.energyInfo.energyAmount > 0)
		absorb(_dt);
	else
		player.isDraining = false;


	glm::vec2 vSpeed;
	if(forwardKeyDown)
	{
		vSpeed.x += lookAt->x;
		vSpeed.y += lookAt->z;
	}
	if(backwardKeyDown)
	{
		vSpeed.x -= lookAt->x;
		vSpeed.y -= lookAt->z;
	}
	if(rightKeyDown)
	{
		vSpeed.x -= lookAt->z;
		vSpeed.y += lookAt->x;
	}
	if(leftKeyDown)
	{
		vSpeed.x += lookAt->z;
		vSpeed.y -= lookAt->x;
	}

	if(vSpeed == glm::vec2())
		playerBoost = false;

	if(!inTheAir && player.rotation != vSpeed)
	{
		if(deathCutofTime <= deathDuration * 0.9f)
			player.rotation = vSpeed != glm::vec2() ? glm::normalize(vSpeed) : vSpeed;
	}

	if(player.energyInfo.energyAmount > 0)
	{
		move(_dt);
		consumeEnergy(_dt);
	}
	else
		slowDeath(_dt);

	if(deathCutofTime <= deathDuration * 0.9f && (vSpeed.x != 0 || vSpeed.y != 0))
	{
		//playersRotDegree
		if(vSpeed.x == 0)
			vSpeed.x = .0001f;
		player.rot = std::acos(vSpeed.y / glm::length(vSpeed)) * vSpeed.x / abs(vSpeed.x);
	}

	CBSpotLight cbSpot;
	cbSpot.lightCount = flashlight ? 1 : 0;
	cbSpot.reach_cosTheta_cosPhi = glm::vec3(2000.0f, cos(std::_Pi / 32.0f), cos(std::_Pi / 4.0f));

	cbSpot.color[0] = glm::vec4(1.f);
	cbSpot.dir[0] = glm::vec4(*lookAt, 0.f);
	cbSpot.pos[0] = glm::vec4(player.position, 1.f);

	graphics->updateCBuffer(cbSpotLightIndex, &cbSpot);

	animationTimer += _dt;
	player.animateModel(player.id, player.animationState, player.animationFrame, player.prevAnimationFrame, animationTimer, player.animationInterpolation, idleFreq, runningFreq, extrationFreq);
}

void PlayerAspect::launchPlayer(float _force)
{
	verticalForce = _force;
	inTheAir = true;

	// Might not support negative force in the final product, design decision needed
	_force > 0 ? player.position.y++ : player.position.y--;
}

void PlayerAspect::move(float _dt)
{
	/* lock the avatar position for debugging purposes */
	//if (input->isNewKeyPress(KeyCode::F1))
	//{
	//	if (debugNoMove)
	//		debugNoMove = false;
	//	else
	//		debugNoMove = true;
	//}

	//if (!debugNoMove)
	//	return;

	if(!inTheAir)
	{
		if(player.rotation.x == 0 && player.rotation.y == 0)
		{
			player.velocity = 0;
			if(player.animationState == aRUNNING)
			{
				player.animationState = aIDLE;
				player.animationFrame = 0;
				player.prevAnimationFrame = 0;
			}
		}
		else
		{
			if(player.animationState != aRUNNING)
			{
				player.animationState = aRUNNING;
				player.animationFrame = 0;
				player.prevAnimationFrame = 0;
			}
			if(adaptation.maxBiomValue() < 0.3f)
				player.velocity = 85 * 0.6f;
			else
			{
				SingleBiom currentPoint = Utility::getBlendMapValue(&bioms, player.position);

				player.velocity = 0;
				for(int i = 0; i < 4; i++)
				{
					if(85 * (0.6f + currentPoint.map1[i] * adaptation.map1[i] / 2.5) > player.velocity)
						player.velocity = 85 * (0.6f + currentPoint.map1[i] * adaptation.map1[i] / 2.5f);
					if(85 * (0.6f + currentPoint.map2[i] * adaptation.map2[i] / 2.5) > player.velocity)
						player.velocity = 85 * (0.6f + currentPoint.map2[i] * adaptation.map2[i] / 2.5f);
				}
			}
		}
		if(playerBoost)
			player.velocity *= 16.f;
	}
	else
		player.velocity *= 0.9999f;

	ForestContainer &forest(*c.get<ForestContainer>(Containers::FOREST));

	bool treeHit = false;
	glm::vec2 playerPos2 = glm::vec2(player.position.x,player.position.z);
	for(uint i = 0; i < forest.treeList.size(); i++)
	{
		glm::vec2 treePos =  glm::vec2(forest.treeList.at(i).pos.x,forest.treeList.at(i).pos.z);

		float dist = glm::length(playerPos2 - treePos);
		float radie = 0.82f * forest.treeList.at(i).scale.x;
		if(dist < radie)
		{
			//sätt till precis utanför, kör bara på xz
			glm::vec2 dir = glm::normalize( playerPos2 - treePos );
			playerPos2 = treePos + dir * radie;
			player.position.x = playerPos2.x;
			player.position.z = playerPos2.y;
			treeHit = true;
			break;
		}
	}
	/*if(treeHit)
		return;*/
	followTerrain(_dt);
}

void PlayerAspect::followTerrain(float _dt)
{
	TerrainContainer &terrain(*c.get<TerrainContainer>(Containers::TERRAIN));

	
	// Move forward to the next position and get the height from that point
	glm::vec3 theoreticalMove = glm::vec3(player.position.x + player.rotation.x * _dt * player.velocity,
										  player.position.y,
										  player.position.z + player.rotation.y * _dt * player.velocity);

	if(theoreticalMove.x >= terrainStartPos.x + QUADSIZE * (terrain.tiny[0].size_x - 1) || theoreticalMove.x < terrainStartPos.x ||
	   theoreticalMove.z >= terrainStartPos.z + QUADSIZE * (terrain.tiny[0].size_y - 1) || theoreticalMove.z < terrainStartPos.z)
	   updateChunk(theoreticalMove);

	// Index of the point closest to the player
	int index = (int)((theoreticalMove.z - terrainStartPos.z) / QUADSIZE) * terrain.tiny[0].size_x +
				(int)((theoreticalMove.x - terrainStartPos.x) / QUADSIZE);

	if (index < 0)
		index = 0; // Ska inte se ut såhär!!! bara för att kolla om det löser crash och vad som händer.------------------------------------------------------
	if (index > 65536)
		index = 65535;

	if(index % terrain.tiny[0].size_x == terrain.tiny[0].size_x - 1)
		index -= 1;
	if(index + terrain.tiny[0].size_x + 1 > terrain.tiny[0].size)
		index -= terrain.tiny[0].size_x;

	glm::vec2 point1 = glm::vec2((int)((theoreticalMove.z - terrainStartPos.z) / QUADSIZE) * terrain.tiny[0].size_x + QUADSIZE, (int)((theoreticalMove.x - terrainStartPos.x) / QUADSIZE));
	glm::vec2 point2 = glm::vec2(point1.x - QUADSIZE, point1.y + QUADSIZE);
	glm::vec2 playerPos = glm::vec2(theoreticalMove.x, theoreticalMove.z);

	glm::vec3 norm;

	if(glm::length(playerPos - point1) < glm::length(playerPos - point2))
		norm = glm::cross(glm::vec3(0, terrainHeights->at(index) - terrainHeights->at(index + terrain.tiny[0].size_x + 1), -QUADSIZE),
			glm::vec3(-QUADSIZE, terrainHeights->at(index) - terrainHeights->at(index + 1), -QUADSIZE));

	else
		norm = glm::cross(glm::vec3(0, terrainHeights->at(index) - terrainHeights->at(index + terrain.tiny[0].size_x), -QUADSIZE),
			glm::vec3(-QUADSIZE, terrainHeights->at(index) - terrainHeights->at(index + terrain.tiny[0].size_x + 1), -QUADSIZE));

	norm = glm::normalize(norm);

	float terrainY;
	float xmod = (float)((int)theoreticalMove.x % QUADSIZE) + theoreticalMove.x - (int)theoreticalMove.x;
	float zmod = (float)((int)theoreticalMove.z % QUADSIZE) + theoreticalMove.z - (int)theoreticalMove.z;
	float xAxis = (xmod < 0 ? 20 - abs(xmod) : xmod) / 20, zAxis = (zmod < 0 ? 20 - abs(zmod) : zmod) / 20;

	uint sizeX = terrain.tiny[0].size_x;
	float xLower = terrainHeights->at(index) * (1 - xAxis) + terrainHeights->at(index + 1) * xAxis;
	float xUpper = terrainHeights->at(index + sizeX) * (1 - xAxis) + terrainHeights->at(index + sizeX + 1) * xAxis;

	terrainY = xLower * (1 - zAxis) + xUpper * zAxis + 15;

	double angle = acos(glm::length(glm::cross(norm, glm::vec3(0, 1, 0))));


	if(terrainY - player.position.y < -2)
	{
		applyGravity(_dt);
		return;
	}
	else if(inTheAir && terrainY >= player.position.y)
	{
		verticalForce = 0;
		inTheAir = false;
		player.position.y = terrainY;
		return;
	}
	else if(inTheAir)
	{
		applyGravity(_dt);
		return;
	}
	else if(terrainY >= player.position.y)
		player.position.y = terrainY;

	if(player.velocity == 0)
	{
		if(angle < 0.75 && !inTheAir)
			glide(_dt, norm);

		return;
	}

	if(!inTheAir && player.velocity == 0) 
		return;

	glm::vec3 dist = glm::normalize(theoreticalMove - player.position);

	// Cannot climb steep hills
	if(angle < 0.75 && !inTheAir && terrainY + 15 > player.position.y + dist.y * _dt * player.velocity)
	{
		// Unless we move forward, the player will glide. Uncomment to remove this functionallity
		glide(_dt, norm);
		return;
	}
	else if(angle < 0.75 && !inTheAir && terrainY + 15 > player.position.y)
	{
		glide(_dt, norm);
		return;
	}

	player.position += dist * (float)_dt * player.velocity;
	player.position.y = terrainY;

	lastFrameGlide = false;
}

void PlayerAspect::glide(float _dt, glm::vec3 _terrainNormal)
{
	// Get the angles for this triangle
	glm::vec3 crossX, crossY, crossZ;
	crossZ = glm::cross(_terrainNormal, glm::vec3(1, 0, 0));
	crossY = glm::cross(_terrainNormal, glm::vec3(0, 1, 0));
	crossX = glm::cross(_terrainNormal, glm::vec3(0, 0, 1));

	if(glm::length(crossZ) > 1) crossZ *= 0.975f;
	if(glm::length(crossY) > 1) crossY *= 0.975f;
	if(glm::length(crossX) > 1) crossX *= 0.975f;

	// Cross product will not return a normalized vector, so we can use acos on the length
	double angleX = acos(glm::length(crossX));
	double angleY = acos(glm::length(crossY));
	double angleZ = acos(glm::length(crossZ));

	// Divide by 0.5PI to normalize the value
	glm::vec3 angles = glm::normalize(glm::vec3(1 - angleX / (PI * 0.5), 1 - angleY / (PI * 0.5), 1 - angleZ / (PI * 0.5)));

	// Since length always returns a possivite value, we need to correct the new direction so that it
	// slides the player along the hill
	if(_terrainNormal.x > 0)	angles.x *= -1;
	if(_terrainNormal.z > 0)	angles.z *= -1;

	player.position -= angles * _dt * 5.f;

	lastFrameGlide = true;
}

void PlayerAspect::updateChunk(glm::vec3 _newPos)
{
	for(int i = 0; i < 9; i++)
	{
		glm::vec3 pos = terrain->tiny[i].position;
		if(_newPos.x <= pos.x + terrain->tiny[0].size_x * (float)QUADSIZE && _newPos.x > pos.x)
		{
			if(_newPos.z <= pos.z + terrain->tiny[0].size_x * (float)QUADSIZE && _newPos.z > pos.z)
			{
				terrainHeights = &terrain->heights[terrain->tiny[i].heightsID];
				terrainStartPos = terrain->tiny[i].position;

				player.smallTerrainID = i;
				break;
			}
		}
	}
}

void PlayerAspect::applyGravity(float _dt)
{
	if(!inTheAir)
		inTheAir = true;

	// Gravitational force
	verticalForce -= 13.82f * _dt * 13.98f;
	player.position += glm::vec3(player.rotation.x * _dt * player.velocity, verticalForce * _dt, player.rotation.y * _dt * player.velocity);
}

void PlayerAspect::jump()
{
	if(!inTheAir && !lastFrameGlide)
	{
		inTheAir = true;
		// A possitive force will launch the player into the air
		verticalForce = 80;
		player.position.y++;
	}
}

void PlayerAspect::absorb(float _dt)
{	
	player.absorbingEnergy = -1;
	player.absorbingPlayer = -1;
	player.animationState = aEXTRACTING;

	glm::vec3 pos = player.position;
	std::array<glm::vec3, 20> energyResources = player.energysPos;
	std::array<glm::vec3, 10> otherPlayers = player.playersPosition;

	std::vector<AbsorbInfo> oponents, resources;

	float range = (float)QUADSIZE * 5.f;

	for(uint i = 0; i < energyResources.size(); i++)
	{
		if (glm::length(pos - energyResources[i]) < range)
			resources.push_back(AbsorbInfo(energyResources[i], i));
	}

	for(uint i = 0; i < otherPlayers.size(); i++)
	{
		if(glm::length(pos - otherPlayers[i]) < range)
			oponents.push_back(AbsorbInfo(otherPlayers[i], i));
	}

	if (oponents.size() == 0 && resources.size() == 0)
	{
		player.isDraining = false;
		return;
	}

	for(uint j = 0; j < oponents.size(); j++)
	{
		glm::vec3 dir = pos - oponents.at(j).pos;
		if(dir.x == 0 && dir.z == 0)
			continue;

		if(acos(glm::dot(glm::normalize(glm::vec2(dir.x, dir.z)), glm::vec2(player.rotation.x, player.rotation.y))) > (PI / 8) / (glm::length(dir) / (range * 2)))
			oponents.erase(oponents.begin() + j, oponents.begin() + j + 1);
		else if(glm::length(oponents.at(j).pos - pos + glm::vec3(player.rotation.x, 0, player.rotation.y) * 10.f) < glm::length(oponents.at(j).pos - pos - glm::vec3(player.rotation.x, 0, player.rotation.y) * 10.f))
			oponents.erase(oponents.begin() + j, oponents.begin() + j + 1);
	}

	for(uint j = 0; j < resources.size(); j++)
	{
		glm::vec3 dir = pos - resources.at(j).pos;
		if(dir.x == 0 && dir.z == 0)
			continue;

		if(acos(glm::dot(glm::normalize(glm::vec2(dir.x, dir.z)), player.rotation)) > (PI / 8) / (glm::length(dir) / (range * 2)))
			resources.erase(resources.begin() + j, resources.begin() + j + 1);
		else if(glm::length(resources.at(j).pos - pos + glm::vec3(player.rotation.x, 0, player.rotation.y) * 10.f) < glm::length(resources.at(j).pos - pos - glm::vec3(player.rotation.x, 0, player.rotation.y) * 10.f))
			resources.erase(resources.begin() + j, resources.begin() + j + 1);
	}

	if (oponents.size() == 0 && resources.size() == 0)
	{
		player.isDraining = false;
		return;
	}

	if (!player.isDraining)
	{
		SoundEngine::getInstance()->playPlayerSound(SoundEngine::Sound::PLAYER_ABSORB);
		player.isDraining = true;
	}

	int id = oponents.size() > 0 ? oponents.at(0).id : resources.at(0).id;
	glm::vec3 closestTarget = oponents.size() > 0 ? oponents.at(0).pos : resources.at(0).pos;
	float closestDist = glm::length(pos - closestTarget);

	bool isPlayer = oponents.size() > 0;
	for(uint j = 1; j < oponents.size(); j++)
	{
		float temp = glm::length(pos - oponents.at(j).pos);
		if(temp < closestDist)
		{
			id = oponents.at(j).id;
			closestTarget = oponents.at(j).pos;
			closestDist = temp;
			isPlayer = true;
		}
	}
	if(!isPlayer)
	{
		for(uint j = 1; j < resources.size(); j++)
		{
			float temp = glm::length(pos - resources.at(j).pos);
			if(temp < closestDist)
			{
				id = resources.at(j).id;
				closestTarget = resources.at(j).pos;
				closestDist = temp;
				isPlayer = false;
			}
		}
	}
	float energy, newEnergy;
	if(isPlayer)
	{
		if(player.playersEnergy[id] > 0)
		{
			energy = player.playersEnergy[id];
			newEnergy = energy - 50 * _dt;
			player.playersEnergy[id] = newEnergy;
			player.energyInfo.energyAmount += energy + newEnergy / 2 * newEnergy < 0 ? -1 : 1;

			network->damage(id, isPlayer ? 1 : 2, energy - newEnergy);
			player.absorbingPlayer = id;
		}
	}
	else
	{
		if(player.energysEnergy[id] > 0)
		{
			energy = player.energysEnergy[id];
			newEnergy = energy - 50 * _dt;
			player.energysEnergy[id] = newEnergy;
			player.energyInfo.energyAmount += energy + newEnergy / 2 * newEnergy < 0 ? -1 : 1;

			network->damage(id, isPlayer ? 1 : 2, energy - newEnergy);
			player.absorbingEnergy = id;
		}
	}
}

void PlayerAspect::checkKeyAndDirection(KeyState _state, bool &_keyDown)
{
	if(!_keyDown && _state == KeyState::PRESSED)
		_keyDown = true;
	else if(_keyDown && _state == KeyState::RELEASED)
		_keyDown = false;
}

void PlayerAspect::reCreateTerrain()
{
	//network->requestSpawnPos();
	terrainAspect->createTerrainFromPosition(glm::vec2(-50000, 124675));
	threadLock = false;
}

void PlayerAspect::reCreateTinys()
{
	//network->requestSpawnPos();
	terrainAspect->createTinyFromPosition();
	threadLockTiny = false;
}

void PlayerAspect::slowDeath(float _dt)
{
	if(deathCutofTime == 0.f)
	{
		//std::thread t1(&PlayerAspect::reCreateTerrain, this);
		//t1.detach();
		speedDecrease = player.velocity / (deathDuration * 0.915f);
		threadLock = true;
		threadLockTiny = true;

		//terrainHeights = &terrain->heights[terrain->tinyActive[player.smallTerrainID].heightsID];
	}

	deathCutofTime += _dt;
	if(deathCutofTime >= deathDuration && !threadLockTiny && !inTheAir)
		reset();

	float freqDecrease = _dt * (1 / (deathDuration * 0.8f));

	if(deathCutofTime >= deathDuration * 0.8f)
	{
		player.velocity = 0.f;
		idleFreq = extrationFreq = runningFreq = 1000.f;

		if(inTheAir)
			followTerrain(_dt);

		if(!threadLock)
		{
			threadLock = true;
			//std::thread t1(&PlayerAspect::reCreateTinys, this);
			//t1.detach();
			reCreateTerrain();
			reCreateTinys();
		}
	}
	else
	{
		if(player.rotation.x != 0 || player.rotation.y != 0)
		{
			player.velocity = inTheAir ? player.velocity * 0.9999f : speedDecrease * (deathDuration * 0.915f - deathCutofTime);
			if(player.animationState != aRUNNING)
			{
				player.animationState = aRUNNING;
				player.animationFrame = 0;
				player.prevAnimationFrame = 0;
			}
		}
		else
		{
			player.velocity = 0;
			if(player.animationState == aRUNNING)
			{
				player.animationState = aIDLE;
				player.animationFrame = 0;
				player.prevAnimationFrame = 0;
			}
		}		

		idleFreq += freqDecrease;
		extrationFreq += freqDecrease;
		runningFreq += freqDecrease / 5;

		followTerrain(_dt);
	}
}

void PlayerAspect::consumeEnergy(float _dt)
{
	SingleBiom currentPoint = Utility::getBlendMapValue(&bioms, player.position);

	float val = 1.25f;
	float t0, t1;
	if(adaptation.maxBiomValue() > 0.3f)
	{
		for(int i = 0; i < 4; i++)
		{
			t0 = val - currentPoint.map1[i] * adaptation.map1[i] / 2;
			t1 = val - currentPoint.map2[i] * adaptation.map2[i] / 2;
			if(t0 < val)
				val = t0;
			if(t1 < val)
				val = t1;
		}

		SingleBiom sb;
		sb.map1 = currentPoint.map1 * (glm::length(adaptation.map1) > 0 ? glm::normalize(adaptation.map1) : adaptation.map1);
		sb.map2 = currentPoint.map2 * (glm::length(adaptation.map2) > 0 ? glm::normalize(adaptation.map2) : adaptation.map2);
		player.energyInfo.idealTemperature = glm::dot(player.energyInfo.biomTemperatures1, sb.map1) + glm::dot(player.energyInfo.biomTemperatures2, sb.map2);
	}
	else
		player.energyInfo.idealTemperature = glm::dot(player.energyInfo.biomTemperatures1, currentPoint.map1) + glm::dot(player.energyInfo.biomTemperatures2, currentPoint.map2);
	
	if(player.energyInfo.idealTemperature > 30)		player.energyInfo.idealTemperature = 30;
	else if(player.energyInfo.idealTemperature < 5)	player.energyInfo.idealTemperature = 5;

	player.energyInfo.energyLossPerSecond = ((float)LENGTHOFDAY * val) / LENGTHOFDAY;

	player.energyInfo.surroundingTemperature =	glm::dot(player.energyInfo.biomTemperatures1, currentPoint.map1) + 
												glm::dot(player.energyInfo.biomTemperatures2, currentPoint.map2);

	// How much energy the player loose depends on the diffenence between the surrounding temp and ideal/current temp
	// plus a constant.
	float consumption = abs(player.energyInfo.surroundingTemperature - player.energyInfo.playerTemperature) * 
		player.energyInfo.energyLossPerDegree * player.energyInfo.energyLossPerSecond * _dt;

	float energyLoss = std::max(consumption, player.energyInfo.energyLossPerSecond) *_dt * (playerBoost ? 2.5f : 1);
	if(flashlight)	energyLoss += 1.f * _dt;
	
	player.energyInfo.energyAmount -= energyLoss;

	// the player's temperature goes towards the ideal temperature
	player.energyInfo.playerTemperature += (player.energyInfo.idealTemperature - player.energyInfo.playerTemperature) * .01f * _dt;

	float diff = player.energyInfo.idealTemperature - player.energyInfo.playerTemperature;

	if(diff <= .01f && diff >= -.01f)
		player.energyInfo.playerTemperature = player.energyInfo.idealTemperature;
}

void PlayerAspect::reset()
{
	player.energyInfo = PlayerContainer::EnergyInfo(); // maybe keep temperature data and only reset energy amount? We'll see
	player.rotation = glm::vec2(0, 0);
	verticalForce = 0;

	player.energyInfo.energyAmount = 600;

	idleFreq = extrationFreq = 0.1f;
	runningFreq = 0.02f;

	deathCutofTime = 0.f;

	player.smallTerrainID = 4;
	terrainHeights = &terrain->heights[terrain->tiny[4].heightsID];
	terrainStartPos = terrain->tiny[4].position;

	adaptation.map1 = adaptation.map2 = glm::vec4();
	dtSum = 0.f;
	updatesPerAdaptationSum = 0;

	terrainAspect->createTerrainBuffer();
	player.position = glm::vec3(-50000, 500, 124675); // will fetch spawnpoint fron the server.
	player.position.y = Utility::mapToTerrain(glm::vec2(player.position.x, player.position.z), *terrain).y;

	terrainAspect->calculateCullingFromSpawn(glm::vec2(-50000, 124675));
}

PlayerAspect::~PlayerAspect()
{
}
