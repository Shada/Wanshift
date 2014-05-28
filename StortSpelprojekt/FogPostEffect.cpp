#include "FogPostEffect.h"
#include "MainState.h"
#include "Utility_PostEffect.h"

FogPostEffect::FogPostEffect(void)
	: PostEffect("FogPostEffect")
{
}


FogPostEffect::~FogPostEffect(void)
{
}

void FogPostEffect::initPostEffect(MainState& _mainState)
{
	graphics	= _mainState.graphics;
	fogIndex	= graphics->getHandleIndex("cbFog",ResourceType::ConstantBuffer);
	currentFog	= "Fog_None";
	oldFog		= currentFog;

	Containers& c		= Containers(std::make_pair(Containers::FOG_SIM, READ), std::make_pair(Containers::CONSTBUFFER, READ));
	FogContainer& fogc	= *c.get<FogContainer>(Containers::FOG_SIM);
	currentFogSim		= fogc.getFogType(currentFog);
	newFogSim			= currentFogSim;		
	oldFogSim			= currentFogSim;
}

void FogPostEffect::preProcess(float _dt)
{
	static float diff;


	if(currentFog != oldFog)
	{
		const float mult = 0.0001f;
		diff += _dt * mult;

		if(diff >= 1)
		{
			printf("\n--- Fog merge complete ---\nold: %s | current: %s\n", oldFog.c_str(), currentFog.c_str());
			diff		= 0;
			oldFog		= currentFog;
			oldFogSim	= newFogSim;
			
		}	

		currentFogSim.color			= POST::interpolate<glm::vec3>( oldFogSim.color, newFogSim.color, diff );
		currentFogSim.globalDensity	= POST::interpolate<float>( oldFogSim.globalDensity, newFogSim.globalDensity, diff );
		currentFogSim.heightFalloff	= POST::interpolate<float>( oldFogSim.heightFalloff, newFogSim.heightFalloff, diff );
		currentFogSim.highlightColor= POST::interpolate<glm::vec3>( oldFogSim.highlightColor, newFogSim.highlightColor, diff );
		currentFogSim.startDepth	= POST::interpolate<float>( oldFogSim.startDepth, newFogSim.startDepth, diff );
	}
}

void FogPostEffect::setArgs(void* _argData)
{
	RainArgData rad = *(RainArgData*)_argData;
	currentFog		= rad.weatherType;

	Containers& c		= Containers(std::make_pair(Containers::FOG_SIM, READ), std::make_pair(Containers::CONSTBUFFER, READ));
	FogContainer& fogc	= *c.get<FogContainer>(Containers::FOG_SIM);
	newFogSim			= fogc.getFogType(currentFog);
}

void FogPostEffect::draw(float _dt, int _renderTarget)
{
	graphics->updateCBuffer( fogIndex, (void*)&currentFogSim);
}

