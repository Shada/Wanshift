#pragma once
#include "PostEffect.h"

class ShimmerPostEffect :
	public PostEffect
{
public:
	ShimmerPostEffect(void);
	~ShimmerPostEffect(void);

	void initPostEffect(MainState& _mainState)override;
	void preProcess(float _dt);
	void draw(float _dt, int _renderTarget);

private:
	int renderTarget;
	int renderTexture;

	float simSpeed;
	float diff;
	float limit;
};

