#pragma once
#include "TerrainContainer.h"
#include <random>
#include "BaseBiom.h"
#include "DetailNode.h"
class ForestBiom : public BaseBiom
{
public:
	ForestBiom();
	void init(RenderInterface *_graphics);
	void generateTrees(int _seed);
	void update(float _deltaTime);
	void draw(UINT renderTargetIndex, UINT amount, UINT depthBufferIndex);
	void drawForward(UINT renderTargetIndex, UINT amount, UINT depthBufferIndex);
	void drawShadow();
	~ForestBiom();
private:
	int once, view, bark, barkNorm, barkSpec, branch, roof;
	DetailNode *quadTree;
	bool setSingleHeight(std::array<Chunk<CHUNKSIZE, CHUNKSIZE>, 9> *_chunks, float _size, glm::vec3 &_pos);
};

