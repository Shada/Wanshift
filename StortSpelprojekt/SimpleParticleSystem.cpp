#include "SimpleParticleSystem.h"
#include "ShaderClass.h"

SimpleParticleSystem::SimpleParticleSystem() : ParticleInterface()
{
	first = true;
}


SimpleParticleSystem::~SimpleParticleSystem()
{
}

void SimpleParticleSystem::init(RenderInterface *_graphics, shaderData* _data)
{
	graphics = _graphics;
	data = _data;

	data->shaders.push_back(new ShaderClass("SimpleSOVS", "SimpleSOGS", "", "", "", graphics, (int)LayoutTypes::LAYOUT_BASIC_PARTICLE, true));
	data->shaders.push_back(new ShaderClass("SimpleParticleVS", "SimpleParticleGS", "", "", "SimpleParticlePS", graphics, (int)LayoutTypes::LAYOUT_BASIC_PARTICLE, false));
	vertexInput input(glm::vec3(420, 200, 420), glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));

	data->bufferIndex.push_back(graphics->createVBufferInit((void**)&input, 1, sizeof(vertexInput)));
	data->bufferIndex.push_back(graphics->createVBufferStreamout(20, sizeof(vertexInput)));
	data->bufferIndex.push_back(graphics->createVBufferStreamout(20, sizeof(vertexInput)));

}

void SimpleParticleSystem::update(float _deltaTime)
{
	data->shaders.at(0)->use();

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

void SimpleParticleSystem::draw()
{
	data->shaders.at(1)->use();

	graphics->setToScreenRender(-1);

	//int handle = graphics->getHandleIndex("postEffectRenderTarget",ResourceType::RenderTargetView);
	//graphics->setRenderTargetViews(handle,1,0);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_BASIC_PARTICLE);

	graphics->setTopology(1);

	UINT stride = sizeof(vertexInput);
	UINT offset = 0;

	graphics->setVertexBuffers(0, data->bufferIndex.at(1), 1, &stride, &offset);

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