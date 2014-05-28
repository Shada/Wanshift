#pragma once
#include "TerrainContainer.h"
#include "ForestContainer.h"
#include "Containers.h"
#include <random>
#include "ShaderClass.h"

class BaseBiom
{
public:
	BaseBiom();
	virtual void init(RenderInterface *_graphics);
	virtual void generateTrees(int _seed) = 0;
	virtual void draw(UINT renderTargetIndex, UINT amount, UINT depthBufferIndex) = 0;

protected:
	TerrainContainer	*terrainContainer;
	ForestContainer		*forestContainer;	
	RenderInterface		*graphics;

	glm::vec3			basePos;
	ShaderClass			*treeTech;
	ShaderClass			*surfaceTech;

	int containerIndex;

	int					instancingIndexTree;
	int					bufferIndexGrass;
	int					instancingIndexGrass;

	std::mt19937 randomNumberGenerator;
	std::uniform_int_distribution<int> range;

	void createBuffers(std::vector<ModelInstance>& temp);

public:
	~BaseBiom();
};

