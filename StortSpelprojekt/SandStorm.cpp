#include "SandStorm.h"
#include <glm\gtc\matrix_transform.hpp>

struct ScreenParticle
{
	glm::vec2	pos;
	glm::vec2	size;
};

SandStorm::SandStorm()
{
}


SandStorm::~SandStorm()
{
}

void SandStorm::init(RenderInterface *_graphics, SandStormContainer *_sandStormContainer, ConstantBufferContainer * _cbContainer)
{
	age = 0;
	graphics = _graphics;

	postEffectRenderTarget = graphics->getHandleIndex("postEffectRenderTarget", ResourceType::RenderTargetView);

	unsigned int groupAmount = 1;	//the amount of particle groups to be made.
	unsigned int maxSize = 1000;		//max size of each group
	sandStormContainer = _sandStormContainer;
	cbContainer = _cbContainer;
	//allocate for <groupAmount> amount of particle groups
	//sandStormContainer->allocate( 0, groupAmount );
	sandStormContainer->imageIndices.resize(groupAmount);
	sandStormContainer->maxSizes.resize(groupAmount);
	sandStormContainer->emitterPositions.resize(groupAmount);
	sandStormContainer->firstTime.resize(groupAmount);

	//initialize them
	sandStormContainer->imageIndices.at(0) = graphics->createShaderResourceViewFromFile("Textures/smokeLowerRes.png");
	
	sandStormContainer->maxSizes.at(0) = maxSize;
	sandStormContainer->emitterPositions.at(0) = glm::vec3(2000,100,400);
	sandStormContainer->firstTime.at(0) = true;


	UINT vertexSize = sizeof(CloudParticle);
	UINT offsets = 0;

	sandStormContainer->bufferIndex = bufferIndex = graphics->createVBufferDynamic(1, vertexSize);
	streamOutIndexFirst		= graphics->createVBufferStreamout( maxSize*groupAmount, vertexSize );
	streamOutIndexSecond	= graphics->createVBufferStreamout( maxSize*groupAmount, vertexSize );
	
	ScreenParticle overlay;
	overlay.pos		= glm::vec2(-1,1);
	overlay.size	= glm::vec2(2,2);
	bufferIndexOverlay		= graphics->createVBufferImmutable( (void**)&overlay, 1, sizeof(ScreenParticle) );

	int w = (int)graphics->getResolution().x;
	int h = (int)graphics->getResolution().y;
	depthMapTex = graphics->createDepthTexture2D(w,h);
	depthMapRTV = graphics->createDepthRenderTargetView(depthMapTex);
	depthMapSRV = graphics->createDepthShaderResourceView(depthMapTex);

	opacityMapTex = graphics->createTexture2D(w,h);
	opacityMapRTV = graphics->createRenderTargetView(opacityMapTex);
	opacityMapSRV = graphics->createShaderResourceView(opacityMapTex);

	drawTex = graphics->createTexture2D(w/4,h/4);
	drawRTV = graphics->createRenderTargetView(drawTex);
	drawSRV = graphics->createShaderResourceView(drawTex);

	finalDrawTex = graphics->createTexture2D(w,h);
	finalDrawRTV = graphics->createRenderTargetView(finalDrawTex);
	finalDrawSRV = graphics->createShaderResourceView(finalDrawTex);

	edgeStencilTex = graphics->createDepthTexture2D(w,h);
	edgeStencilRTV = graphics->createDepthRenderTargetView(edgeStencilTex);
	edgeStencilSRV = graphics->createDepthShaderResourceView(edgeStencilTex);



	CloudParticle pData;
	pData.pos = sandStormContainer->emitterPositions.at(0);
	pData.age = 5.0f;
	pData.rot = 7.0f;
	pData.size = glm::vec2(0,0); // 250 250
	pData.type = 1; // 0
	sandStormContainer->particleData.push_back(pData);
	graphics->updateVBuffer(bufferIndex, (void**)&sandStormContainer->particleData[0],sandStormContainer->particleData.size(),vertexSize);

	streamOutTech	= new ShaderClass("particleCloudVSStreamOut",	"particleCloudGSStreamOut",	"","","",							graphics,(int)LayoutTypes::LAYOUT_PARTICLECLOUD_SO, true);
	depthTech		= new ShaderClass("particleCloudVS",			"particleCloudGSDepth",		"","","particleCloudPSDepth",		graphics,(int)LayoutTypes::LAYOUT_PARTICLECLOUD_DEPTH, false);
	opacityTech		= new ShaderClass("particleCloudVS",			"particleCloudGSOpacity",	"","","particleCloudPSOpacity",		graphics,(int)LayoutTypes::LAYOUT_PARTICLECLOUD_OPACITY,false);
	drawTech		= new ShaderClass("particleCloudVS",			"particleCloudGS",			"","","particleCloudPS",			graphics,(int)LayoutTypes::LAYOUT_PARTICLECLOUD, false);
	projectTech		= new ShaderClass("screenParticleVS",			"screenParticleGS",			"","","screenParticlePS",			graphics,(int)LayoutTypes::LAYOUT_PARTICLECLOUD_FINAL, false);
	finalTech		= new ShaderClass("screenParticleVS",			"screenParticleGS",			"","","screenParticlePSNoDiscard",	graphics,(int)LayoutTypes::LAYOUT_PARTICLECLOUD_FINAL, false);

	cbsandStormIndex = graphics->getHandleIndex("cbSandStorm", ResourceType::ConstantBuffer);
}

