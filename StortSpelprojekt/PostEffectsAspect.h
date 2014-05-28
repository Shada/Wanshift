#pragma once

#include "AspectInterface.h"
#include "PostEffect.h"
#include"ParticleAspect.h"
#include "ShimmerPostEffect.h"

class MainState;

class PostEffectsAspect
	: public AspectInterface
{
public:
	PostEffectsAspect(void);
	~PostEffectsAspect(void);

	void init(MainState& _mainState) override;
	ParticleAspect* softies;	// soft particles

private:
	//std::vector<PostEffect*> postEffects;
	std::map<std::string, PostEffect*> postEffects;
	
	void runAspect(float _dt) override;

	int renderTarget;
	int renderTexture;

	RenderInterface* graphics;

	ConstantBufferContainer *cbContainer;
};

