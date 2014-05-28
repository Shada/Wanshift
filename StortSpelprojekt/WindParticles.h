#pragma once

#include <random>
#include "ParticleInterface.h"
#include "Containers.h"

class WindParticles :
	public ParticleInterface
{
public:
	WindParticles(void);
	~WindParticles(void);

	/*Initiates the particle*/
	void init(RenderInterface *_graphics, shaderData* _data);

	/*Updates the particle*/
	void update(float _deltaTime);

	/*Draws the particles*/
	void draw();

private:

	int cbParticleStartIndex;
	struct vertexInput
	{
		glm::vec3 pos;
		glm::vec3 vel;
		float time;
		int type;
		float lifeTime;
		float radie;

		vertexInput(glm::vec3 _pos, glm::vec3 _vel, float _time, int _type, float _lifeTime, float _radie)
		{
			pos = _pos;
			vel = _vel;
			time = _time;
			type = _type;
			lifeTime = _lifeTime;
			radie = _radie;
		}

	};

	bool first;

	RenderInterface* graphics;
	shaderData* data;
	std::vector<vertexInput> particles;
	std::vector<WindContainer::Particle>& windParticlesQue;

};
