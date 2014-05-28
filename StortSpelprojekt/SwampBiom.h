#pragma once
#include "BaseBiom.h"
class SwampBiom : public BaseBiom
{
public:
	SwampBiom();
	void init(RenderInterface *_graphics);
	void generateTrees(int _seed);
	void update(float _deltaTime);
	void draw(UINT renderTargetIndex, UINT amount, UINT depthBufferIndex);
	void drawWater(UINT _renderTargetIndex, UINT _sceneResourceIndex, UINT _depthResourceIndex);
	~SwampBiom();
private:
	ShaderClass *waterTech;
	UINT numTrees;
};