void SandStorm::update(float _deltaTime)
{
	//static int a = 0;
	//if (a != 200)
	//	a++;
	//if (a > 100 && a != 200)
	//{
	//	a = 200;
	//	sandStormContainer->emitterPositions.at(0) = glm::vec3(70000, 100, 80000);
	//	sandStormContainer->firstTime.at(0) = true;
	//	sandStormContainer->particleData.clear();
	//	CloudParticle pData;
	//	pData.pos = sandStormContainer->emitterPositions.at(0);
	//	pData.age = 5.f;
	//	pData.rot = 7.f;
	//	pData.size = glm::vec2(250, 250);
	//	pData.type = 0;
	//	sandStormContainer->particleData.push_back(pData);
	//	graphics->updateVBuffer(bufferIndex, (void**)&sandStormContainer->particleData[0], sandStormContainer->particleData.size(), sizeof(CloudParticle));
	//}

	age += _deltaTime;
	//glm::vec3 _windDirection	= glm::vec3(1,0,0);
	UINT strides[1]				= {sizeof(CloudParticle)};

	UINT offsets[1]				= {0};

	unsigned int index			= 0;

	graphics->setTopology(1);
	graphics->setIndexBuffer(-1, 0);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_PARTICLECLOUD_SO);
	graphics->setGeometryShaderResource(0, graphics->getRandomSeedIndex(), 1 );
	if( sandStormContainer->firstTime.at(index) )
	{
		graphics->setVertexBuffers(0,bufferIndex,1,strides,offsets);
		streamOutTech->use();
		graphics->setSOTargets( 1,streamOutIndexFirst, offsets );
		graphics->draw(1,0);
		graphics->resetSOTargets();
		sandStormContainer->firstTime.at(index) = false;
	}
	else
	{
		graphics->setVertexBuffers(0,streamOutIndexFirst,1,strides,offsets);
		streamOutTech->use();
		graphics->setSOTargets( 1,streamOutIndexSecond, offsets );
		graphics->drawAuto();
		graphics->resetSOTargets();

		uint temp = streamOutIndexFirst;
		streamOutIndexFirst = streamOutIndexSecond;
		streamOutIndexSecond = temp;
	}
	float distance = 19000.0f;
	//update CB
	//sandStormContainer->emitterPositions.at(index).x += cbContainer->cbEveryFrame.windDirection.x * cbContainer->cbEveryFrame.timeStep * 10;
	//sandStormContainer->emitterPositions.at(index).z += cbContainer->cbEveryFrame.windDirection.y * cbContainer->cbEveryFrame.timeStep * 10;

	glm::vec3 temp;
	temp.x = sandStormContainer->emitterPositions.at(index).x - 100;
	temp.y = sandStormContainer->emitterPositions.at(index).y - 100;
	temp.z = sandStormContainer->emitterPositions.at(index).z - 100;

	glm::vec3 sunDir = cbContainer->cbLight.lightDir;
	glm::vec3 sunUp = glm::cross(glm::cross(glm::vec3(1, 0, 0), sunDir), sunDir);
	glm::vec3 viewPos = sandStormContainer->emitterPositions.at(index) - sunDir * distance;

	CBSandStorm sandStormData;
	sandStormData.pos = viewPos;
	sandStormData.DepthView = glm::lookAt(viewPos, sandStormContainer->emitterPositions.at(index), sunUp);
	sandStormData.age = age;

	graphics->updateCBuffer(cbsandStormIndex, &sandStormData);
	//graphics->updateCBSandStorm( &sandStormContainer->emitterPositions.at(index), 19000 , age);
}

void SandStorm::resetParticle(int _index)
{
	sandStormContainer->firstTime.at(_index) = true;
}

int SandStorm::randomInteger(int _min, int _max)
{
	return (rand() % (_max - _min) + _min);
}

