#include "ForestBiom.h"
#include "Utility.h"

ForestBiom::ForestBiom()
{
}

void ForestBiom::init(RenderInterface *_graphics)
{
	BaseBiom::init(_graphics);
	view		= graphics->getHandleIndex("cbView",ResourceType::ConstantBuffer);
	once		= graphics->getHandleIndex("cbOnce",ResourceType::ConstantBuffer);
	bark		= graphics->getHandleIndex("bark", ResourceType::ShaderResourceView);
	barkNorm	= graphics->getHandleIndex("barkNormal", ResourceType::ShaderResourceView);
	barkSpec	= graphics->getHandleIndex("barkSpec", ResourceType::ShaderResourceView);
	branch		= graphics->getHandleIndex("branch", ResourceType::ShaderResourceView);
	roof		= graphics->getHandleIndex("leafRoof", ResourceType::ShaderResourceView);
	quadTree = new DetailNode(graphics, &forestContainer->ActiveListLod0, &forestContainer->ActiveListLod1, &forestContainer->ActiveListLod2);
}

bool ForestBiom::setSingleHeight(std::array<Chunk<CHUNKSIZE, CHUNKSIZE>, 9> *_chunks, float _size, glm::vec3 &_pos)
{
	glm::vec3 chunkPos = glm::vec3(floor(_pos.x / _size), floor(_pos.y / _size), floor(_pos.z / _size)) * _size;

	for(int y = 0; y < 9; y++)
	{
		if(_chunks->at(y).position == chunkPos)
		{
			int heightIndex = (int)((_pos.z - _chunks->at(y).position.z) / (pow(3, _chunks->at(y).level - 1) * QUADSIZE)) * CHUNKSIZE +
				(int)((_pos.x - _chunks->at(y).position.x) / (pow(3, _chunks->at(y).level - 1) * QUADSIZE));

			_pos.y = terrainContainer->heights[_chunks->at(y).heightsID].at(heightIndex);

			return true;
		}
	}
	return false;
}

