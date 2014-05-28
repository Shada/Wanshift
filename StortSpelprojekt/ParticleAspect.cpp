#include "ParticleAspect.h"
#include "ParticleContainer.h"
#include "SimpleParticleSystem.h"
#include "WindParticles.h"
#include "MainState.h"
#include "EnergyParticle.h"
#include "CelestialBodies.h"
#include "PlayerDrain.h"
#include "ConstantBufferContainer.h"

ParticleAspect::ParticleAspect() : AspectInterface("ZParticles", 0)
{}

ParticleAspect::~ParticleAspect()
{}

void ParticleAspect::init(MainState& _mainState)
{
	graphics = _mainState.graphics;
	particleContainer = new ParticleContainer();
	init();
}

void ParticleAspect::init()
{
	//add containers here and give them to the particle systems to put their data (eg, the buffer data)
	Containers containers(std::make_pair(Containers::SANDSTORM, MODIFY), std::make_pair(Containers::CONSTBUFFER, MODIFY));
	// Add aspects here
	// Do this by particleSystems.push_back( ParticleClass );
	// Dont forget to include the class
	particleSystems.push_back(new CelestialBodies());
	particleSystems.push_back(new WindParticles());
	particleSystems.push_back(new EnergyParticle());
	particleSystems.push_back(new PlayerDrain());
	

	//particleSystems.push_back(new SimpleParticleSystem());

	sandStormContainer = containers.get<SandStormContainer>(Containers::SANDSTORM);
	ConstantBufferContainer &cbContainer = *containers.get<ConstantBufferContainer>(Containers::CONSTBUFFER);
	sandStorm.init(graphics, sandStormContainer, &cbContainer);

	// Calls the init function of each particle system
	for(unsigned int i = 0; i < particleSystems.size(); i++)
	{
		particleContainer->addShader();
		particleSystems.at(i)->init(graphics, particleContainer->getShader(i));
	}
}

void ParticleAspect::runAspect(float _dt)
{
	

	for(unsigned int i = 0; i < particleSystems.size(); i++)
	{
		particleSystems.at(i)->update(_dt);
		particleSystems.at(i)->draw();
	}
	sandStorm.update(_dt);
	sandStorm.draw();

}
