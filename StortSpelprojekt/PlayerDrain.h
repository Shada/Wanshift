#pragma once
#include "ParticleInterface.h"
#include "Containers.h"
#include <vector>
#include "Utility.h"

#define nrOfParticles 500
#define nrOfPlayers 10

class PlayerDrain : public ParticleInterface
{
public:
	PlayerDrain();
	~PlayerDrain();

	/*Initiates the particle*/
	void init(RenderInterface *_graphics, shaderData* _data);

	/*Updates the particle*/
	void update(float _deltaTime);

	/*Draws the particles*/
	void draw();

private:

	Containers	c;
	PlayerContainer	&player;
	RenderInterface* graphics;
	EnemyContainer &enemy;

	shaderData* data;
	int vertexBuffer;

	float timeOfDay;

	struct vertexInput
	{
		glm::vec3 pos;
		float time;
		float radie;
		int to;
		int type;

		vertexInput()
		{}

		vertexInput(glm::vec3 _pos, float _time, float _radie, int _to, int _type)
		{
			pos = _pos;
			time = _time;
			radie = _radie;
			to = _to;
			type = _type;
		}

	};

	vertexInput particleList[nrOfParticles];

};

