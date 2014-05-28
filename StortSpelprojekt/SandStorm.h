#pragma once
#include "RenderInterface.h"
#include "SandStormContainer.h"
#include "ConstantBufferContainer.h"
#include "ShaderClass.h"
class SandStorm
{
public:
	SandStorm();
	~SandStorm();

	void init(RenderInterface *_graphics, SandStormContainer *_cloudContainer, ConstantBufferContainer * _cbContainer);
	void start(glm::vec3 _position, SandStormContainer* _sandStormContainer, RenderInterface* _graphics);
	void end();
	void update(float _deltaTime);
	void draw();
	void resetParticle(int _index);
	int randomInteger(int _min, int _max);

private:
	SandStormContainer	*sandStormContainer;
	ConstantBufferContainer * cbContainer;
	RenderInterface		*graphics;
	ShaderClass			*drawTech;
	ShaderClass			*streamOutTech;
	ShaderClass			*depthTech;
	ShaderClass			*opacityTech;
	ShaderClass			*projectTech;
	ShaderClass			*finalTech;

	float				age;

	//buffer indices

	int					bufferIndex;
	int					streamOutIndexFirst;
	int					streamOutIndexSecond;
	int					bufferIndexOverlay;

	int					depthMapTex;
	int					depthMapRTV;
	int					depthMapSRV;

	int					opacityMapTex;
	int					opacityMapRTV;
	int					opacityMapSRV;

	int					edgeStencilTex;
	int					edgeStencilRTV;
	int					edgeStencilSRV;

	int					drawTex;
	int					drawRTV;
	int					drawSRV;

	int					finalDrawTex;
	int					finalDrawRTV;
	int					finalDrawSRV;

	int					cbsandStormIndex;
	int					postEffectRenderTarget;
};
