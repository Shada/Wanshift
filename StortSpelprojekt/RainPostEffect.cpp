#include "RainPostEffect.h"
#include "MainState.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Utility_PostEffect.h"

struct RainDrop
{
	glm::vec3	position;
	glm::vec3	velocity;
	float		state;
};

RainPostEffect::RainPostEffect(UINT _groupSize, UINT _gridSizeMultiple, UINT _heightMapSize)
	: PostEffect("rain")
{
	lerpTime = 0;
}

RainPostEffect::~RainPostEffect()
{
}

glm::mat4 createOrtho(float _left, float _right, float _top, float _bottom, float _near, float _far)
{
	glm::mat4 ort;

	ort[0][0] = (float)2.0 / (_right - _left);
	ort[0][1] = (float)0;
	ort[0][2] = (float)0;
	ort[0][3] = (float)0;
	ort[1][0] = (float)0;
	ort[1][1] = (float)2.0 / (_top - _bottom);
	ort[1][2] = (float)0;
	ort[1][3] = (float)0;
	ort[2][0] = (float)0;
	ort[2][1] = (float)0;
	ort[2][2] = (float)1.0 / (_far - _near);
	ort[2][3] = (float)0;
	ort[3][0] = (float)(_left + _right) / (_left - _right);
	ort[3][1] = (float)(_top + _bottom) / (_bottom - _top);
	ort[3][2] = (float)_near / (_near - _far);
	ort[3][3] = (float)1;

	return ort;
}

glm::mat4 mul(glm::mat4 _a, glm::mat4 _b)
{
	glm::mat4 m;

	for(int i = 0; i < 4; i++)
	{
		m[i][0] =	( _a[i][0] * _b[0][0] ) +
					( _a[i][1] * _b[1][0] ) +
					( _a[i][2] * _b[2][0] ) +
					( _a[i][3] * _b[3][0] );

		m[i][1] =	( _a[i][0] * _b[0][1] ) +
					( _a[i][1] * _b[1][1] ) +
					( _a[i][2] * _b[2][1] ) +
					( _a[i][3] * _b[3][1] );

		m[i][2] =	( _a[i][0] * _b[0][2] ) +
					( _a[i][1] * _b[1][2] ) +
					( _a[i][2] * _b[2][2] ) +
					( _a[i][3] * _b[3][2] );

		m[i][3] =	( _a[i][0] * _b[0][3] ) +
					( _a[i][1] * _b[1][3] ) +
					( _a[i][2] * _b[2][3] ) +
					( _a[i][3] * _b[3][3] );
	}
	glm::mat4 c = _a * _b;

	return m;
}

void RainPostEffect::updateTransforms()
{
	/* Update transformations */
	
	CameraContainer* cam = containers->get<CameraContainer>(Containers::CAMERA);

	/* view direction */
	glm::vec3 camPos= glm::vec3(cam->vEye);
	glm::vec3 camDir= glm::vec3(cam->vLookDir);
	camDir			= glm::vec3(camDir.x, 0.0f, camDir.z);
	camDir			= glm::normalize(camDir);

	glm::vec3 boundHalfSize = currentSimulationState.boundHalfSize;
	/* volume center */
	glm::vec3 offset		= glm::vec3(camDir.x * boundHalfSize.x, 0.0f, camDir.z * boundHalfSize.z);
	glm::vec3 boundCenter	= camPos + offset * 0.5f;//0.2f;

	/* rainviewproj ortho matrix*/
	glm::vec3 eye	= boundCenter + glm::vec3(0.0f, boundHalfSize.y, 0.0f);
	glm::vec3 at	= boundCenter;
	glm::vec3 up	= glm::vec3(0,0,1.0);
	
	glm::mat4 view = glm::lookAt(eye,at,up);

	glm::mat4 proj = createOrtho(-boundHalfSize.x, boundHalfSize.x, -boundHalfSize.y, boundHalfSize.y, 0.0f, 2.0f * boundHalfSize.z);

	// update  render cbuffer
	currentRainState.rViewProjMatrix	= mul(view, proj);
	currentRainState.viewDir			= camDir;

	// update simulation cbuffer
	//currentSimulationState.boundCenter	= camPos + offset * 0.8f + rsv2.centerOffset;
	currentSimulationState.boundCenter	= camPos + offset * 0.5f + rsv1.centerOffset;
}

