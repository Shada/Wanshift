#pragma once
#include "AspectInterface.h"
#include "Atmosphere.h"
#include "ShaderClass.h"
#include "RenderInterface.h"
#include "SkyDome.h"
#include "WanInput/InputInterface.h"

class SkyAspect :
	public AspectInterface
{
public:
	SkyAspect();
	~SkyAspect(void);

	void init(MainState& _mainState)override;

protected:
	/* all internal data processing for derived classes are to be defined here */
	void runAspect(float _dt)override;
	int cbWorldAttributesIndex;
	int cbScatterAttributesIndex;
public:
	void draw();
	
	RenderInterface*	graphics;
	ShaderClass*		skyShader;
	ShaderClass*		moonShader;
	Atmosphere			atmosphere;
	SkyDome				skyDome;

	int					sunPositionBufferIndex;

	int	renderTargetIndex;
	int textureIndex;
	int shaderResourceIndex;
	int dIndex;
	
	int precomCBufferIndex;

	InputInterface* input;
	Containers* c;
	int renderTarget, renderTexture;

	std::string newSky, oldSky;
	ScatterType newScatter, oldScatter, currentScatter;
};
