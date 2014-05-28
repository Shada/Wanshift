#include "DeferredRenderingAspect.h"
#include "MainState.h"

#include <glm/gtc/matrix_transform.hpp>
#include "ModelLoaderFactory.h"

DeferredRenderingAspect::DeferredRenderingAspect()
		: AspectInterface("DeferredRenderingAspect", 0)
{
	graphics = NULL;

	Containers containers(std::make_pair(Containers::PLAYER, READ), std::make_pair(Containers::DETAIL_LAYER, READ), std::make_pair(Containers::TERRAIN, READ), std::make_pair(Containers::CAMERA, MODIFY),
						  std::make_pair(Containers::ENEMY, READ));

	player = containers.get<PlayerContainer>(Containers::PLAYER);
	terrainDetail = containers.get<DetailLayerContainer>(Containers::DETAIL_LAYER);
	terrain = containers.get<TerrainContainer>(Containers::TERRAIN);
	terrain->grassChunks = std::vector<grassVertexPerInstance>(25);
	camera = containers.get<CameraContainer>(Containers::CAMERA);
	forestContainer = containers.get<ForestContainer>(Containers::FOREST);
	enemyContainer = containers.get<EnemyContainer>(Containers::ENEMY);
}

DeferredRenderingAspect::~DeferredRenderingAspect()
{
	SAFE_DELETE(passTwoShaderIndex);
	SAFE_DELETE(blurHorizontalIndex);
	SAFE_DELETE(blurVerticalIndex);
	SAFE_DELETE(tessellation);
	SAFE_DELETE(skysphereShaderIndex);
	SAFE_DELETE(terrainScatter);
	SAFE_DELETE(skyScatter);
	SAFE_DELETE(player);
	SAFE_DELETE(terrainDetail);
	SAFE_DELETE(terrain);
	SAFE_DELETE(camera);
	SAFE_DELETE(skysphereAspect);
}

