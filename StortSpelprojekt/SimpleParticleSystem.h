#pragma once
#include "ParticleInterface.h"

class SimpleParticleSystem :
	public ParticleInterface
{
public:
	SimpleParticleSystem();
	~SimpleParticleSystem();

	/*Initiates the particle*/
	void init(RenderInterface *_graphics, shaderData* _data);

	/*Updates the particle*/
	void update(float _deltaTime);

	/*Draws the particles*/
	void draw();

private:

	struct vertexInput
	{
		glm::vec3 pos;
		glm::vec3 vel;
		glm::vec3 color;

		vertexInput(glm::vec3 _pos, glm::vec3 _vel, glm::vec3 _color)
		{
			pos = _pos;
			vel = _vel;
			color = _color;
		}

	};

	bool first;

	RenderInterface* graphics;
	shaderData* data;
	std::vector<vertexInput> particles;
};
