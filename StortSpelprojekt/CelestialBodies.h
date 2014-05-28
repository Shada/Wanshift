#pragma once
#include "ParticleInterface.h"
#include "Containers.h"

#define nrOfPlanets 2
#define planetDistance 160000

class CelestialBodies :
	public ParticleInterface
{
public:
	CelestialBodies();
	~CelestialBodies();

	/*Initiates the particle*/
	void init(RenderInterface *_graphics, shaderData* _data);

	/*Updates the particle*/
	void update(float _deltaTime);

	/*Draws the particles*/
	void draw();
private:
	RenderInterface* graphics;
	shaderData* data;
	Containers	c;
	PlayerContainer &player;

	float timeOfDay;

	struct vertexInputCelestial
	{
		glm::vec3 pos;
		float radie;
		int texture;

		vertexInputCelestial()
		{}

		vertexInputCelestial(glm::vec3 _pos, float _radie, int _texture)
		{
			pos = _pos;
			radie = _radie;
			texture = _texture;
		}

	};

	vertexInputCelestial input[nrOfPlanets];
	glm::vec3 positions[nrOfPlanets];
	glm::mat4 rotMatrix[nrOfPlanets];
};