void RainPostEffect::updateSimConstants(float _dt)
{
	glm::vec2 currentWindEffect	= currentSimulationState.windForce;
	
	float rX = (float)(rand() % 101) * 0.02f - 1.0f;
	currentWindEffect.x += rX * maxWindVariance * _dt;
	if(currentWindEffect.x > maxWindEffect)
		currentWindEffect.x = maxWindEffect;
	else if(currentWindEffect.x < -maxWindEffect)
		currentWindEffect.x = -maxWindEffect;
	
	
	float randY = (float)(rand() % 101) * 0.02f - 1.0f;
	currentWindEffect.y += randY * maxWindVariance * _dt;
	if(currentWindEffect.y > maxWindEffect)
		currentWindEffect.y = maxWindEffect;
	else if(currentWindEffect.y < -maxWindEffect)
		currentWindEffect.y = -maxWindEffect;

	currentSimulationState.rDt			= _dt;
	currentSimulationState.windForce	= currentWindEffect;
	
	graphics->updateCBuffer( graphics->getHandleIndex("cbRainSimulationConstants",ResourceType::ConstantBuffer), &currentSimulationState );
}

void RainPostEffect::updateRainConstants(float _dt)
{
	CameraContainer* cam = containers->get<CameraContainer>(Containers::CAMERA);
	
	glm::mat4 camViewProj	= mul( cam->viewMatrix, cam->projMatrix );
	camViewProj				= glm::transpose(camViewProj);
	glm::vec3 camDir		= cam->vLookDir;
	camDir					= glm::normalize(camDir);

	currentRainState.rViewProjMatrix= camViewProj;
	currentRainState.viewDir		= camDir;

 	graphics->updateCBuffer( graphics->getHandleIndex("cbRainConstants",ResourceType::ConstantBuffer), &currentRainState );
}

void RainPostEffect::generateHeightMap()
{
	/* resize the viewport */
	//graphics->setViewport( heightMapSize, heightMapSize );

	/* create orthographic matrix */
	updateTransforms();

	/* prepare shader */
	//graphics->clearDepthStencil( heightMapDepthView );

	graphics->setNullRenderTargetView();

	//graphics->setInputLayout(

	// shader
	//heightMapShader->use();

	/* reset the viewport to global resolution */
	int w, h;
	w = (int)graphics->getResolution().x;
	h = (int)graphics->getResolution().y;
	graphics->setViewport( w, h );
}

void RainPostEffect::preProcess(float _dt)
{
	/* */
	Containers container(std::make_pair(Containers::BIOMS, READ),std::make_pair(Containers::PLAYER,READ));
	BiomsContainer& bc = *container.get<BiomsContainer>(Containers::BIOMS);
	PlayerContainer& pc = *container.get<PlayerContainer>(Containers::PLAYER);

	int size = bc.bufferCloudMap.size();
	float cloudSize = bc.bufferCloudMap[size/2][size/2];
	cloudSize = 1;

	static const float endTime = 100.0f;
	//static float lerpTime;
	if(cloudSize <= 0)
		return;

	if(lerpTime > endTime)
	{
		lerpTime -= _dt * simSettings.simulationSpeed;
		lerpTime = endTime;
	}
	//else if(lerpTime < endTime)
	//{
	//	lerpTime += _dt * simSettings.simulationSpeed;
	//}

	// current & new 
	float cDen		= rsv1.rainDensity;
	float cVariance = rsv1.maxWindVariance;
	float cEffect	= rsv1.maxWindEffect;

	float nDen		= rsv2.rainDensity;	
	float nVariance = rsv2.maxWindVariance;	
	float nEffect	= rsv2.maxWindEffect;

	float percent = 0;// lerpTime / endTime;
	
	rainDensity		= POST::interpolate<float>( cDen, nDen, percent );
	maxWindVariance	= POST::interpolate<float>(cVariance, nVariance, percent );
	maxWindEffect	= POST::interpolate<float>( cEffect, nEffect, percent );

	glm::vec3 cBound = rsv1.boundHalfSize;
	glm::vec3 nBound = rsv2.boundHalfSize;

	currentSimulationState.boundHalfSize	= POST::interpolate<glm::vec3>( cBound, nBound, percent );

	//lerpTime += _dt * simSettings.simulationSpeed;

	if(rainDensity > 1.0)
	{
		rainDensity = 1;
	}
	if(rainDensity < 0.0)
	{
		rainDensity = 0;
	}

	updateTransforms();
	updateSimConstants(_dt * simSettings.simulationSpeed);
	graphics->setCBuffers(12,1,graphics->getHandleIndex("cbRainSimulationConstants",ResourceType::ConstantBuffer));
	graphics->CSexecSingle( rainBufferUAV, noiseTextureIndex, computeShaderIndex, simSettings.groupSize, simSettings.groupSize );

	//printf("LERP:%f\n",lerpTime);
}

