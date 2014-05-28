#pragma once

#include "RenderInterface.h"
#include "Model.h"
#include "Containers.h"
#include "ForestBiom.h"

//#include "PlayerContainer.h"
//#include "TerrainContainer.h"
//#include "ShaderClass.h"
//#include "CameraContainer.h"
#include "ShadowContainer.h"

#define NROFCASCADES 4
#define SHADOWMAPSIZE 1024

class CascadeShadow
{
public:
	CascadeShadow();
	~CascadeShadow();

	void init(RenderInterface *graphics);
	void setModel(Model* _m){ m = _m; };
	void setCamera(CameraContainer* _camera){ camera = _camera; }
	void setTerrain(TerrainContainer* _terrain){ terrain = _terrain; }
	void setPlayer(PlayerContainer* _player){ player = _player; }
	void setForest(ForestBiom* _forest){ forest = _forest; }

	///*	Uses stencil buffer to create shading using volume shadows*/
	void shadowing();

	void compileShadow();

	void updateLight(float _dt);

	int cbLightInfoIndex;
	int cbAnimationMatricesIndex;
	int cbWorldIndex;


private:
	void ExtractFrustumPoints();
	void ExtractFrustumBoundSphere(float fNear, float fFar, glm::vec3& vBoundCenter, float& fBoundRadius);
	bool cascadeNeedsUpdate(glm::mat4 mShadowView, int iCascadeIdx, glm::vec3* newCenter, glm::vec3* vOffset);
	glm::mat4 orthoCalc(float w, float h, float zf, float zn);

	void blurShadow();

	void draw();
	void update();

	RenderInterface *graphics;

	Model *m;
	TerrainContainer* terrain;
	PlayerContainer* player;
	CameraContainer* camera;
	ShadowContainer shadowConstants;
	ForestBiom* forest;

	Frustum frustum;

	float timeOfDay;
	glm::vec4 lightDir;
	//CBLightInfoData lightTest;
	ConstantBufferContainer *cbuffers;

	bool timeUp;

	int shadowTextureArray;
	int shadowDepthStencilViewArray;
	int shadowShaderResourceViewArray;

	int shadowCompileTexture;
	int shadowCompileRenderTarget;
	int shadowCompileResourceView;

	int computeIndex;
	int computeUAV;
	int computeTexture;
	int computeSRV;

	glm::vec3 shadowSphereCenter;
	float shadowSphereRadie;
	glm::vec3 arrFrustumPoints[8];
	glm::vec3 sunAxis;

	float frustumScales[NROFCASCADES + 1];
	glm::vec3 cascadeSphereCenter[NROFCASCADES];
	float cascadeSphereRadie[NROFCASCADES];

	glm::mat4 globalShadowView;
	glm::mat4 globalShadowProjection;

	glm::mat4 worldToShadow;
	glm::mat4 localShadowProjection[NROFCASCADES];


	ShaderClass *shadowmapIndex;
	ShaderClass *shadowcharIndex;
	ShaderClass *shadowTreeIndex;
	ShaderClass *shadowmapCompileIndex;
};