void DeferredRenderingAspect::init(MainState& _mainState)
{
	graphics = _mainState.graphics;

	for (auto& i : player->playersAnimationState)
		i = AnimationState::aNOANIMATION;
	for (auto& i : player->playersPrevAnimationFrame)
		i = AnimationState::aNOANIMATION;

	cbWorldIndex = graphics->getHandleIndex("cbWorld",ResourceType::ConstantBuffer);
	cbViewIndex = graphics->getHandleIndex("cbView",ResourceType::ConstantBuffer);
	cbOnceIndex = graphics->getHandleIndex("cbOnce",ResourceType::ConstantBuffer);
	cbEveryFrameIndex = graphics->getHandleIndex("cbEveryframe",ResourceType::ConstantBuffer);
	cbTerrainIndex = graphics->getHandleIndex("cbTerrain",ResourceType::ConstantBuffer);
	cbAnimationMatricesIndex = graphics->getHandleIndex("cbAnimationMatrices", ResourceType::ConstantBuffer);
	cbLightInfoIndex = graphics->getHandleIndex("cbLightInfo",ResourceType::ConstantBuffer);
	cbWorldAttributesIndex = graphics->getHandleIndex("cbWorldAttribute",ResourceType::ConstantBuffer);
	cbScatterAttributesIndex = graphics->getHandleIndex("cbScatterAttributes",ResourceType::ConstantBuffer);
	cbSpotLightIndex = graphics->getHandleIndex("cbSpotLight",ResourceType::ConstantBuffer);

	desertTexIndex = graphics->getHandleIndex("deserttexture", ResourceType::ShaderResourceView);
	stoneTexIndex = graphics->getHandleIndex("stone", ResourceType::ShaderResourceView);
	snowStoneTexIndex = graphics->getHandleIndex("snowstone", ResourceType::ShaderResourceView);
	meadowTexIndex = graphics->getHandleIndex("meadow", ResourceType::ShaderResourceView);
	swampTexIndex = graphics->getHandleIndex("swamp", ResourceType::ShaderResourceView);
	forestTexIndex = graphics->getHandleIndex("forest", ResourceType::ShaderResourceView);
	seedTexIndex = graphics->getHandleIndex("seed", ResourceType::ShaderResourceView);
	heightMapIndex = graphics->getHandleIndex("heightmap", ResourceType::ShaderResourceView); 

	modelShaderIndex = new ShaderClass("modelVS",
										  "",
										  "",
										  "",
										  "modelPS",
										  graphics,
										  (int)LayoutTypes::LAYOUT_DEFAULT,
										  false);

	blurHorizontalIndex		= new ShaderClass("deferredLightVS",
												"deferredLightGS",
												"",
												"",
												"blurHorizontalPS", 
												graphics,
												(int)LayoutTypes::LAYOUT_DEFAULT,
												false);
	blurVerticalIndex		= new ShaderClass("deferredLightVS",
												"deferredLightGS",
												"",
												"",
												"blurVerticalPS", 
												graphics,
												(int)LayoutTypes::LAYOUT_DEFAULT,
												false);
	passTwoShaderIndex		= new ShaderClass("deferredLightVS",
												"deferredLightGS",
												"",
												"",
												"deferredLightPS", 
												graphics,
												(int)LayoutTypes::LAYOUT_DEFAULT,
												false);
	tessellation			= new ShaderClass("tessellationVS",
												"",
												"tessellationHS",
												"tessellationDS",
												"tessellationPS", 
												graphics, (int)LayoutTypes::LAYOUT_DEFAULT,
												false);
	skysphereShaderIndex	= new ShaderClass("skysphereVS",
												"",
												"",
												"",
												"skyspherePS", 
												graphics,
												(int)LayoutTypes::LAYOUT_DEFAULT,
												false);
	terrainIndex			= new ShaderClass("terrainVS",
												"",
												"",
												"",
												"terrainPS",
												graphics, (int)LayoutTypes::LAYOUT_DEFAULT,
												false);
	animationShaderIndex = new ShaderClass("modelAnimVS",
										   "",
										   "",
										   "",
										   "modelPS",
										   graphics,
										   (int)LayoutTypes::LAYOUT_ANIMATION,
										   false);
	//--------------------------------------------------------------------//
	//------------------------- render targets ---------------------------//
	//--------------------------------------------------------------------//
	int mrtvTex[numberOfRenderTargets];
	int mrtvSRV[numberOfRenderTargets];
	int mrtvRTV[numberOfRenderTargets];

	int w = (int)graphics->getResolution().x;
	int h = (int)graphics->getResolution().y;

	int handleIndex;
	std::string deferredNames[numberOfRenderTargets] = { "color", "normal", "specular", "glow" };

	for(int i = 0; i < numberOfRenderTargets; i++)
	{
		mrtvTex[i] = graphics->createTexture2D(w, h);
		mrtvRTV[i] = graphics->createRenderTargetView(mrtvTex[i]);
		handleIndex = mrtvSRV[i] = graphics->createShaderResourceView(mrtvTex[i]);

		graphics->addHandle( std::to_string(i), ResourceType::ShaderResourceView, handleIndex );
		graphics->addHandle( deferredNames[i], ResourceType::ShaderResourceView, handleIndex );
		graphics->addHandle( deferredNames[i], ResourceType::RenderTargetView, mrtvRTV[i] );
	}

	int sceneTex = graphics->createTexture2D(w,h);
	sceneRenderTarget = graphics->createRenderTargetView(sceneTex);
	sceneResource = graphics->createShaderResourceView(sceneTex);
	//sceneRenderTarget = graphics->getHandleIndex("fakeBufferRenderTarget", ResourceType::RenderTargetView);// graphics->createRenderTargetView(sceneTex);
	//sceneResource = graphics->getHandleIndex("fakeBuffer", ResourceType::ShaderResourceView);// graphics->createShaderResourceView(sceneTex);
	
	//blur stuff
	int blurtex = graphics->createTexture2D(w, h);
	renderTargetViewPointerBlurH = graphics->createRenderTargetView(blurtex);
	shaderResourceViewPointerBlurH = graphics->createShaderResourceView(blurtex);

	blurtex = graphics->createTexture2D(w, h);
	renderTargetViewPointerBlurV = graphics->createRenderTargetView(blurtex);
	shaderResourceViewPointerBlurV = graphics->createShaderResourceView(blurtex);

	//-------------------------------------------------------------//
	//----------------------- Model Loading ------------------------//
	//-------------------------------------------------------------//
	//ModelLoader* ml2 = Loader::ModelLoaderAPI::createLoader();
	ModelLoaderInterface* ml = Loader::ModelLoaderAPI::createLoader(Loader::LoaderType::MESHLOADER);
	ModelLoaderInterface* al = Loader::ModelLoaderAPI::createLoader(Loader::LoaderType::ANIMATIONLOADER);
	ModelLoaderInterface* pl = Loader::ModelLoaderAPI::createLoader(Loader::LoaderType::PLANELOADER);

	DefaultVertex v;

	float* data = ml->loadFBX("Models/PlayerModel/Animation/BindPose/BindPose.fbx", &player->playerModel);

	uint numvertices = player->playerModel.getVertexAmount();

	NormallessVertex vnl;
	int someindices = graphics->createVBufferImmutable((void**)&data[0],
													   numvertices, sizeof(v));
	player->playerModel.setVertexBufferID(someindices);

	float* data1 = al->loadFBX("Models/PlayerModel/Animation/Cycles/IdleCycle/IdleCycle.fbx", &player->playerModel);
	someindices = graphics->createVBufferImmutable((void**)&data1[0], numvertices * player->playerModel.getAnimationAt(0).nClusters, sizeof(float));
	player->playerModel.getAnimationAt(0).weightBufferID = someindices;
	float* data2 = al->loadFBX("Models/PlayerModel/Animation/Cycles/RunCycle/RunCycle.fbx", &player->playerModel);
	someindices = graphics->createVBufferImmutable((void**)&data2[0], numvertices * player->playerModel.getAnimationAt(1).nClusters, sizeof(float));
	player->playerModel.getAnimationAt(1).weightBufferID = someindices;
	float* data3 = al->loadFBX("Models/PlayerModel/Animation/Cycles/ExtractEnergyCycle/ExtractEnergyCycle.fbx", &player->playerModel);
	someindices = graphics->createVBufferImmutable((void**)&data3[0], numvertices * player->playerModel.getAnimationAt(2).nClusters, sizeof(float));
	player->playerModel.getAnimationAt(2).weightBufferID = someindices;

	// here we go! Textures will be loaded!
	for(Group &group : player->playerModel.getGroups())
	{
		group.material.diffuseMap = graphics->createShaderResourceViewFromFile(group.material.diffuseMapName);
		group.material.emissiveMap = graphics->createShaderResourceViewFromFile(group.material.emissiveMapName);
		group.material.normalMap = graphics->createShaderResourceViewFromFile(group.material.normalMapName);
		group.material.specularMap = graphics->createShaderResourceViewFromFile(group.material.specularMapName);
	}
	
	UINT oldSize = forestContainer->modelList.size();
	forestContainer->modelList.resize( oldSize + 3);
	float* data0 = ml->loadFBX( "Models/redwoodTree3.fbx", &forestContainer->modelList.at(oldSize) );
	forestContainer->modelList.at(oldSize).setVertexBufferID( graphics->createVBufferImmutable((void**)&data0[0], forestContainer->modelList.at(oldSize).getVertexAmount(), sizeof(v) ) );

	data0 = pl->loadFBX( "Models/roof.fbx", &forestContainer->modelList.at(oldSize + 1) );
	forestContainer->modelList.at(oldSize +1).setVertexBufferID( graphics->createVBufferImmutable((void**)&data0[0], forestContainer->modelList.at(oldSize+1).getVertexAmount(), sizeof(vnl) ) );

	data0 = pl->loadFBX( "Models/branch.fbx", &forestContainer->modelList.at(oldSize + 2) );
	forestContainer->modelList.at(oldSize +2).setVertexBufferID( graphics->createVBufferImmutable((void**)&data0[0], forestContainer->modelList.at(oldSize+2).getVertexAmount(), sizeof(vnl) ) );

	Loader::ModelLoaderAPI::deleteLoader(ml);
	Loader::ModelLoaderAPI::deleteLoader(al);
	Loader::ModelLoaderAPI::deleteLoader(pl);
	//-------------------------------------------------------------//
	//-------------------------- skybox ---------------------------//
	//-------------------------------------------------------------//

	skysphereAspect = new SkysphereAspect();

	skyboxTexIndex = graphics->createCubeTexture("Textures/demostars.dds");

	someindices = graphics->createVBufferImmutable((void**)&skysphereAspect->getVertices()->at(0),
												   skysphereAspect->getVertices()->size(), sizeof(DefaultVertex));
	skysphereAspect->setVertexBufferID(someindices);
	skysphereAspect->clearVertices();
	someindices = graphics->createIndexBufferImmutable((void**)&skysphereAspect->getIndices()->at(0), skysphereAspect->getIndices()->size());
	skysphereAspect->setIndexBufferID(someindices);
	skysphereAspect->clearIndices();

	//-------------------------------------------------------------//
	//----------------------- scatter -----------------------------//
	//-------------------------------------------------------------//

	renderTargetViewPointer = mrtvRTV[0];
	shaderResourceViewPointer = mrtvSRV[0];

	skyScatter = new SkyAspect();

	skyScatter->init(_mainState);

	terrainScatter = new ShaderClass("deferredLightVS", 
									 "deferredLightGS", 
									 "", 
									 "", 
									 "terrainScatterPS", 
									 graphics, 
									 (int)LayoutTypes::LAYOUT_DEFAULT, 
									 false); 





	//-------------------------------------------------------------//
	//------------------------ Forest -----------------------------//
	//-------------------------------------------------------------//
	forest.init(graphics);
	forest.generateTrees(34543654);

	shadow = new CascadeShadow();
	shadow->init(graphics);
	shadow->setCamera(camera);
	shadow->setModel(&player->playerModel);
	shadow->setPlayer(player);
	shadow->setTerrain(terrain);
	shadow->setForest(&forest);

	//-------------------------------------------------------------//
	//------------------------ Swamp  -----------------------------//
	//-------------------------------------------------------------//
	swamp.init(graphics);
}

