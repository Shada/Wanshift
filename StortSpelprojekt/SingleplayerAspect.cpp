#include "SingleplayerAspect.h"
#include "Utility.h"

SingleplayerAspect::SingleplayerAspect()
{
	for(auto& i : Containers(std::make_pair(Containers::TERRAIN, MODIFY)).get<TerrainContainer>(Containers::TERRAIN)->chunkSeeds)
	{
		for(auto& seed : i)
			seed = std::random_device()();
	}
	player.position = glm::vec3(40000, 0, -100979);
	//player.position = glm::vec3((rand() % 1000000) - 500000, 0, (rand() % 1000000) - 500000);
}

SingleplayerAspect::~SingleplayerAspect()
{}

void SingleplayerAspect::init(MainState& _mainState)
{
	PlayerContainer *player = c.get<PlayerContainer>(Containers::PLAYER);
	TerrainContainer &terrain(*c.get<TerrainContainer>(Containers::TERRAIN));
	EnemyContainer *npc = c.get<EnemyContainer>(Containers::ENEMY);
	BiomsContainer *bioms = c.get<BiomsContainer>(Containers::BIOMS);

	Containers containers(std::make_pair(Containers::CONSTBUFFER, MODIFY));
	cbuffers = containers.get<ConstantBufferContainer>(Containers::CONSTBUFFER);
	cbEveryFrameIndex = _mainState.graphics->getHandleIndex("cbEveryframe", ResourceType::ConstantBuffer);

	graphics = _mainState.graphics;
	mainstate = &_mainState;
	
	int sign1, sign2;
	float dist;
	glm::vec3 dir;
	for(uint i = 0; i < npc->pos.size(); i++)
	{
		sign1 = rand();
		sign2 = rand();
		dir.x = (float)rand();
		dir.z = (float)rand();

		dist  = sign1 % 8000 + 4000.f;
		sign1 = sign1 % 2 ? 1 : -1;
		sign2 = sign2 % 2 ? 1 : -1;
		dir.x = sign1 * dir.x / RAND_MAX;
		dir.z = sign2 * dir.z / RAND_MAX;
		if(dir.x == 0 && dir.z == 0)
		{
			dir.x = (float)sign1;
			dir.z = (float)sign2;
		}
		dir = glm::normalize(dir);

		npc->pos[i] = player->position + dir * dist;
		npc->pos[i].y = 0;
		npc->targetPos[i] = npc->pos[i];
		npc->action[i] = EntityAction::eaIDLE;
	}
	for(int i = 0; i < 20; i++)
	{
		sign1 = rand();
		sign2 = rand();
		dir.x = (float)rand();
		dir.z = (float)rand();

		dist  = sign1 % 10000 + 5000.f;
		sign1 = sign1 % 2 ? 1 : -1;
		sign2 = sign2 % 2 ? 1 : -1;
		dir.x = sign1 * dir.x / RAND_MAX;
		dir.z = sign2 * dir.z / RAND_MAX;
		if(dir.x == 0 && dir.z == 0)
		{
			dir.x = (float)sign1;
			dir.z = (float)sign2;
		}
		dir = glm::normalize(dir);
		player->energysPos[i] = Utility::mapToTerrain(glm::vec2(player->position.x + dir.x * dist, player->position.z + dir.z * dist), terrain);
		if(player->energysPos[i].y < 50)
			player->energysPos[i].y = 70;
		else
			player->energysPos[i].y += 20;
		player->energysEnergy[i] = 200;
	}

	morning = true;

	bioms->desertStorm.direction = glm::vec2(-1, 0);
	bioms->desertStorm.position.x = player->position.x + 12000;
	bioms->desertStorm.size = 4000;
	bioms->desertStorm.speed = 100;
}

