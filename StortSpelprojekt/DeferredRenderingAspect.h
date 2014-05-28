#pragma once
#include "aspectinterface.h"

#include "RenderInterface.h"
#include "ShaderClass.h"

#include "SkysphereAspect.h"
#include "SkyAspect.h"
#include "CascadeShadow.h"
#include "ForestBiom.h"
#include "SwampBiom.h"
#include <Model.h>

#define numberOfRenderTargets 4

class DeferredRenderingAspect :
	public AspectInterface
{
public:
	DeferredRenderingAspect();
	~DeferredRenderingAspect();

	void runAspect(float _dt)override;

	void init(MainState& _mainState)override;

private:
	/*	Runs the first pass in the deferred rendering and writes
		to the COLOR, NORMAL and SPECULAR maps and creates a depth stencil*/
	void passOne(float _dt);

	/*	Uses the maps and depth stencil created in pass one to render the final scene*/
	void passTwo();

	RenderInterface *graphics;

	ShaderClass *terrainIndex;
	ShaderClass *passTwoShaderIndex;
	ShaderClass *blurHorizontalIndex;
	ShaderClass *blurVerticalIndex;
	ShaderClass *tessellation;
	ShaderClass *skysphereShaderIndex;
	ShaderClass *terrainScatter;
	ShaderClass *animationShaderIndex;
	ShaderClass *modelShaderIndex;

	PlayerContainer* player;
	DetailLayerContainer* terrainDetail;
	TerrainContainer *terrain;
	CameraContainer *camera;
	ForestContainer *forestContainer;
	EnemyContainer *enemyContainer;

	Model m;
	ForestBiom forest;
	SwampBiom swamp;
	
	SkysphereAspect *skysphereAspect;
	// </temporary shit>

	int renderTargetViewPointer;
	int shaderResourceViewPointer;

	int sceneRenderTarget;
	int sceneResource;

	int renderTargetViewPointerBlurH; // horizontal will write to this
	int renderTargetViewPointerBlurV; // vertical will write to this
	int shaderResourceViewPointerBlurH; // vertical will read from this
	int shaderResourceViewPointerBlurV; // deferred pass 2 will read from this

	int cbWorldIndex;
	int cbViewIndex;
	int cbOnceIndex;
	int cbEveryFrameIndex;
	int cbTerrainIndex;
	int cbAnimationMatricesIndex;
	int cbLightInfoIndex;
	int cbWorldAttributesIndex;
	int cbScatterAttributesIndex;
	int cbSpotLightIndex;

	int desertTexIndex;
	int stoneTexIndex;
	int snowStoneTexIndex;
	int meadowTexIndex;
	int swampTexIndex;
	int forestTexIndex;
	int seedTexIndex;
	int heightMapIndex;
	int skyboxTexIndex;


	struct BufferInfo
	{
		int buffer;
		int bStart;
		int bAmount;
	};

	SkyAspect*	skyScatter;
	
	CascadeShadow* shadow;

	float tempTimer;
	uint currentframe;

};