void SandStorm::draw()
{
	unsigned int index = 0;

	UINT strides = sizeof(CloudParticle);
	UINT offsets = 0;

	graphics->setTopology(1);
	graphics->setCBuffers(5, 1, cbsandStormIndex);

	//graphics->updateVBuffer(bufferIndex, (void**)&sandStormContainer->particleData[0],sandStormContainer->particleData.size(),strides);
	graphics->setVertexBuffers(0,streamOutIndexSecond,1,&strides,&offsets);
	//graphics->setToScreenRender(-1);

	//int w = graphics->getResolution().x;
	//int h = graphics->getResolution().y;

	//graphics->setViewport( w, h );
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_PARTICLECLOUD_DEPTH);

	graphics->setRenderTargetViews(0, 1, depthMapRTV );
	graphics->setDepthStencilState(0);

	graphics->setGeometryShaderResource(0, graphics->getRandomSeedIndex(), 1 );
	graphics->setPixelShaderResource(0, graphics->getRandomSeedIndex(), 1 );
	graphics->setPixelShaderResource(1,sandStormContainer->imageIndices.at(index), 1 );
	graphics->setBlendState(4);

	depthTech->use();

	graphics->drawAuto();
	//----------------------------------------------------------------------------------
	//opacity map
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_PARTICLECLOUD_OPACITY);

	graphics->setRenderTargetViews(opacityMapRTV, 1, 0 );
	//graphics->setRenderTargetViews(0, 1, -1 );
	graphics->setDepthStencilState(3);

	graphics->setPixelShaderResource(1,sandStormContainer->imageIndices.at(index), 1 );
	graphics->setPixelShaderResource(2, depthMapSRV, 1);

	opacityTech->use();

	graphics->drawAuto();
	//----------------------------------------------------------------------------------
	//render to lower resolution rendertarget (to reduce fillrate)
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_PARTICLECLOUD);
	graphics->setRenderTargetViews(drawRTV, 1, -1 );
	//graphics->setRenderTargetViews(0, 1, -1 );
	//graphics->setToScreenRender(-1);
	graphics->setViewport((int)graphics->getResolution().x / 4, (int)graphics->getResolution().y / 4);
	graphics->setPixelShaderResource(3, graphics->getDepthResourceView(), 1);
	graphics->setPixelShaderResource(4, opacityMapSRV, 1);
	graphics->setDepthStencilState(3);
	
	graphics->setBlendState(2);

	drawTech->use();

	graphics->drawAuto();
	//----------------------------------------------------------------------------------
	//final render
	strides = sizeof(ScreenParticle);
	graphics->setVertexBuffers(0,bufferIndexOverlay,1,&strides,&offsets);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_PARTICLECLOUD_FINAL);
	graphics->setRenderTargetViews(0, 1, edgeStencilRTV );
	graphics->setPixelShaderResource(1, drawSRV, 1);
	graphics->setDepthStencilState(0);
	graphics->setBlendState(2);
	graphics->setViewport( (int)graphics->getResolution().x, (int)graphics->getResolution().y );

	projectTech->use();

	graphics->draw(1,0);


	//render edges in final rendertarget

	graphics->setVertexBuffers(0,streamOutIndexSecond,1,&strides,&offsets);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_PARTICLECLOUD);
	graphics->setRenderTargetViews(finalDrawRTV, 1, edgeStencilRTV );
	//graphics->setRenderTargetViews(0, 1, -1 );
	//graphics->setToScreenRender(-1);
	//graphics->setViewport( graphics->getResolution().x/4, graphics->getResolution().y/4 );
	graphics->setPixelShaderResource(1, sandStormContainer->imageIndices.at(index), 1 );
	graphics->setPixelShaderResource(2, depthMapSRV, 1);
	graphics->setPixelShaderResource(3, graphics->getDepthResourceView(), 1);
	graphics->setPixelShaderResource(4, opacityMapSRV, 1);
	graphics->setDepthStencilState(2);
	
	graphics->setBlendState(2);

	drawTech->use();

	graphics->drawAuto();

	//render final rendertarget

	strides = sizeof(ScreenParticle);
	graphics->setVertexBuffers(0,bufferIndexOverlay,1,&strides,&offsets);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_PARTICLECLOUD_FINAL);
	graphics->setRenderTargetViews(0, 1, edgeStencilRTV );
	graphics->setPixelShaderResource(1, finalDrawSRV, 1);
	graphics->setDepthStencilState(0);
	graphics->setBlendState(2);
	graphics->setViewport((int)graphics->getResolution().x, (int)graphics->getResolution().y);

	finalTech->use();

	graphics->draw(1,0);
}