void SingleplayerAspect::runAspect(float _dt)
{
	EnemyContainer *npc = c.get<EnemyContainer>(Containers::ENEMY);
	PlayerContainer *player = c.get<PlayerContainer>(Containers::PLAYER);
	TerrainContainer &terrain(*c.get<TerrainContainer>(Containers::TERRAIN));
	BiomsContainer *bioms = c.get<BiomsContainer>(Containers::BIOMS);
	
	if (bioms->desertStorm.position.x < player->position.x - 11000)
		bioms->desertStorm.position.x = player->position.x + 11000;

	glm::vec2 vectis;
	int sign1, sign2;
	float dist;
	glm::vec3 dir;

	for(uint i = 0; i < npc->pos.size(); i++)
	{
		vectis = glm::vec2(player->position.x - npc->pos[i].x, player->position.z - npc->pos[i].z);
		dist = glm::length(vectis);
		if(dist <= 1500.f)
		{
			npc->action[i] = EntityAction::eaWALKING;
			npc->targetPos[i] = player->position;
			if(dist <= 65.f)
			{
				npc->action[i] = EntityAction::eaATTACK;
				player->energyInfo.energyAmount -= _dt / 1000.f * 5.f;
			}
			if(dist >= _dt / 1000.f * 80.f)
			{
				glm::vec2 dir = glm::normalize(vectis) * _dt / 1000.f * 80.f;
				npc->pos[i].x += dir.x;
				npc->pos[i].z += dir.y;
			}
			else
			{
				npc->pos[i].x = player->position.x;
				npc->pos[i].z = player->position.z;
			}
		}
		else
		{
			if(dist > 14000.f)
			{
				sign1 = rand();
				sign2 = rand();
				dir.x = (float)rand();
				dir.z = (float)rand();

				dist = sign1 % 8000 + 4000.f;
				sign1 = sign1 % 2 ? 1 : -1;
				sign2 = sign2 % 2 ? 1 : -1;
				dir.x = sign1 * dir.x / RAND_MAX;
				dir.z = sign2 * dir.z / RAND_MAX;
				if(dir.x == 0 && dir.z == 0)
				{
					dir.x = (float)sign1;
					dir.z = (float)sign2;
				}
				dir = glm::normalize(dir);

				npc->pos[i] = player->position + dir * dist;
				npc->pos[i].y = 0;
			}
			npc->targetPos[i] = npc->pos[i];
			npc->action[i] = EntityAction::eaIDLE;
		}
	}

	for(uint i = 0; i < 20; i++)
	{
		vectis = glm::vec2(player->position.x - player->energysPos[i].x, player->position.z - player->energysPos[i].z);
		if(vectis.x > 15000.f || vectis.y > 15000.f || player->energysEnergy[i] < 0)
		{
			sign1 = rand();
			sign2 = rand();
			dir.x = (float)rand();
			dir.z = (float)rand();

			dist = sign1 % 10000 + 5000.f;
			sign1 = sign1 % 2 ? 1 : -1;
			sign2 = sign2 % 2 ? 1 : -1;
			dir.x = sign1 * dir.x / RAND_MAX;
			dir.z = sign2 * dir.z / RAND_MAX;
			if(dir.x == 0 && dir.z == 0)
			{
				dir.x = (float)sign1;
				dir.z = (float)sign2;
			}
			dir = glm::normalize(dir);
			player->energysPos[i] = Utility::mapToTerrain(glm::vec2(player->position.x + dir.x * dist, player->position.z + dir.z * dist), terrain);
			if(player->energysPos[i].y < 50)
				player->energysPos[i].y = 70;
			else
				player->energysPos[i].y += 20;
			player->energysEnergy[i] = 200;
		}
	}

	int temp = 100;
	if (timeUp == true)
	{
		timeOfDay += cbuffers->cbEveryFrame.timeStep / temp;
		if (timeOfDay > 1)
		{
			timeOfDay = 1;
			timeUp = false;
			mainstate->cbContainer->cbEveryFrame.dayBool.x = false;
		}
	}
	else
	{
		timeOfDay -= cbuffers->cbEveryFrame.timeStep / temp;
		if (timeOfDay < 0)
		{
			timeOfDay = 0;
			timeUp = true;
			mainstate->cbContainer->cbEveryFrame.dayBool.x = true;
		}
	}

	mainstate->cbContainer->cbEveryFrame.timeofDay = timeOfDay;

}

void SingleplayerAspect::damage(uint _i, unsigned short _type, float _damage)
{}

void SingleplayerAspect::died()
{}
