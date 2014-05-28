#include "ShimmerPostEffect.h"
#include "Containers.h"
#include "Utility.h"

ShimmerPostEffect::ShimmerPostEffect(void)
	: PostEffect("shimmer"), simSpeed(5000), limit(1)
{
}


ShimmerPostEffect::~ShimmerPostEffect(void)
{
}

void ShimmerPostEffect::initPostEffect(MainState& _mainState)
{
	std::string shaderName = getName();
	shader = new ShaderClass("heatHazeVS", "", "", "", "heatHazePS", graphics, (int)LayoutTypes::LAYOUT_POSITION, false);
	glm::vec2 r = graphics->getResolution();
	int texture = graphics->createTexture2D( (int)r.x, (int)r.y);
	
	renderTarget = graphics->createRenderTargetView(texture);
	renderTexture = graphics->createShaderResourceView(texture);

	printf("WWAAAGH!: %i:%i:%i\n",texture,renderTarget,renderTexture);
	graphics->addHandle("haze", ResourceType::ShaderResourceView, renderTexture);
}

cbHeat updateHaze(float _dt, const float _simSpeed)
{
	cbHeat haze;

	haze.heatDistance = 1;
	haze.hDt = _dt;
	haze.offsetScale = 0.01f;

	static float d;
	d += _dt;

	if(d >= _simSpeed)
		d = 0;

	haze.hPadding.x = d / _simSpeed;

	return haze;
}

void ShimmerPostEffect::preProcess(float _dt)
{
	const float mult = 0.0001f;
	if(active)
	{
		if(diff < limit)
			diff += _dt * mult;

		if(diff > limit)
			diff = limit;
	}
	else
	{
		if(diff > 0)
			diff -= _dt * mult;

		if(diff < 0)
			diff = 0;
	}
}

void ShimmerPostEffect::draw(float _dt, int _renderTarget)
{
	//if(diff < 0)
	//	return;

	cbHeat haze = updateHaze(_dt, simSpeed);
	haze.offsetScale *= diff;

	int cHandle = graphics->getHandleIndex("cbHeatHaze", ResourceType::ConstantBuffer);

	graphics->updateCBuffer(cHandle, &haze);
	graphics->setCBuffers(6, 1, graphics->getHandleIndex("cbHeatHaze", ResourceType::ConstantBuffer));

	shader->use();

	graphics->setRenderTargetViews(_renderTarget,1,-1);

	graphics->setPixelShaderResource(7, graphics->getDepthResourceView(), 1);
	graphics->setPixelShaderResource(10, graphics->getHandleIndex("fakeBuffer", ResourceType::ShaderResourceView), 1);
	graphics->setPixelShaderResource(6, graphics->getHandleIndex("hazenoise", ResourceType::ShaderResourceView), 1);


	graphics->setTopology(0);
	graphics->setBlendState(2);
	graphics->setRasterState(2);

	

	graphics->setInputLayout((int)LayoutTypes::LAYOUT_POSITION);
	graphics->setNullVertexBuffers();

	graphics->draw( 6, 0 );

	graphics->setPixelShaderResource(7,-1,1);
	graphics->setPixelShaderResource(10,-1,1);
	graphics->setPixelShaderResource(6,-1,1);

	//graphics->setBlendState(0);
}