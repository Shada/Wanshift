#pragma once
#include "ParticleEffectContainer.h"

#define WISPSPARKLES	20
#define WISPSPERSWARM	25
#define WISPSIZE		glm::vec2(10,10)

struct EnemyContainer;

class ParticleEffect
{
private:
	RenderInterface			*graphics;
	ShaderClass				*drawTech;
	ParticleEffectContainer	*particleEffectContainer;
	EnemyContainer			*enemyContainer;
	UINT					containerIndex;
	UINT					enemyIndex;
	UINT					enemySubIndex;

public:
	ParticleEffect( UINT _particleContainerIndex, UINT _enemyIndex, UINT _enemySubIndex );
	~ParticleEffect();
	void init(RenderInterface *_graphics, ParticleEffectContainer *_particleEffectContainer, EnemyContainer *_enemyContainer, UINT _vertexIndex, UINT _vertexAmount);
	void update(float _deltaTime);
	void draw();
	void resetParticle(int _index);
	//int randomInteger(int _min, int _max);
};

