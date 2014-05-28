#include "CelestialBodies.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

CelestialBodies::CelestialBodies() : ParticleInterface(), c(std::make_pair(Containers::PLAYER, READ)), player(*c.get<PlayerContainer>(Containers::PLAYER))
{
	timeOfDay = 0;
}


CelestialBodies::~CelestialBodies()
{
}


void CelestialBodies::init(RenderInterface *_graphics, shaderData* _data)
{
	graphics = _graphics;
	data = _data;

	data->shaders.push_back(new ShaderClass("CelestialDrawVS", "CelestialDrawGS", "", "", "CelestialDrawPS", graphics, (int)LayoutTypes::LAYOUT_CELESTIAL, false));

	data->bufferIndex.push_back(graphics->createVBufferDynamic(nrOfPlanets, sizeof(vertexInputCelestial)));

	positions[0] = glm::vec3(1, 0, 0);
	input[0].radie = 40000;
	input[0].pos = glm::vec3(0,1,0) * (float)planetDistance;
	input[0].texture = 0;
	rotMatrix[0] = glm::rotate(0.0f,glm::vec3(0,1,0));

	positions[1] = glm::vec3(0, -1, 0);
	input[1].radie = 5000;
	input[1].pos = input[0].pos + (glm::vec3(0, 1, 0) * input[1].radie);
	input[1].texture = 1;
	rotMatrix[1] = glm::rotate(0.0f, glm::vec3(0, 1, 0));


}

void CelestialBodies::update(float _deltaTime)
{
	float timeStep = _deltaTime / 1000;

	int temp = 100;
	timeOfDay += timeStep / temp;
	if (timeOfDay >= 1)
		timeOfDay = 0;
	
	// planet one
	rotMatrix[0] = glm::rotate(glm::mat4(), timeStep / temp * 360, glm::vec3(0, 1, 0));
	positions[0] = glm::vec3(glm::vec4(positions[0], 1) * rotMatrix[0]);
	input[0].pos = (glm::vec3(0, 1, 0) + positions[0]) * (float)planetDistance + glm::vec3(player.position.x, 0, player.position.y);

	// planet two
	rotMatrix[1] = glm::rotate(glm::mat4(), timeStep / temp * 360, glm::vec3(1, 0, 0));
	positions[1] = glm::vec3(glm::vec4(positions[1], 1) * rotMatrix[1]);
	input[1].pos = input[0].pos + positions[1] * (input[0].radie * 1.5f);



	graphics->updateVBuffer(data->bufferIndex.at(0), (void**)&input[0], nrOfPlanets, sizeof(vertexInputCelestial));
}


void CelestialBodies::draw()
{
	data->shaders.at(0)->use();
	graphics->setDepthStencilState(3);
	graphics->setToScreenRender(-1);
	//int handle = graphics->getHandleIndex("postEffectRenderTarget",ResourceType::RenderTargetView);
	//graphics->setRenderTargetViews(handle,1,-1);

	graphics->setInputLayout((int)LayoutTypes::LAYOUT_CELESTIAL);

	graphics->setTopology(1);

	graphics->setRasterState(0);

	UINT stride = sizeof(vertexInputCelestial);
	UINT offset = 0;

	graphics->setPixelShaderResource(1, graphics->getDepthResourceView(), 1);
	graphics->setPixelShaderResource(2, graphics->getHandleIndex("planetcircle", ResourceType::ShaderResourceView), 1);
	graphics->setPixelShaderResource(3, graphics->getHandleIndex("planettex1", ResourceType::ShaderResourceView), 1);
	graphics->setPixelShaderResource(4, graphics->getHandleIndex("planettex2", ResourceType::ShaderResourceView), 1);

	graphics->setVertexBuffers(0, data->bufferIndex.at(0), 1, &stride, &offset);

	graphics->setBlendState(3);

	graphics->draw(nrOfPlanets, 0);

}