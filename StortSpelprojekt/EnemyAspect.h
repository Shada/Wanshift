#pragma once
#include "AspectInterface.h"
#include "Containers.h"
#include "ParticleEffect.h"
class EnemyAspect :public AspectInterface
{
public:
	EnemyAspect();
	~EnemyAspect();

	void runAspect(float _dt) override;
	void init(MainState &_ms) override;
	void draw();
	void update( float _dt);
	
private:

	void followTerrain(float _dt);
	void updateLights();

	int cbWispIndex;
	
	Containers		c;
	RenderInterface *graphics;
	EnemyContainer	*ec;
	PlayerContainer *pl;
	ConstantBufferContainer *cbuffers;
	ParticleEffectContainer *pe;
	ShaderClass		*wispTech;
	std::vector< ParticleEffect > particleEffects;
};