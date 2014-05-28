#include "Atmosphere.h"
#include "Containers.h"

Atmosphere::Atmosphere()
{
	Containers containers(std::make_pair(Containers::CONSTBUFFER, MODIFY));

	constantbuffers = containers.get<ConstantBufferContainer>(Containers::CONSTBUFFER);
	atmospheres = containers.get<AtmosphereContainer>(Containers::ATMOSPHERE_SIM);
}

Atmosphere::~Atmosphere()
{
}

void Atmosphere::setQuality(RenderInterface* _graphics)
{
	AtmosphereQuality aq = atmospheres->getAtmosphereQuality();
	constantbuffers->cbOnce.nSamples = aq.nSamples;
	constantbuffers->cbOnce.scaleDepth = aq.scaleDepth;
	constantbuffers->cbOnce.scaleOverDepth = aq.scaleOverDepth;
	int qsIndex = _graphics->getHandleIndex("cbOnce",ResourceType::ConstantBuffer);
	_graphics->updateCBuffer(qsIndex, &constantbuffers->cbOnce);
}

void Atmosphere::setWorldAttributes(RenderInterface* _graphics)
{
	AtmosphereWorld aw = atmospheres->getAtmosphereWorld();
	_graphics->updateCBuffer(waIndex, &aw);
}

void Atmosphere::setScatterAttributes(RenderInterface* _graphics, ScatterType _current)
{
	_graphics->updateCBuffer(_graphics->getHandleIndex("cbScatterAttributes", ResourceType::ConstantBuffer), &_current);
}

void Atmosphere::init(RenderInterface* _graphics)
{
	
	waIndex = _graphics->getHandleIndex("cbWorldAttribute",ResourceType::ConstantBuffer);
	saIndex = _graphics->getHandleIndex("cbScatterAttributes",ResourceType::ConstantBuffer);

	setQuality(_graphics);
	setWorldAttributes(_graphics);
	//setScatterAttributes(_graphics, 0, 1, 0);

	printf("Atmosphere CBuffer Sanity Check\n");
	printf("QS = %i\nWA = %i\nSA = %i\n", sizeof(QualitySettings), sizeof(CBWorldAttributes), sizeof(CBScatterAttributes));
}

void Atmosphere::update(RenderInterface* _graphics, float _dt)
{	
	//setQuality(_graphics);
	setWorldAttributes(_graphics);

	//printf("Quality: %f | %f | %f\n", constantbuffers->cbOnce.scaleDepth,constantbuffers->cbOnce.scaleOverDepth,constantbuffers->cbOnce.nSamples);
}