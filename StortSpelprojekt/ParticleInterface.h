#pragma once
#include "RenderInterface.h"
#include "ParticleContainer.h"

class ParticleInterface
{
public:
	ParticleInterface();
	~ParticleInterface();

/*	Initializes the particle system
	
	#_graphics a pointer to the graphifs interface
	#_data a pointer to the data container in particleContainer*/
	virtual void init(RenderInterface *_graphics, shaderData* _data) = 0;

/*	updates the particles in the particle system
	#_deltaTime the time since the last update*/
	virtual void update(float _deltaTime) = 0;

/* draws the particles into the world*/
	virtual void draw() = 0;
};