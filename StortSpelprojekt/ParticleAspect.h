#pragma once
#include "Aspectinterface.h"
#include "RenderInterface.h"
#include "ParticleInterface.h"
#include <vector>
#include "ParticleContainer.h"
#include "SandStormContainer.h"
#include "SandStorm.h"

class ParticleAspect :
	public AspectInterface
{
public:
	ParticleAspect();
	~ParticleAspect();

	/*	Updates and draws all the particle systems*/
	void runAspect(float _dt);
	void init(MainState& _mainState)override;
private:
	/*	Initializes all the particle systems*/
	void init();

	RenderInterface *graphics;

	std::vector<ParticleInterface*> particleSystems;

public:
	SandStorm						sandStorm;
private:

	//containers
	ParticleContainer		*particleContainer;
	SandStormContainer		*sandStormContainer;
};