#pragma once
#include "PostEffect.h"

class FogPostEffect :
	public PostEffect
{
public:
	FogPostEffect(void);
	~FogPostEffect(void);

private:
	void initPostEffect(MainState& _mainState)override;
	void preProcess(float _dt)override;
	void draw(float _dt, int _renderTarget)override;
	void setArgs(void* _argData)override;

	ShaderClass*		shader;
	RenderInterface*	graphics;

	int fogIndex;
	std::string currentFog, oldFog;
	FogSim currentFogSim, newFogSim, oldFogSim;
};

