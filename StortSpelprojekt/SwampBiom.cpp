#include "SwampBiom.h"


SwampBiom::SwampBiom()
{
	numTrees = 4000;
}

void SwampBiom::init(RenderInterface *_graphics)
{
	BaseBiom::init(_graphics);
	forestContainer->treeType.at(containerIndex) = 1;
	waterTech = new ShaderClass( "deferredLightVS", "deferredLightGS", "", "", "WaterPS", graphics,(int)LayoutTypes::LAYOUT_DEFAULT, false );
	//todo: load mesh and such
}
void SwampBiom::generateTrees( int _seed )
{
	randomNumberGenerator.seed(_seed);
	float scale = 1.0f;
	std::vector<ModelInstance> temp(numTrees);
	for(unsigned int i = 0; i < numTrees; i++)
	{
		scale = 100.0f + (float)range(randomNumberGenerator) * 0.1f;

		temp.at(i).pos = glm::vec3(range(randomNumberGenerator),10,range(randomNumberGenerator));
		temp.at(i).rot = glm::vec3((float)range(randomNumberGenerator) * 0.1f);
		temp.at(i).scale = glm::vec3(scale,scale,scale);
	}
	

	forestContainer->treeList.insert(forestContainer->treeList.begin() + forestContainer->treeList.size(),temp.begin(),temp.end());
	int bla = range(randomNumberGenerator);
}
void SwampBiom::update(float _deltaTime)
{
	
}
void SwampBiom::draw(UINT renderTargetIndex, UINT amount, UINT depthBufferIndex)
{
	//unsigned int index = 0;

	//UINT strides[2];
	//strides[0] = sizeof(DefaultVertex);
	//strides[1] = sizeof(ModelInstance);
	//UINT offsets[2] = { 0, 0 };

	////graphics->updateVBuffer(bufferIndex, (void**)&sandStormContainer->particleData[0],sandStormContainer->particleData.size(),strides);
	//graphics->setVertexBuffers(0, forestContainer->modelList.at(0).getVertexBufferID(), 1, &strides[0], &offsets[0]);
	//graphics->setVertexBuffers(1, instancingIndexTree, 1, &strides[1], &offsets[1]);
	//graphics->setInputLayout((int)LayoutTypes::LAYOUT_MODEL_INSTANCED);

	//treeTech->use();
	//graphics->setRenderTargetViews(renderTargetIndex, amount, depthBufferIndex);
	//graphics->setDepthStencilState(0);
	//graphics->setRasterState(1);

	//graphics->setCBuffers(2,1,once);
	//graphics->setCBuffers(1,1,view);
	//graphics->setPixelShaderResource(2, bark, 1);
	//graphics->setPixelShaderResource(4, graphics->getHandleIndex("black",ResourceType::ShaderResourceView), 1);
	//graphics->setBlendState(0);
	//graphics->setTopology(0);
	//graphics->drawInstanced(forestContainer->modelList.at(0).getVertexAmount(), 1000, 0, 0);
}
void SwampBiom::drawWater(UINT _renderTargetIndex, UINT _sceneResourceIndex, UINT _depthResourceIndex)
{
	graphics->setTopology(1);
	graphics->setCBuffers(5, 1, graphics->getHandleIndex("cbFog",ResourceType::ConstantBuffer));
	//graphics->setRenderTargetViews(_renderTargetIndex,1,-1);
	graphics->setPixelShaderResource(1,_sceneResourceIndex,1);
	graphics->setPixelShaderResource(2,_depthResourceIndex,1);
	graphics->setPixelShaderResource(3,graphics->getHandleIndex("waterheightmap", ResourceType::ShaderResourceView),1);
	graphics->setPixelShaderResource(4,graphics->getHandleIndex("waternormalmap", ResourceType::ShaderResourceView),1);
	graphics->setPixelShaderResource(5,graphics->getHandleIndex("waterfoam", ResourceType::ShaderResourceView),1);
	waterTech->use();
	graphics->draw(1,0);
}
SwampBiom::~SwampBiom()
{
	SAFE_DELETE(waterTech);
}