void ForestBiom::generateTrees(int _seed)
{
	int minX = INT32_MAX;
	int minY = INT32_MAX;
	int minZ = INT32_MAX;
	int maxX = INT32_MIN;
	int maxY = INT32_MIN;
	int maxZ = INT32_MIN;
	BiomsContainer *biom = Containers(std::make_pair(Containers::BIOMS, READ)).get<BiomsContainer>(Containers::BIOMS);
	basePos = glm::vec3(biom->forestPoints.at(0).x - abs(terrainContainer->large[0].position.x), 0, biom->forestPoints.at(0).y - abs(terrainContainer->large[0].position.z));

	randomNumberGenerator.seed(_seed);
	int randomNumber = range(randomNumberGenerator);
	float scale;

	uint treesPerBlend = 1;
	uint foliageAmount = 10;

	std::vector<ModelInstance> temp;

	int size = BLENDTEXSIZE * 3 + 1;
	float step = (CHUNKSIZE / BLENDTEXSIZE) * QUADSIZE * 9;
	glm::vec3 startPos = terrainContainer->large[0].position;
	std::vector<glm::vec4> *blendMap = &terrainContainer->biomMap.map2;
	std::vector<glm::vec4> *blendMap1 = &terrainContainer->biomMap.map1;
	for(int i = 0; i < size * size; i++)
	{
		if(blendMap->at(i).x < 0.7f && blendMap1->at(i).w < 0.7f)
			continue;
		if(i % 3 != 0 && blendMap->at(i).x > 0.7f)
			continue;

		if(i % 7 != 0 && blendMap1->at(i).w > 0.7f)
			continue;

		basePos = startPos + glm::vec3((i % size) * step, 0, (i / size) * step);
		uint oldSize = temp.size();
		temp.resize(oldSize + treesPerBlend);

		for(uint j = oldSize; j < temp.size(); j++)
		{
			scale = 150.0f + range(randomNumberGenerator)*0.01f;

			temp.at(j).pos = basePos + glm::vec3(range(randomNumberGenerator) * 0.51, 0, range(randomNumberGenerator) * 0.51);

			if(temp.at(j).pos.x < minX)
				minY = temp.at(j).pos.x;
			if(temp.at(j).pos.x > maxX)
				maxX = temp.at(j).pos.x;

			if(temp.at(j).pos.y < minY)
				minY = temp.at(j).pos.y;
			if(temp.at(j).pos.y > maxY)
				maxX = temp.at(j).pos.y;

			if(temp.at(j).pos.z < minZ)
				minZ = temp.at(j).pos.z;
			if(temp.at(j).pos.z > maxZ)
				maxX = temp.at(j).pos.z;

			//find out what heightmap to look for heights in
			temp.at(j).pos.y = Utility::mapToTerrain(glm::vec2(temp.at(j).pos.x,temp.at(j).pos.z), *terrainContainer).y;

			temp.at(j).pos.y -= scale;
			temp.at(j).scale = glm::vec3(scale, scale, scale);
			temp.at(j).rot = glm::vec3(0, range(randomNumberGenerator) * 0.001f, 0);
		}
	}

	forestContainer->startIDLod0.at(containerIndex) = forestContainer->treeList.size();
	forestContainer->treeList.insert(forestContainer->treeList.begin() + forestContainer->treeList.size(), temp.begin(), temp.end());
	forestContainer->sizeLod0.at(containerIndex) = forestContainer->treeList.size() - forestContainer->startIDLod0.at(containerIndex);

	forestContainer->startIDFoliage.at(containerIndex) = forestContainer->treeList.size();
	glm::vec2 randomPos;
	float randomHeight;


	for (uint i = 0; i < forestContainer->startIDFoliage.at(containerIndex); i += 25)
	{
		SingleBiom name = Utility::getBlendMapValue(biom, temp.at(i).pos);
		if (name.map2.x > 0.7)
		{
			temp.resize(temp.size() + 1);
			randomHeight = range(randomNumberGenerator)*0.1f;
			scale = 700.0f + range(randomNumberGenerator)*0.01f;
			temp.back().pos = glm::vec3(temp.at(i).pos.x, Utility::mapToTerrain(randomPos, *terrainContainer).y + 2900 + randomHeight, temp.at(i).pos.z);
			temp.back().scale = glm::vec3(scale, scale, scale);
			temp.back().rot = glm::vec3(0, range(randomNumberGenerator) * 0.001f, 0);
		}
	}

	for (uint i = 1; i < forestContainer->startIDFoliage.at(containerIndex); i += 20)
	{
		SingleBiom name = Utility::getBlendMapValue(biom, temp.at(i).pos);
		if (name.map2.x > 0.7)
		{
			temp.resize(temp.size() + 1);
			randomHeight = range(randomNumberGenerator)*0.05f;
			scale = 500.0f + range(randomNumberGenerator)*0.01f;
			temp.back().pos = glm::vec3(temp.at(i).pos.x, Utility::mapToTerrain(randomPos, *terrainContainer).y + 2000 + randomHeight, temp.at(i).pos.z);
			temp.back().scale = glm::vec3(scale, scale, scale);
			temp.back().rot = glm::vec3(0, range(randomNumberGenerator) * 0.001f, 0);
		}
	}

	for (uint i = 2; i < forestContainer->startIDFoliage.at(containerIndex); i += 15)
	{
		SingleBiom name = Utility::getBlendMapValue(biom, temp.at(i).pos);
		if (name.map2.x > 0.7)
		{
			temp.resize(temp.size() + 1);
			randomHeight = range(randomNumberGenerator)*0.1f;
			scale = 150.0f + range(randomNumberGenerator)*0.01f;
			temp.back().pos = glm::vec3(temp.at(i).pos.x, Utility::mapToTerrain(randomPos, *terrainContainer).y + 1500 + randomHeight, temp.at(i).pos.z);
			temp.back().scale = glm::vec3(scale, scale, scale);
			temp.back().rot = glm::vec3(0, range(randomNumberGenerator) * 0.001f, 0);
		}
	}

	forestContainer->treeList.insert(forestContainer->treeList.begin() + forestContainer->treeList.size(), temp.begin() + forestContainer->treeList.size(), temp.end());
	forestContainer->sizeFoliage.at(containerIndex) = temp.size() - forestContainer->startIDFoliage.at(containerIndex);

	createBuffers(forestContainer->treeList);

	
	quadTree->setDimensions( glm::vec3( minX, minY, minZ ), glm::vec3( maxX, maxY, maxZ ) );

	std::vector< ModelInstance *> nodeTreeList; //needs to be declared somewhere else so that the elements can be properly deleted in the destructor
	quadTree->buildTree(&nodeTreeList);
}

void ForestBiom::update(float _deltaTime)
{
	PlayerContainer *player = Containers(std::make_pair(Containers::PLAYER, READ)).get<PlayerContainer>(Containers::PLAYER);
	std::vector<ModelInstance> temp;
	uint treesPerBlend = 1;
	int size = BLENDTEXSIZE * 3 + 1;
	float step = (CHUNKSIZE / BLENDTEXSIZE) * QUADSIZE * 9;
	glm::vec3 startPos = terrainContainer->large[0].position;
	std::vector<glm::vec4> *blendMap = &terrainContainer->biomMap.map2;
	std::vector<glm::vec4> *blendMap1 = &terrainContainer->biomMap.map1;
	for(uint i = 0; i < forestContainer->startIDFoliage.at(containerIndex); i++)
	{
		glm::vec3 pos = forestContainer->treeList.at(i).pos;
		float dist = glm::length(player->position - pos);
		if(dist > 100000)
			continue;
		uint oldSize = temp.size();
		temp.resize(oldSize + treesPerBlend);

		for(uint j = oldSize; j < temp.size(); j++)
		{
			temp.at(j).pos = pos;

			//find out what heightmap to look for heights in
			temp.at(j).pos.y = Utility::mapToTerrain(glm::vec2(temp.at(j).pos.x,temp.at(j).pos.z), *terrainContainer).y;
		}
	}
	forestContainer->sizeLod0.at(containerIndex) = temp.size();
	if(temp.size() > 0)
		graphics->replaceImmutableVBuffer(instancingIndexTree,(void**)&temp[0], temp.size(), sizeof(temp.at(0) ));
}

