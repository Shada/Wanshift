#include "PostEffectsAspect.h"

#include "MainState.h"
#include "RainPostEffect.h"
#include "ShimmerPostEffect.h"
#include "CloudPostEffect.h"
#include "FogPostEffect.h"

#include "Utility.h"
#include "Utility_PostEffect.h"

// x = desert, y = mountain, z = meadow, w = forrest, x1 = swamp


PostEffectsAspect::PostEffectsAspect(void)
	: AspectInterface("PostEffects", 0)
{
}


PostEffectsAspect::~PostEffectsAspect(void)
{
}

void PostEffectsAspect::init(MainState& _mainState)
{
	graphics = _mainState.graphics;

	glm::vec2 r		= graphics->getResolution();
	int texture		= graphics->createTexture2D( (int)r.x, (int)r.y);
	renderTarget	= graphics->createRenderTargetView(texture);
	renderTexture	= graphics->createShaderResourceView(texture);

	graphics->addHandle("postEffectTexture", ResourceType::ShaderResourceView, renderTexture);
	graphics->addHandle("postEffectRenderTarget", ResourceType::RenderTargetView, renderTarget);

	// add all post effects before effect->init	
	//postEffects["Cloud"]	= new CloudPostEffect();	// clouds are not ready to be implemented
	postEffects["Haze"]		= new ShimmerPostEffect();	
	postEffects["Rain"]		= new RainPostEffect(4,4,512);
	postEffects["Fog"]		= new FogPostEffect();
	
	for( auto pIt: postEffects )
	{
		pIt.second->init(_mainState);
	}

	softies = new ParticleAspect();
	softies->init(_mainState);
}

/* Note: We lack a proper system to update weather changes,
	this is not a good solution */
void PostEffectsAspect::runAspect(float _dt)
{
	/* Find the major biom */
	Containers containers(std::make_pair(Containers::BIOMS, READ), std::make_pair(Containers::PLAYER, READ), std::make_pair(Containers::TERRAIN, READ), std::make_pair(Containers::CONSTBUFFER, MODIFY), std::make_pair(Containers::SANDSTORM, READ));
	PlayerContainer& pc		= *containers.get<PlayerContainer>(Containers::PLAYER);
	BiomsContainer &bc		= *containers.get<BiomsContainer>(Containers::BIOMS);
	cbContainer				= containers.get<ConstantBufferContainer>(Containers::CONSTBUFFER);

	glm::vec3 pos	= pc.position;
	SingleBiom& b	= Utility::getBlendMapValue(&bc, pos);
	glm::vec4 v		= b.map1;

	int major			= POST::getDominantBiomId(v);
	float majorVal		= v[major];
	POST::PWEATHER bw	= (POST::PWEATHER)major;

	if(majorVal == 0)
		bw = POST::PWEATHER::FORREST;

	float ToD = cbContainer->cbEveryFrame.timeofDay;
	
	/* Set weather Arguments based on the current major biom */
	switch(bw)
	{
	case POST::PWEATHER::DESERT:		// haze
		{
			postEffects["Haze"]->toggleActive(true);
			postEffects["Fog"]->toggleActive(true);

			RainArgData rad;

			/* find out whether you're in a sandstorm or just a regular desert */
			SandStormContainer& sc = *containers.get<SandStormContainer>(Containers::SANDSTORM);

			if (sc.insideSandstorm)
			{
				rad.weatherType = "Fog_Sandstorm";
			}
			else
			{
				rad.weatherType = "Fog_Desert";
			}

			postEffects["Fog"]->setArgs((void*)&rad);
		}
		break;
	case POST::PWEATHER::MEADOW:		// rain
		{
			postEffects["Rain"]->toggleActive(true);
		
			RainArgData rad;
			rad.weatherType = "Rain";
			rad.startState	= "Medium";
			rad.endState	= "Heavy";

			postEffects["Rain"]->setArgs((void*)&rad);

			rad.weatherType = "Fog_Meadow";
			postEffects["Fog"]->setArgs((void*)&rad);
		}
		break;
	case POST::PWEATHER::MOUNTAIN:	// snow
		{
			postEffects["Rain"]->toggleActive(true);
		
			RainArgData rad;
			rad.weatherType = "Snow";
			rad.startState	= "Medium";
			rad.endState	= "Heavy";

			postEffects["Rain"]->setArgs((void*)&rad);

			rad.weatherType = "Fog_Mountain";
			postEffects["Fog"]->setArgs((void*)&rad);
		}
		break;
	case POST::PWEATHER::SWAMP:		// rain
		{
			postEffects["Rain"]->toggleActive(false);
			RainArgData rad;
			rad.weatherType = "Fog_Swamp";
			postEffects["Fog"]->setArgs((void*)&rad);
		}
		break;
	case POST::PWEATHER::FORREST:		// rain
		{
			postEffects["Rain"]->toggleActive(true);
		
			RainArgData rad;
			rad.weatherType = "Rain";
			rad.startState	= "Light";
			rad.endState	= "Light";

			postEffects["Rain"]->setArgs((void*)&rad);

			rad.weatherType = "Fog_Forrest";
			postEffects["Fog"]->setArgs((void*)&rad);
		}
		break;
	}

	if (ToD <= 0.25f)	// night, switch regular weather for meteor showers
	{
		postEffects["Rain"]->toggleActive(true);

		RainArgData rad;
		rad.weatherType = "Meteor";
		rad.startState = "Light";
		rad.endState = "Light";

		postEffects["Rain"]->setArgs((void*)&rad);
	}

	/* --- Dispatch all drawcalls ---
		(note: preprocessing should have been separated into its own aspect and run
		during the regular game updates instead */

	/* note: there's a strict order whitch to draw the effects
		- due to no proper scene graph this has been hell
		*** if clouds are to be reimplemented they will probably have to be rendered
		before the softies */

	/* SOFTIES */
	softies->run(_dt);

	/* Post Effects */
	postEffects["Fog"]->preProcess(_dt);
	postEffects["Fog"]->draw(_dt,renderTarget);


	postEffects["Haze"]->preProcess(_dt);
	postEffects["Haze"]->draw(_dt,renderTarget);

	postEffects["Rain"]->preProcess(_dt);
	postEffects["Rain"]->draw(_dt,renderTarget);
 
	/* Toggle all post effects to not active (not a good solution) */
	for( auto pe : postEffects )
	{
		pe.second->toggleActive(false);
	}		
}