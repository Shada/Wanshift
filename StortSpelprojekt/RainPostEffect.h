#pragma once

#include "PostEffect.h"
#include "RainSimContainer.h"

class Containers;

class RainPostEffect
	: public PostEffect
{
public:
	RainPostEffect(UINT _groupSize = 4, UINT _gridSizeMultiple = 32, UINT _heightMapSize = 512);
	~RainPostEffect(void);

	void setArgs(void* _argData)override;

private:
	void updateSimConstants(float _dt);
	void updateRainConstants(float _dt);

	void updateTransforms();
	void preProcess(float _dt) override;
	void draw(float _dt, int _renderTarget) override;
	void initPostEffect(MainState& _mainState) override;

	int noiseTextureIndex;
	int rainTextureIndex;

	int rainSimBuffer;	
	int rainSimBufferView;
	int rainBufferUAV;
	int computeShaderIndex;

	int		maxRainDrops;
	float	rainDensity;
	float	maxWindVariance;
	float	maxWindEffect;

	Containers* containers;

	RainSimVariables	rsv1;
	RainSimVariables	rsv2;

	CBRainConstants currentRainState;
	CBRainSimulationConstants currentSimulationState;
	RainSimSettings	simSettings;
	float lerpTime;
	int renderTexture;
	int renderTarget;
	std::string currentType;
	void generateHeightMap();
	void setRainState(const std::string& _weatherType, const std::string& _startState = "Light", const std::string& _endState = "Heavy");
};

