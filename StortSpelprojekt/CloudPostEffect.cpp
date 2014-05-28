#include "CloudPostEffect.h"

#include "Containers.h"

CloudPostEffect::CloudPostEffect()
	: PostEffect("Cloud")
{

}

CloudPostEffect::~CloudPostEffect()
{

}

void CloudPostEffect::initPostEffect(MainState& _mainState)
{
	//cloudTexture = graphics->getHandleIndex("default", ResourceType::ShaderResourceView);

	Containers container(std::make_pair(Containers::BIOMS, READ));
	BiomsContainer bc = *container.get<BiomsContainer>(Containers::BIOMS);

	int vbSize	= bc.cloudSize * bc.cloudSize;	// number of cloud points
	int vbStride= sizeof(float); 

	cloudDynamicVB = graphics->createVBufferDynamic( vbSize, vbStride );
	cbCloud cb;
	cb.cellSize = CLOUD_CELLSIZE;
	cb.offset = bc.cloudMapPosition;
	cb.size = bc.cloudSize;
	cb.intervalStart	= 10;
	cb.intervalEnd		= 20;



	int tex3d = graphics->createTexture3D(128,128,4);
	graphics->createShaderResourceView3D(tex3d);

	graphics->updateCBuffer( graphics->getHandleIndex("cbCloud", ResourceType::ConstantBuffer), &cb );

	shader = new ShaderClass("cloudVS", "cloudGS", "", "", "cloudPS", graphics, (int)LayoutTypes::LAYOUT_1FLOAT, false);
}

void CloudPostEffect::preProcess(float _dt)
{
	//Containers container(std::make_pair(Containers::BIOMS, READ));
	//BiomsContainer bc = *container.get<BiomsContainer>(Containers::BIOMS);

	//graphics->updateVBuffer( cloudDynamicVB, (void**)&bc.bufferCloudMap, bc.cloudSize * bc.cloudSize, sizeof(float) );


	
}

void CloudPostEffect::draw(float _dt, int _renderTarget)
{

	graphics->setPixelShaderResource(0,-1,14);

	shader->use();

	graphics->setTopology(1);
	graphics->setBlendState(2);
	graphics->setRasterState(2);
	graphics->setDepthStencilState(2);
	//graphics->setRenderTargetViews(_renderTarget, 1, 0);
	graphics->setToScreenRender(0);
	Containers container(std::make_pair(Containers::BIOMS, READ));
	BiomsContainer& bc = *container.get<BiomsContainer>(Containers::BIOMS);

	//graphics->updateVBuffer( cloudDynamicVB, (void**)&bc.bufferCloudMap[0][0], bc.cloudSize * bc.cloudSize, sizeof(float) );

	// update cbuffer
	cbCloud cb;
	cb.cellSize = CLOUD_CELLSIZE;
	cb.offset	= bc.cloudMapPosition;
	cb.size		= bc.cloudSize;
	cb.intervalStart	= 0;
	cb.intervalEnd		= 14;

	graphics->updateCBuffer( graphics->getHandleIndex("cbCloud",ResourceType::ConstantBuffer), &cb);
	graphics->setCBuffers( 10, 1, graphics->getHandleIndex("cbCloud", ResourceType::ConstantBuffer) );
	graphics->setPixelShaderResource( 9, graphics->getHandleIndex("cloud", ResourceType::ShaderResourceView), 1 );

	graphics->setInputLayout( (int)LayoutTypes::LAYOUT_1FLOAT );

	UINT stride = sizeof(float);
	UINT offset = 0;
	//graphics->setVertexBuffers(1, cloudDynamicVB, 1, &stride, &offset);
	graphics->setNullVertexBuffers();
	int vbSize	= bc.cloudSize * bc.cloudSize;	// number of cloud points
	
	graphics->draw(vbSize,0);
}