void DeferredRenderingAspect::runAspect(float _dt)
{
	graphics->setCBuffers(0,1,cbWorldIndex);
	graphics->setCBuffers(1,1,cbViewIndex);
	graphics->setCBuffers(2,1,cbOnceIndex);
	graphics->setCBuffers(3,1,cbEveryFrameIndex);

	// Call shadowing pass
	shadow->updateLight(_dt);
	shadow->shadowing(); // Använd inte efter passOne(); ger bara svart

	// Call the renderer for the first pass
	passOne(_dt);

	// Call the second pass for rendering to the final scene
	passTwo();
}

void DeferredRenderingAspect::passOne(float _dt)
{

	UINT stride;
	UINT offset;

	// *********************************//
	//				skybox				//
	//**********************************//
	graphics->setRenderTargetViews(renderTargetViewPointer, 1, -1);
	graphics->setBlendState(2);
	skysphereShaderIndex->use();
	graphics->setTopology(0);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_DEFAULT);
	graphics->setRasterState(0);
	graphics->setPixelShaderResource(1, skyboxTexIndex, 1);
	stride = sizeof(DefaultVertex);
	offset = 0;

	graphics->setIndexBuffer(skysphereAspect->getIndexBufferID(), 0);
	graphics->setVertexBuffers(0, skysphereAspect->getVertexBufferID(), 1, &stride, &offset);

	graphics->drawIndexed(skysphereAspect->getIndexAmount(), 0, 0);

	graphics->setPixelShaderResource(1, -1, 1);
	//**********************//
	//		Atmoshpere		//
	//**********************//
	skyScatter->run(_dt);	// needs deltaTime

	glm::mat4 matrix;
	CBWorldData world;
	
	// Set the rendertargets
	graphics->setRenderTargetViews(renderTargetViewPointer, numberOfRenderTargets, 0); // indexet för depth buffern ska ändras i senare skede
	
	graphics->setCBuffers(5,1,cbTerrainIndex);
	//// Set depth stencil state
	graphics->setDepthStencilState(0); // index värder för depth stensil staten ska ändras senare
	//
	//// Set blend state
	graphics->setBlendState(0); // Ändra 0an senare...
	//// Set primitive topology
	graphics->setTopology(0);
	
	//******************************//
	//          grass               //
	//******************************//
	terrain->grass->draw(&camera->frustum);

	//******************************//
	//			models				//
	//******************************//
	//passOneShaderIndex->use();
	animationShaderIndex->use();
	stride = sizeof(DefaultVertex);
	offset = 0;
	graphics->setVertexBuffers(0, player->playerModel.getVertexBufferID(), 1, &stride, &offset);

	stride = sizeof(float)* 15;
	offset = 0;
	graphics->setVertexBuffers(1, player->playerModel.getAnimationAt(0).weightBufferID, 1, &stride, &offset);

	graphics->setInputLayout((int)LayoutTypes::LAYOUT_ANIMATION);

	graphics->setRasterState(1);

	graphics->setCBuffers(6, 1, cbAnimationMatricesIndex);
	CBAnimationMatrices cbanim;

	for(uint i = 0; i < player->playerModel.getGroupAmount(); i++)
	{
		graphics->setPixelShaderResource(2, player->playerModel.getGroupAt(i).material.diffuseMap, 1);
		graphics->setPixelShaderResource(3, player->playerModel.getGroupAt(i).material.specularMap, 1);
		graphics->setPixelShaderResource(4, player->playerModel.getGroupAt(i).material.emissiveMap, 1);
		graphics->setPixelShaderResource(8, player->playerModel.getGroupAt(i).material.normalMap, 1);

		matrix = glm::translate(glm::mat4x4(), glm::vec3(player->position.x, player->position.y - 15.0f, player->position.z));
		matrix = glm::rotate(matrix, player->rot * 180 / 3.14f, glm::vec3(0.f, 1.f, 0.f));
		matrix = glm::scale(matrix, glm::vec3(1.5f, 1.5f, 1.5f));
		world.mWorld = matrix;
		world.mWorldInv = matrix;

		cbanim.interpolation[0] = player->animationInterpolation;
		for(int j = 0; j < 15; j++)
		{
			cbanim.mat1[j] = player->playerModel.getAnimationAt(player->animationState).frameData.at(player->prevAnimationFrame).matrices[j];
			cbanim.mat2[j] = player->playerModel.getAnimationAt(player->animationState).frameData.at(player->animationFrame).matrices[j];
		}
		graphics->updateCBuffer(cbAnimationMatricesIndex, &cbanim);
		graphics->updateCBuffer(cbWorldIndex, &world);

		int startIndex = player->playerModel.getGroupAt(i).startIndex;
		int amount = player->playerModel.getGroupAt(i).vertexAmount;
		graphics->draw(amount, startIndex);

		for(uint j = 0; j < player->playersPosition.size(); j++)// these have to be solved somehow sow they can have their own animationcycle
		{
			if(player->playersPosition[j] == glm::vec3())
				continue;

			matrix = glm::translate(glm::mat4x4(), glm::vec3(player->playersPosition[j].x, player->playersPosition[j].y - 14.0f, player->playersPosition[j].z));
			matrix = glm::rotate(matrix, player->playersRotDegree[j] * 180 / 3.14f, glm::vec3(0.f, 1.f, 0.f));
			matrix = glm::scale(matrix, glm::vec3(1.5f, 1.5f, 1.5f));

			world.mWorld = matrix;
			world.mWorldInv = matrix;


			cbanim.interpolation[0] = player->playersAnimationInterpolation[j];
			for(int k = 0; k < 15; k++)
			{
				cbanim.mat1[k] = player->playerModel.getAnimationAt(player->playersAnimationState[j]).frameData.at(player->playersPrevAnimationFrame[j]).matrices[k];
				cbanim.mat2[k] = player->playerModel.getAnimationAt(player->playersAnimationState[j]).frameData.at(player->playersAnimationState[j]).matrices[k];
			}
			graphics->updateCBuffer(cbAnimationMatricesIndex, &cbanim);
			graphics->updateCBuffer(cbWorldIndex, &world);
			graphics->draw(amount, startIndex);
		}
	}


	//**********************************//
	//				terrain				//
	//**********************************//

	// Set input layout
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_DEFAULT);

	world.mWorld = glm::mat4();
	world.mWorldInv = glm::mat4();
	
	graphics->updateCBuffer(cbWorldIndex, &world);
	//
	//// Set the vertex buffers
	graphics->setRasterState(0);

	graphics->setPixelShaderResource(1, desertTexIndex, 1);
	graphics->setPixelShaderResource(2, stoneTexIndex, 1);
	graphics->setPixelShaderResource(3, meadowTexIndex, 1);
	graphics->setPixelShaderResource(4, swampTexIndex, 1);
	graphics->setPixelShaderResource(5, forestTexIndex, 1);
	graphics->setPixelShaderResource(6, seedTexIndex, 1);
	graphics->setPixelShaderResource(7, heightMapIndex, 1);
	graphics->setPixelShaderResource(8, terrain->biomMapID1, 1);
	graphics->setPixelShaderResource(9, terrain->biomMapID2, 1);
	graphics->setPixelShaderResource(10, snowStoneTexIndex, 1);

	//// Set the vertex buffers
	stride = sizeof(DefaultVertex);
	offset = 0;
	graphics->setVertexBuffers(0, terrain->bufferID, 1, &stride, &offset);
	graphics->setIndexBuffer(terrain->indexBufferID, offset);


	//// Render to the render targets

	terrainIndex->use();
	terrain->tesselationchunks.clear();

	terrain->smallNodes.at(0).draw(&camera->frustum);
	terrain->mediumNodes.at(0).draw(&camera->frustum);
	terrain->largeNodes.at(0).draw(&camera->frustum);

	if(terrain->tesselationchunks.size() > 0)
	{
		graphics->setDomainShaderResource(1, heightMapIndex, 1);
		graphics->setDomainShaderResource(2, terrain->biomMapID1, 1);

		graphics->setTopology(2);
		tessellation->use();
		for(uint i = 0; i < terrain->tesselationchunks.size(); i++)
			graphics->drawIndexed(terrain->nVerticesPerTiny / (TINYSPLITTING*TINYSPLITTING), terrain->tesselationchunks.at(i), 0);
	}

	//-------------------------------------------------------------//
	//------------------------ Forest -----------------------------//
	//-------------------------------------------------------------//
	//forest.update(_dt);
	forest.draw(renderTargetViewPointer, numberOfRenderTargets, 0 );
	
	//******************************//
	//			glow				//
	//******************************//
	blurHorizontalIndex->use();

	graphics->setRasterState(0);
	graphics->setRenderTargetViews(renderTargetViewPointerBlurH, 1, -1);
	graphics->setPixelShaderResource(1, shaderResourceViewPointer + 3, 1);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_DEFAULT); // index värdet ska ändras senare
	graphics->setTopology(1);

	graphics->draw(1, 0);

	blurVerticalIndex->use();
	graphics->setRenderTargetViews(renderTargetViewPointerBlurV, 1, -1);
	graphics->setPixelShaderResource(1, shaderResourceViewPointerBlurH, 1);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_DEFAULT);
	graphics->setTopology(1);

	graphics->draw(1, 0);

	graphics->setPixelShaderResource(1,-1,0);
}

