#pragma once

#include <random>
#include "NetworkAspect.h"
#include "MainState.h"
class SingleplayerAspect : public NetworkAspect
{
public:
	SingleplayerAspect();
	~SingleplayerAspect();

	void runAspect(float _dt)override;
	void init(MainState& _mainState)override;

	/* See NetworkAspect for documentation for functions*/
	void damage(uint _i, unsigned short _type, float _damage)override;
	void died()override;
private:
	ConstantBufferContainer *cbuffers;
	int cbEveryFrameIndex;
	RenderInterface *graphics;
	MainState *mainstate;
	bool morning;
	float timeOfDay;
	bool timeUp;
};