#pragma once

#include <glm/glm.hpp>
#include "RenderInterface.h"
#include "ReaderINI.h"
#include "AtmosphereContainer.h"
#include "ConstantBufferContainer.h"

class Atmosphere
{
public:
	Atmosphere();
	~Atmosphere();

	/* initialize the atmoshpere values
		
		#_graphics: renderInterface to allow cbuffer updating */
	void init(RenderInterface* _graphics);

	/* update cbuffers
	
		#_graphics_ renderInterface to allow cbuffer updating */
	void update(RenderInterface* _graphics, float _dt);

	// update rayleigh & mie constants
	
	//#_graphics: renderInterface 
	void setScatterAttributes(RenderInterface* _graphics, ScatterType _current);

private:
	
	struct QualitySettings
	{
		UINT nSamples;	// 4
		float scaleDepth;		// 4
		float scaleOverDepth;	// 4
		float padding;			// 4
	};	
	/* update quality constants cbuffer
	
		#_graphics: renderInterface */
	void setQuality(RenderInterface* _graphics);

	/* update world constants cbuffer
	
		#_graphics: renderInterface */
	void setWorldAttributes(RenderInterface* _graphics);

	CBWorldAttributes wa;
	QualitySettings qs;
	CBScatterAttributes sa;

	//int qsIndex;
	int waIndex;
	int saIndex;

	AtmosphereContainer* atmospheres;
	ConstantBufferContainer* constantbuffers;
};