void RainPostEffect::draw(float _dt, int _renderTarget)
{
	graphics->setPixelShaderResource(0,-1,14);
	if(!active)
		return;

	updateRainConstants(_dt * simSettings.simulationSpeed);

	// set shaders
	shader->use();
	
	graphics->setTopology(0);
	graphics->setBlendState(3);
	graphics->setRasterState(2);

	graphics->setRenderTargetViews(_renderTarget, 1, 0);
	//graphics->setToScreenRender(-1);

	graphics->setCBuffers(12,1,graphics->getHandleIndex("cbRainSimulationConstants",ResourceType::ConstantBuffer));
	graphics->setCBuffers(13,1,graphics->getHandleIndex("cbRainConstants",ResourceType::ConstantBuffer));
	graphics->setVertexShaderResource( 1, rainSimBufferView, 1 );
	graphics->setPixelShaderResource( 1, rainTextureIndex, 1);

	graphics->setInputLayout((int)LayoutTypes::LAYOUT_VERTEXID);
	graphics->setNullVertexBuffers();

	int tot = (int)((float)maxRainDrops * rainDensity);

	graphics->draw( tot * 6, 0);

	graphics->setBlendState(0);
	
	graphics->setVertexShaderResource( 1,-1, 1 );
}

void RainPostEffect::setRainState(const std::string& _weatherType, const std::string& _startState, const std::string& _endState)
{
	currentType = _weatherType;
	simSettings			=  containers->get<RainSimContainer>(Containers::Type::RAIN_SIM)->getSettings(_weatherType + "_Settings");
	RainSimConstants rsc= containers->get<RainSimContainer>(Containers::Type::RAIN_SIM)->getRainConstants(_weatherType + "_Constants");

	rsv1 = containers->get<RainSimContainer>(Containers::Type::RAIN_SIM)->getRainVariables(_weatherType + "_" + _startState);
	rsv2 = containers->get<RainSimContainer>(Containers::Type::RAIN_SIM)->getRainVariables(_weatherType + "_" + _endState);

	/* local members */
	int gridSize	= simSettings.gridSize * simSettings.groupSize;
	maxRainDrops	= gridSize * gridSize;
	maxWindVariance	= rsv1.maxWindVariance;
	maxWindEffect	= rsv1.maxWindEffect;
	rainDensity		= rsv1.rainDensity;

	// render cbuffer
	currentRainState.ambientColor	= rsc.color;
	currentRainState.rScale			= rsc.particleScale;
	
	// simulation cbuffer
	currentSimulationState.boundHalfSize		= rsv1.boundHalfSize;
	currentSimulationState.heightMapHalfSize	= (float)rsv1.heightMapSize;
	currentSimulationState.verticalSpeed		= rsv1.verticalSpeed;
	currentSimulationState.windVariation		= rsv1.maxWindVariance;
	currentSimulationState.stretch				= rsv1.stretch;

	/* Shader Resource Views */
	noiseTextureIndex = graphics->getHandleIndex(simSettings.noiseMap, ResourceType::ShaderResourceView);
	rainTextureIndex = graphics->getHandleIndex(simSettings.textureName, ResourceType::ShaderResourceView);
}

void RainPostEffect::initPostEffect(MainState& _mainState)
{
	containers = &_mainState.c;

	setRainState("Rain", "Medium", "Medium");

	std::string shaderName = getName();

	computeShaderIndex = graphics->createComputeShader(shaderName + "CS");

	shader	= new ShaderClass("rainVS", "", "", "", "rainPS", graphics, (int)LayoutTypes::LAYOUT_VERTEXID, false);

	int textureIndex = graphics->createTexture2D(128,128);
	renderTarget = graphics->createRenderTargetView(textureIndex);
	renderTexture = graphics->createShaderResourceView(textureIndex);

	/* Buffers */
	int rainVBStride= sizeof(RainDrop);
	int rainVBSize	= rainVBStride * maxRainDrops;

	// init all to zero (to force simulation to kick off)
	RainDrop* drops = new RainDrop[maxRainDrops];
	ZeroMemory( drops, sizeof(drops) * maxRainDrops );
	for(int i = 0; i < maxRainDrops; ++i)
	{
		drops[i].position	= glm::vec3(0, -1000.0f, 0);
		drops[i].velocity	= glm::vec3(0, simSettings.simulationSpeed, 0);
		drops[i].state		= 0.0f;
	}

	// create the buffer
 	rainSimBuffer = graphics->createUABuffer((void**)&drops[0], rainVBSize, rainVBStride);

	/* Views */
	rainSimBufferView = graphics->createCustomShaderResourceView( maxRainDrops, rainSimBuffer);
	graphics->addHandle("raintest", ResourceType::ShaderResourceView,rainSimBufferView);

	/* UAVs */
	rainBufferUAV = graphics->createUAV( rainSimBuffer, maxRainDrops );

	/* clean up */
	delete[] drops;
}

void RainPostEffect::setArgs(void* _argData)
{
	RainArgData rad		= *(RainArgData*)_argData;
	std::string type	= rad.weatherType;
	std::string start	= rad.startState;
	std::string end		= rad.endState;

	if(currentType != type)
	{
		printf("--- weather switch ---\ntype:%s\nstart:%s\nend:%s\n",type.c_str(),start.c_str(),end.c_str());
		setRainState(type,start,end);
		lerpTime = 0;
	}
}