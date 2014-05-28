#include "WindParticles.h"
#include "ShaderClass.h"

WindParticles::WindParticles()
	:	ParticleInterface(),
		windParticlesQue(Containers(std::make_pair(Containers::WIND, MODIFY)).get<WindContainer>(Containers::WIND)->windParticlesQue)
{
	first = true;
}

WindParticles::~WindParticles()
{
}

void WindParticles::init(RenderInterface *_graphics, shaderData* _data)
{
	graphics = _graphics;
	data = _data;

	cbParticleStartIndex = graphics->getHandleIndex("cbParticleStart",ResourceType::ConstantBuffer);
	data->shaders.push_back(new ShaderClass("VindStreamoutVS", "VindStreamoutGS", "", "", "", graphics, (int)LayoutTypes::LAYOUT_BASIC_PARTICLE, true));
	data->shaders.push_back(new ShaderClass("VindDrawVS", "VindDrawGS", "", "", "VindDrawPS", graphics, (int)LayoutTypes::LAYOUT_BASIC_PARTICLE, false));
	vertexInput input(glm::vec3(420, 10, 420), glm::vec3(0, 0, 0), 2, 0, 5, 1);

	data->bufferIndex.push_back(graphics->createVBufferInit((void**)&input, 1, sizeof(vertexInput)));
	data->bufferIndex.push_back(graphics->createVBufferStreamout(5000, sizeof(vertexInput)));
	data->bufferIndex.push_back(graphics->createVBufferStreamout(5000, sizeof(vertexInput)));

//	data->textures.push_back(graphics->createShaderResourceViewFromFile("Textures/smoke2.png"));
}

void WindParticles::update(float _deltaTime)
{
	if(windParticlesQue.size() > 0)
	{
		CBParticleStart pStart;
		pStart.effectType = 1;
		pStart.pad = 0;
		pStart.pos = windParticlesQue.back().startPosition;
		pStart.dir = windParticlesQue.back().direction;
		windParticlesQue.pop_back();
		graphics->updateCBuffer(cbParticleStartIndex, (void*)&pStart);
	}

	graphics->setCBuffers(0,1,cbParticleStartIndex);
	data->shaders.at(0)->use();

	graphics->setGeometryShaderResource(0, graphics->getRandomSeedIndex(), 1);

	graphics->setInputLayout((int)LayoutTypes::LAYOUT_BASIC_PARTICLE);

	graphics->setTopology(1);

	UINT stride = { sizeof(vertexInput) };
	UINT offset = { 0 };

	if(first == true)
	{
		graphics->setVertexBuffers(0, data->bufferIndex.at(0), 1, &stride, &offset);
	}
	else
	{
		graphics->setVertexBuffers(0, data->bufferIndex.at(1), 1, &stride, &offset);
	}

	graphics->setSOTargets(1, data->bufferIndex.at(2), &offset);

	if(first == true)
	{
		graphics->draw(1, 0);
	}
	else
	{
		graphics->drawAuto();
		//graphics->draw(50,0);

	}

	graphics->resetSOTargets();

	graphics->swapBuffers(&data->bufferIndex.at(1), &data->bufferIndex.at(2));
}

void WindParticles::draw()
{
	data->shaders.at(1)->use();

	graphics->setToScreenRender(-1);
	//		int handle = graphics->getHandleIndex("postEffectRenderTarget",ResourceType::RenderTargetView);
	//graphics->setRenderTargetViews(handle,1,-1);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_BASIC_PARTICLE);

	graphics->setTopology(1);

	graphics->setRasterState(0);

	UINT stride = sizeof(vertexInput);
	UINT offset = 0;

	graphics->setPixelShaderResource(1, graphics->getHandleIndex("sandparticle", ResourceType::ShaderResourceView), 1);
	graphics->setPixelShaderResource(2, graphics->getDepthResourceView(), 1);

	//	graphics->setPixelShaderResource(0,21,1);

	graphics->setVertexBuffers(0, data->bufferIndex.at(1), 1, &stride, &offset);

	graphics->setBlendState(2);

	if(first == true)
	{
		graphics->draw(1, 0);
		first = false;
	}
	else
	{
		graphics->drawAuto();
		//graphics->draw(50,0);

	}
}