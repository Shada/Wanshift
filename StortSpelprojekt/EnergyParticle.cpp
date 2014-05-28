#include "EnergyParticle.h"

EnergyParticle::EnergyParticle() : ParticleInterface(), c(std::make_pair(Containers::TERRAIN, READ)),
player(*c.get<PlayerContainer>(Containers::PLAYER)), detail(*c.get<DetailLayerContainer>(Containers::DETAIL_LAYER)), cBuffers(*c.get<ConstantBufferContainer>(Containers::CONSTBUFFER))
{
	first = true;

}

EnergyParticle::~EnergyParticle()
{
}

void EnergyParticle::init(RenderInterface *_graphics, shaderData* _data)
{
	graphics = _graphics;
	data = _data;

	data->shaders.push_back(new ShaderClass("EnergyStreamoutVS", "EnergyStreamoutGS", "", "", "", graphics, (int)LayoutTypes::LAYOUT_ENERGY_PARTICLE, true));
	data->shaders.push_back(new ShaderClass("EnergyDrawVS", "EnergyDrawGS", "", "", "EnergyDrawPS", graphics, (int)LayoutTypes::LAYOUT_ENERGY_PARTICLE, false));

	cbParticleEnergyIndex = graphics->getHandleIndex("cbParticleEnergy", ResourceType::ConstantBuffer);
	cbLightIndex = graphics->getHandleIndex("cbLightInfo", ResourceType::ConstantBuffer);

	glm::vec3 pos;
	
	pos = glm::vec3(0,0,0);
	
	vertexInputEnergy input(pos, 0, glm::vec3(0,0,0), 2, 0, 50, 0);

	data->bufferIndex.push_back(graphics->createVBufferInit((void**)&input, 1, sizeof(input)));
	data->bufferIndex.push_back(graphics->createVBufferStreamout(2000, sizeof(input)));
	data->bufferIndex.push_back(graphics->createVBufferStreamout(2000, sizeof(input)));

	//cBuffers.cbLight.lightCount += 20;


	cBuffers.energyLightCount = 20;

	for(uint i = 0; i < cBuffers.energyLightCount; i++)
	{
		cBuffers.cbLight.pos[i] = glm::vec4(0, -5000, 0, 1);
		cBuffers.cbLight.color[i] = glm::vec4(1, 1, 1, 1);
		cBuffers.cbLight.reach[i] = glm::vec4(400, 400, 400, 1);
	}

}

void EnergyParticle::update(float _deltaTime)
{
	CBParticleEnergy pStart;
	pStart.pad = 0;
	pStart.lifeTime = 0;
	pStart.radie = 5;
	pStart.time = 2;

	if(player.isDraining)
	{
		pStart.goal[player.absorbingEnergy] = glm::vec4(player.position, 1);
	}
	for (int i = 0; i < nrOfEnergy; i++)
	{
		pStart.pos[i] = glm::vec4(player.energysPos[i], 1);
		cBuffers.cbLight.pos[i] = glm::vec4(player.energysPos[i], 1);
	}

	graphics->updateCBuffer(cbLightIndex, (void*)&cBuffers.cbLight);
	graphics->updateCBuffer(cbParticleEnergyIndex, (void*)&pStart);

	graphics->setCBuffers(0, 1, cbParticleEnergyIndex);
	data->shaders.at(0)->use();

	graphics->setGeometryShaderResource(0, graphics->getRandomSeedIndex(), 1);

	graphics->setInputLayout((int)LayoutTypes::LAYOUT_ENERGY_PARTICLE);

	graphics->setTopology(1);

	UINT stride = { sizeof(vertexInputEnergy) };
	UINT offset = { 0 };

	if (first == true)
	{
		graphics->setVertexBuffers(0, data->bufferIndex.at(0), 1, &stride, &offset);
	}
	else
	{
		graphics->setVertexBuffers(0, data->bufferIndex.at(1), 1, &stride, &offset);
	}

	graphics->setSOTargets(1, data->bufferIndex.at(2), &offset);

	if (first == true)
	{
		graphics->draw(1, 0);
	}
	else
	{
		graphics->drawAuto();
	}

	graphics->resetSOTargets();

	graphics->swapBuffers(&data->bufferIndex.at(1), &data->bufferIndex.at(2));
}


void EnergyParticle::draw()
{
	data->shaders.at(1)->use();

	graphics->setToScreenRender(-1);
		//int handle = graphics->getHandleIndex("postEffectRenderTarget",ResourceType::RenderTargetView);
	//graphics->setRenderTargetViews(handle,1,-1);

	graphics->setInputLayout((int)LayoutTypes::LAYOUT_ENERGY_PARTICLE);

	graphics->setTopology(1);

	graphics->setRasterState(0);

	UINT stride = sizeof(vertexInputEnergy);
	UINT offset = 0;

	graphics->setPixelShaderResource(1, graphics->getHandleIndex("planetcirclefuzzy", ResourceType::ShaderResourceView), 1);
	graphics->setPixelShaderResource(2, graphics->getDepthResourceView(), 1);

	graphics->setVertexBuffers(0, data->bufferIndex.at(1), 1, &stride, &offset);

	graphics->setBlendState(2);

	if (first == true)
	{
		graphics->draw(1, 0);
		first = false;
	}
	else
	{
		graphics->drawAuto();
	}
}