void ForestBiom::draw(UINT renderTargetIndex, UINT amount, UINT depthBufferIndex)
{
	if(forestContainer->sizeLod0.at(containerIndex) <= 0)
		return;
	unsigned int index = 0;

	UINT strides[2];
	strides[0] = sizeof(DefaultVertex);
	strides[1] = sizeof(ModelInstance);
	UINT offsets[2] = { 0, 0 };

	//graphics->updateVBuffer(bufferIndex, (void**)&sandStormContainer->particleData[0],sandStormContainer->particleData.size(),strides);
	graphics->setVertexBuffers(0, forestContainer->modelList.at(0).getVertexBufferID(), 1, &strides[0], &offsets[0]);
	graphics->setVertexBuffers(1, instancingIndexTree, 1, &strides[1], &offsets[1]);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_MODEL_INSTANCED);

	treeTech->use();
	graphics->setRenderTargetViews(renderTargetIndex, amount, depthBufferIndex);
	graphics->setDepthStencilState(0);
	graphics->setRasterState(1);
	
	graphics->setCBuffers(2,1,once);
	graphics->setCBuffers(1,1,view);
	graphics->setPixelShaderResource(2, bark, 1);
	graphics->setPixelShaderResource(3, barkSpec, 1);
	graphics->setPixelShaderResource(4, graphics->getHandleIndex("black",ResourceType::ShaderResourceView), 1);
	graphics->setPixelShaderResource(8, barkNorm, 1);
	graphics->setBlendState(0);
	graphics->setTopology(0);

	//TODO:
	//check the collision of the AABB in the quadTree (logic exists in the base class CullingNode)
	//if a node is hit, perform the same test on its child nodes, and so on. For each leaf node insert its trees (objects vector elements) in a global vector to be sent to the buffer. (different list for different LoD)
	graphics->drawInstanced(forestContainer->modelList.at(0).getVertexAmount(), forestContainer->sizeLod0.at(containerIndex), 0, forestContainer->startIDLod0.at(containerIndex));
}

void ForestBiom::drawForward(UINT renderTargetIndex, UINT amount, UINT depthBufferIndex)
{
	unsigned int index = 0;

	UINT strides[2];
	strides[0] = sizeof(NormallessVertex);
	strides[1] = sizeof(ModelInstance);
	UINT offsets[2] = { 0, 0 };

	graphics->setVertexBuffers(0, forestContainer->modelList.at(1).getVertexBufferID(), 1, &strides[0], &offsets[0]);
	graphics->setVertexBuffers(1, instancingIndexTree, 1, &strides[1], &offsets[1]);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_SURFACE_INSTANCED);

	surfaceTech->use();
	graphics->setRenderTargetViews(renderTargetIndex, amount, depthBufferIndex);
	graphics->setDepthStencilState(1);
	graphics->setRasterState(2);
	graphics->setBlendState(2);
	graphics->setTopology(0);

	graphics->setPixelShaderResource(2, roof, 1);
	graphics->drawInstanced(forestContainer->modelList.at(1).getVertexAmount(), forestContainer->sizeFoliage.at(containerIndex), 0, forestContainer->startIDFoliage.at(containerIndex) );
}

void ForestBiom::drawShadow()
{
	unsigned int index = 0;

	UINT strides[2];
	strides[0] = sizeof(DefaultVertex);
	strides[1] = sizeof(ModelInstance);
	UINT offsets[2] = { 0, 0 };

	graphics->setVertexBuffers(0, forestContainer->modelList.at(0).getVertexBufferID(), 1, &strides[0], &offsets[0]);
	graphics->setVertexBuffers(1, instancingIndexTree, 1, &strides[1], &offsets[1]);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_MODEL_INSTANCED);

	graphics->setDepthStencilState(0);
	graphics->setRasterState(0);

	graphics->setBlendState(0);
	graphics->setTopology(0);
	graphics->drawInstanced(forestContainer->modelList.at(0).getVertexAmount(), 1000, 0, 0);
}

ForestBiom::~ForestBiom()
{
	SAFE_DELETE(quadTree);
}