void DeferredRenderingAspect::passTwo()
{
	graphics->setCBuffers(5,1,graphics->getHandleIndex("cbFog",ResourceType::ConstantBuffer));

	//**********************************//
	//				pass2				//
	//**********************************//
	// Sets the shaders for pass two
	// Set input layout

	graphics->setInputLayout((int)LayoutTypes::LAYOUT_DEFAULT);
	graphics->setCBuffers(4,1,cbLightInfoIndex);
	// Set topology
	graphics->setTopology(1);

	graphics->setRasterState(0);
	// Set render targets

	graphics->clearRenderTargetSingle(renderTargetViewPointerBlurH);
	graphics->setRenderTargetViews(renderTargetViewPointerBlurH, 1, -1);


	terrainScatter->use();

	graphics->setCBuffers(9,1,cbWorldAttributesIndex);
	graphics->setCBuffers(10,1,cbScatterAttributesIndex);
	graphics->setPixelShaderResource(1, graphics->getDepthResourceView(), 1);
	graphics->setPixelShaderResource(2, shaderResourceViewPointer, 1);

	graphics->draw(1, 0);

	// Blur the shadow map
	shadow->compileShadow();

	graphics->setCBuffers(6,1,cbSpotLightIndex);
	passTwoShaderIndex->use();
	//graphics->setToScreenRender(-1);
	graphics->setRenderTargetViews(sceneRenderTarget,1,-1);
	// Sets the color, normal and specular map to the shaders

	graphics->setPixelShaderResource(1, graphics->getDepthResourceView(), 1);
	graphics->setPixelShaderResource(2, shaderResourceViewPointerBlurH, 1);
	graphics->setPixelShaderResource(3, shaderResourceViewPointer + 1, 3);
	graphics->setPixelShaderResource(6, shaderResourceViewPointerBlurV, 1);
	graphics->setPixelShaderResource(7, graphics->getHandleIndex("computetexture", ResourceType::ShaderResourceView), 1);
	
	glm::mat4 matrix;
	CBWorldData world;
 
	//matrix = glm::translate(glm::mat4x4(), glm::vec3(player->position.x, player->position.y - 10.0f, player->position.z));

	world.mWorld = matrix;
	world.mWorldInv = matrix;

	graphics->updateCBuffer(cbWorldIndex, &world);

	// Render the final scene
	graphics->draw(1, 0);
	
	graphics->setToScreenRender(-1);
	swamp.drawWater(238764,sceneResource,graphics->getDepthResourceView());

	graphics->setPixelShaderResource(1, -1, 1);
	graphics->setPixelShaderResource(2, -1, 1);
	graphics->setPixelShaderResource(3, -1, 1);
	graphics->setPixelShaderResource(4, -1, 1);
	graphics->setPixelShaderResource(5, -1, 1);
	graphics->setPixelShaderResource(6, -1, 1);
	graphics->setPixelShaderResource(7, -1, 1);
	
	forest.drawForward(0, 1, 0 );
}


