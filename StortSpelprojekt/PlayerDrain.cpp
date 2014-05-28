#include "PlayerDrain.h"


PlayerDrain::PlayerDrain() : ParticleInterface(), c(std::make_pair(Containers::PLAYER, READ)), player(*c.get<PlayerContainer>(Containers::PLAYER)), enemy(*c.get<EnemyContainer>(Containers::ENEMY))
{
}


PlayerDrain::~PlayerDrain()
{
}

void PlayerDrain::init(RenderInterface *_graphics, shaderData* _data)
{
	graphics = _graphics;
	data = _data;

	data->shaders.push_back(new ShaderClass("DrainDrawVS", "DrainDrawGS", "", "", "DrainDrawPS", graphics, (int)LayoutTypes::LAYOUT_DRAIN_PARTICLE, false));

	data->bufferIndex.push_back(graphics->createVBufferDynamic(nrOfParticles, sizeof(vertexInput)));

	for (int i = 0; i < nrOfParticles; i++)
	{
		particleList[i].pos = glm::vec3(0,0,0);
		particleList[i].radie = 5;
		particleList[i].time = 0;
		particleList[i].to = 0;
		particleList[i].type = 0;
	}
}

void PlayerDrain::update(float _deltaTime)
{
	TerrainContainer &terrain(*c.get<TerrainContainer>(Containers::TERRAIN));
	float timeStep = _deltaTime / 1000;

	int temp = 100;
	timeOfDay += timeStep / temp;
	if (timeOfDay >= 1)
		timeOfDay = 0;

	for (int i = 0; i < nrOfPlayers; i++)
	{
		if (player.absorbingPlayer != -1) // ska bytas för att hantera om någon spelare drainar en annan och inte bara du någon
		{
			for (int j = 0; j < nrOfParticles; j++)
			{
				if (particleList[j].time <= 0)
				{				 
					particleList[j].pos = player.playersPosition.at(player.absorbingPlayer);
					particleList[j].radie = 5;
					particleList[j].time = 1;
					particleList[j].to = i;
					particleList[j].type = 0;
					break;
				}
			}
		}
	}
	

	for (UINT i = 0; i < enemy.action.size(); i++)
	{
		if (enemy.action.at(i) == EntityAction::eaATTACK)
		{
			for (int j = 0; j < nrOfParticles; j++)
			{
				if (particleList[j].time <= 0)
				{
					particleList[j].pos = enemy.targetPos.at(i);
					particleList[j].radie = 5;
					particleList[j].time = 1;
					particleList[j].to = i;
					particleList[j].type = 1;
					break;
				}
			}
		}
	}

	for (int i = 0; i < nrOfParticles; i++)
	{
		if (particleList[i].type == 0)
		{
			particleList[i].pos = particleList[i].pos - ((particleList[i].pos - player.playersPosition.at(particleList[i].to)) * (timeStep));
		}
		else if (particleList[i].type == 1)
		{
			//particleList[i].pos = particleList[i].pos + ((particleList[i].pos - enemy.pos.at(particleList[i].to)) / (timeStep / 2));
			//particleList[i].pos = enemy.hordeunits.at(particleList[i].to * HORDESIZE).pos + enemy.pos.at(particleList[i].to);
			//particleList[i].pos = particleList[i].pos + ((particleList[i].pos - glm::vec3(0, player.position.y, 0) + enemy.pos.at(particleList[i].to)) / (timeStep / 2));// glm::vec3(0, player.position.y, 0) + enemy.pos.at(particleList[i].to);
			//particleList[i].pos += glm::vec3(0, 1, 0);
			//particleList[i].pos = player.position;
			//particleList[i].time = 1;

			//glm::vec3 move = (particleList[i].pos - (enemy.hordeunits.at(particleList[i].to * HORDESIZE).pos + enemy.pos.at(particleList[i].to)));
			//move /= (timeStep / 2);

			//particleList[i].pos += move;


			//particleList[i].pos = particleList[i].pos; // enemy.targetPos.at(i); player pos
			//particleList[i].pos = glm::vec3(0, enemy.hordeunits.at(particleList[i].to * HORDESIZE).pos.y / 10, 0) + enemy.pos.at(particleList[i].to);

			//particleList[i].pos = glm::vec3(0, player.position.y, 0) + enemy.pos.at(particleList[i].to);
			glm::vec3 onTerrain = Utility::mapToTerrain(glm::vec2(enemy.pos.at(particleList[i].to).x, enemy.pos.at(particleList[i].to).z), terrain);
			onTerrain.y += 5;
			glm::vec3 move = ((particleList[i].pos - onTerrain) * (timeStep / 2));
			particleList[i].pos -= move;

		}
		particleList[i].time -= (timeStep / 2);
	}


	//particleList[99].pos = player.position;
	//particleList[99].radie = 5;
	//particleList[99].time = 1;
	//particleList[99].to = 0;
	//particleList[99].type = 1;

	graphics->updateVBuffer(data->bufferIndex.at(0), (void**)&particleList[0], nrOfParticles, sizeof(vertexInput));

}

void PlayerDrain::draw()
{
	data->shaders.at(0)->use();

	graphics->setToScreenRender(-1);
	//int handle = graphics->getHandleIndex("postEffectRenderTarget", ResourceType::RenderTargetView);
	//graphics->setRenderTargetViews(handle, 1, -1);

	graphics->setInputLayout((int)LayoutTypes::LAYOUT_DRAIN_PARTICLE);

	graphics->setTopology(1);

	graphics->setRasterState(0);

	UINT stride = sizeof(vertexInput);
	UINT offset = 0;

	graphics->setPixelShaderResource(1, graphics->getHandleIndex("planetcirclefuzzy", ResourceType::ShaderResourceView), 1);
	graphics->setPixelShaderResource(2, graphics->getDepthResourceView(), 1);

	graphics->setVertexBuffers(0, data->bufferIndex.at(0), 1, &stride, &offset);

	graphics->setBlendState(2);

	graphics->draw(nrOfParticles, 0);
}