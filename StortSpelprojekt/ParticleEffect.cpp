#include "ParticleEffect.h"
#include "EnemyContainer.h"

ParticleEffect::ParticleEffect( UINT _particleContainerIndex, UINT _enemyIndex, UINT _enemySubIndex)
{
	containerIndex	= _particleContainerIndex;
	enemyIndex		= _enemyIndex;
	enemySubIndex	= _enemySubIndex;
}


ParticleEffect::~ParticleEffect()
{
}

void ParticleEffect::init(RenderInterface *_graphics, ParticleEffectContainer *_particleEffectContainer, EnemyContainer *_enemyContainer, UINT _vertexIndex, UINT _vertexAmount)
{
	enemyContainer				= _enemyContainer;
	particleEffectContainer		= _particleEffectContainer;
	graphics					= _graphics;
	
	particleEffectContainer->vertexIndex.at(containerIndex)		= _vertexIndex;
	particleEffectContainer->vertexAmount.at(containerIndex)	= _vertexAmount;
	UINT start = particleEffectContainer->particleData.size();
	UINT end = start + WISPSPARKLES;
	float fraction = 0.5f / (float)WISPSPARKLES;
	particleEffectContainer->particleData.resize(particleEffectContainer->particleData.size() + WISPSPARKLES);
	UINT counter = 0;
	for(UINT i = start; i < end; i++)
	{
		resetParticle(i);
		particleEffectContainer->particleData.at(i).alpha = 0.5f - (fraction * counter);
		counter++;
	}

	//drawTech	= new ShaderClass("wispVS",	"wispGS",	"","","wispPS",							graphics,(int)LayoutTypes::LAYOUT_PARTICLE, false);
}

void ParticleEffect::update(float _deltaTime)
{
	UINT start, end;
	start = particleEffectContainer->vertexIndex.at(containerIndex);
	end = start + WISPSPARKLES;
	for(UINT i = start; i < end; i++)
	{
		particleEffectContainer->particleData.at(i).pos.y -= 0.05f;
		particleEffectContainer->particleData.at(i).alpha -= 0.03f;
		particleEffectContainer->particleData.at(i).size -= glm::vec2(0.06f, 0.06f);
		if(particleEffectContainer->particleData.at(i).alpha <= 0)
		{
			resetParticle(i);
		}
	}
}

void ParticleEffect::draw()
{
	UINT strides = sizeof(Particle);
	UINT offsets = 0;

	//graphics->setViewport( w, h );
	//graphics->setRenderTargetViews(0, 1, -1);
	graphics->setTopology(1);
	graphics->setToScreenRender(-1);
	graphics->setDepthStencilState(2);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_PARTICLE);
	drawTech->use();
	graphics->setBlendState(1);
	graphics->setRasterState(2);
	//graphics->setCBuffers(5, 1, cbWispIndex);
	graphics->setVertexBuffers(0, enemyContainer->vBufferExtraIndex.at(enemyIndex), 1, &strides, &offsets);
	CBWisp wosp;
	UINT start = particleEffectContainer->vertexIndex.at(containerIndex);
	UINT end = start + particleEffectContainer->vertexAmount.at(containerIndex);
	for(UINT i = start; i < end; i++)
	{
		graphics->setPixelShaderResource(1, 2, 1);
		graphics->setPixelShaderResource(4, graphics->getDepthResourceView(), 1);
		graphics->draw( particleEffectContainer->vertexAmount.at(containerIndex), particleEffectContainer->vertexIndex.at(containerIndex) );
	}
}
void ParticleEffect::resetParticle(int _index)
{
	particleEffectContainer->particleData.at(_index).alpha = 0.5f;
	particleEffectContainer->particleData.at(_index).pos = enemyContainer->hordeunits.at(enemyIndex * 25+ enemySubIndex).pos;
	particleEffectContainer->particleData.at(_index).rot = 0;
	particleEffectContainer->particleData.at(_index).size = glm::vec2(4,4);
}