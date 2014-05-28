#include "SkyAspect.h"
#include "Container.h"
#include "MainState.h"
#include "EnvironmentAspect.h"
#include "Utility_PostEffect.h"

struct DomeVDesc
{
	glm::vec3 p;
};

SkyAspect::SkyAspect()
		: AspectInterface("Sky", 0),
skyShader(nullptr), moonShader(nullptr)
{

}

SkyAspect::~SkyAspect(void)
{
}

void SkyAspect::init(MainState& _mainState)
{
	graphics = _mainState.graphics;
	input = _mainState.input;

	skyShader = new ShaderClass("skyVS", "", "", "", "skyPS", graphics, (int)LayoutTypes::LAYOUT_SKY, false);
	//moonShader = new ShaderClass("moonVS", "moonGS", "", "", "moonPS", graphics, (int)LayoutTypes::LAYOUT_OVERLAY, false);
	
	atmosphere.init(graphics);

	cbWorldAttributesIndex = graphics->getHandleIndex("cbWorldAttribute",ResourceType::ConstantBuffer);
	cbScatterAttributesIndex = graphics->getHandleIndex("cbScatterAttributes",ResourceType::ConstantBuffer);

	bool res = skyDome.init(160, 40, graphics);

	Containers containers(std::make_pair(Containers::ATMOSPHERE_SIM, READ));
	AtmosphereContainer& ac = *containers.get<AtmosphereContainer>(Containers::ATMOSPHERE_SIM);

	currentScatter	= ac.getScatter("Scatter_Default");
	newScatter		= ac.getScatter("Scatter_Swamp");
	oldScatter		= ac.getScatter("Scatter_Default");
	newSky			= "Scatter_Swamp";
	oldSky			= "Scatter_Default";
}


void SkyAspect::runAspect(float _dt)
{
		// x = desert, y = mountain, z = meadow, w = forrest, x1 = swamp
	/* Find the major & minor */
	Containers containers( std::make_pair(Containers::BIOMS, READ),std::make_pair(Containers::PLAYER, READ),std::make_pair(Containers::TERRAIN, READ), std::make_pair(Containers::ATMOSPHERE_SIM, READ));
	PlayerContainer& pc = *containers.get<PlayerContainer>(Containers::PLAYER);
	BiomsContainer &bc = *containers.get<BiomsContainer>(Containers::BIOMS);
	AtmosphereContainer& ac = *containers.get<AtmosphereContainer>(Containers::ATMOSPHERE_SIM);

	glm::vec3 pos	= pc.position;
	SingleBiom& b	= Utility::getBlendMapValue(&bc, pos);
	glm::vec4 v		= b.map1;

	int major		= POST::getDominantBiomId(v);
	float majorVal	= v[major];

	POST::PWEATHER bw		= (POST::PWEATHER)major;

	if(majorVal == 0)
		bw = POST::PWEATHER::FORREST;

	if(newSky == oldSky)
	switch(bw)
	{
	case POST::PWEATHER::DESERT:
		{
			newScatter = ac.getScatter("Scatter_Desert");
			newSky = "Scatter_Desert";
		}
		break;
	case POST::PWEATHER::FORREST:
		{
			newScatter = ac.getScatter("Scatter_Forrest");
			newSky = "Scatter_Forrest";
		}
		break;
	case POST::PWEATHER::MEADOW:
		{
			newScatter = ac.getScatter("Scatter_Meadow");
			newSky = "Scatter_Meadow";
		}
		break;
	case POST::PWEATHER::MOUNTAIN:
		{
			newScatter = ac.getScatter("Scatter_Mountain");
			newSky = "Scatter_Mountain";
		}
		break;
	case POST::PWEATHER::SWAMP:
		{
			newScatter = ac.getScatter("Scatter_Swamp");
			newSky = "Scatter_Swamp";
		}
		break;
	}

	static float diff;

	if(newSky != oldSky)
	{
		const float mult = 0.0001f;
		diff += _dt * mult;
		
		if(diff >= 1)
		{
			printf("\n--- Sky merge complete ---\nold: %s |current: %s\n",oldSky.c_str(),newSky.c_str());
			diff		= 0;
			oldSky		= newSky;
			oldScatter	= newScatter;
		}

		currentScatter.exposure = POST::interpolate<float>(		oldScatter.exposure,	newScatter.exposure,	diff );
		currentScatter.gM		= POST::interpolate<float>(		oldScatter.gM,			newScatter.gM,			diff );
		currentScatter.km4PI	= POST::interpolate<float>(		oldScatter.km4PI,		newScatter.km4PI,		diff );
		currentScatter.kmEsun	= POST::interpolate<float>(		oldScatter.kmEsun,		newScatter.kmEsun,		diff );
		currentScatter.kr4PI	= POST::interpolate<float>(		oldScatter.kr4PI,		newScatter.kr4PI,		diff );
		currentScatter.krEsun	= POST::interpolate<float>(		oldScatter.krEsun,		newScatter.krEsun,		diff );
		currentScatter.wM		= POST::interpolate<glm::vec3>( oldScatter.wM,			newScatter.wM,			diff );
		currentScatter.wR		= POST::interpolate<glm::vec3>( oldScatter.wR,			newScatter.wR,			diff );


	}

	atmosphere.setScatterAttributes(graphics, currentScatter);

	atmosphere.update(graphics, _dt);
	draw();
}

void SkyAspect::draw()
{

	graphics->setBlendState(1);

	graphics->setCBuffers(4,1,graphics->getHandleIndex("cbLightInfo",ResourceType::ConstantBuffer));
	graphics->setCBuffers(5,1,graphics->getHandleIndex("cbFog",ResourceType::ConstantBuffer));


	graphics->setRasterState(2);

	graphics->setCBuffers(9,1,cbWorldAttributesIndex);
	graphics->setCBuffers(10,1,cbScatterAttributesIndex);


	skyShader->use();

	//int handle = graphics->getHandleIndex("fakeBufferRenderTarget", ResourceType::RenderTargetView);
	int handle = graphics->getHandleIndex("color", ResourceType::RenderTargetView);
	graphics->setRenderTargetViews(handle,1,-1);
	//graphics->setRenderTargetViews(0, 1, 0);
	
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_SKY);
	graphics->setTopology(3);

	UINT stride = sizeof(glm::vec3);
	UINT offset = 0;

	graphics->setVertexBuffers(0, skyDome.getVIndex(), 1, &stride, &offset);
	graphics->setIndexBuffer(skyDome.getBIndex(), offset);

	graphics->drawIndexed(skyDome.getNumIndices(), 0, 0);

	graphics->setRenderTargetViews(-1,1,-1);
}