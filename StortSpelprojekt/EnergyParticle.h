#pragma once
#include "ParticleInterface.h"
#include "Containers.h"
#include "Utility.h"

#define nrOfEnergy 20

class EnergyParticle : public ParticleInterface
{
public:
	EnergyParticle();
	~EnergyParticle();

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
	PlayerContainer	&player;
	DetailLayerContainer &detail;
	ConstantBufferContainer &cBuffers;
	bool first;

	int cbParticleEnergyIndex;
	int cbLightIndex;

	struct vertexInputEnergy
	{
		glm::vec3 pos;
		int index;
		glm::vec3 goal;
		float time;
		float lifeTime;
		float radie;
		int type;
		int pad;

		vertexInputEnergy(glm::vec3 _pos, int _index, glm::vec3 _goal, float _time, float _lifeTime, float _radie, int _type)
		{
			pos = _pos;
			index = _index;
			goal = _goal;
			time = _time;
			lifeTime = _lifeTime;
			radie = _radie;
			type = _type;
			pad = 0;
		}

	};
};

