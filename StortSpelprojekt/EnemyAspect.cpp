#include "EnemyAspect.h"
#include "MainState.h"

EnemyAspect::EnemyAspect() : AspectInterface("ZZEnemy", 10), c(std::make_pair(Containers::ENEMY, MODIFY), std::make_pair(Containers::TERRAIN, READ), 
															std::make_pair(Containers::PLAYER, READ), std::make_pair(Containers::PARTICLE_EFFECT, MODIFY)
															, std::make_pair(Containers::CONSTBUFFER, MODIFY))
{
	ec = c.get<EnemyContainer>(Containers::ENEMY);
	pl = c.get<PlayerContainer>(Containers::PLAYER);
	pe = c.get<ParticleEffectContainer>(Containers::PARTICLE_EFFECT);
	cbuffers = c.get<ConstantBufferContainer>(Containers::CONSTBUFFER);
}


EnemyAspect::~EnemyAspect()
{}

void EnemyAspect::init(MainState &_ms)
{
	unsigned int wispAmount = 5;
	graphics = _ms.graphics;
	ec->allocate(0, wispAmount);
	// horde behaviour
	float randx, randz;
	float multiplier = 1.f / float(RAND_MAX);
	int hordeunitindex;
	glm::vec3 initpos(0.f, -10000.f, 0.f);
	for(uint j = 0; j < ec->pos.size(); j++)
	{
		ec->pos[j] = initpos;
		ec->targetPos[j] = ec->pos[j];
		ec->color[j] = glm::vec3(0.2f + std::rand() * multiplier * .8f,
								 0.2f + std::rand() * multiplier * .8f,
								 0.2f + std::rand() * multiplier * .8f);
		hordeunitindex = j * HORDESIZE;
		ec->hordeunits[hordeunitindex].alpha = 1;
		ec->hordeunits[hordeunitindex].size = WISPSIZE;
		ec->hordeunits[hordeunitindex].rot = 0;

		randx = 3 - (float)std::rand() * multiplier * 6;
		randz = 3 - (float)std::rand() * multiplier * 6;
		ec->hordeunitdir1[hordeunitindex] = glm::vec3(randx, 0, randz);

		// every horde is 20 or 10 units
		for(uint i = 1; i < HORDESIZE; i++)
		{
			hordeunitindex = i + j*HORDESIZE;
			randx = 5.f - (float)std::rand() * multiplier * 10;
			randz = 5.f - (float)std::rand() * multiplier * 10;

			ec->hordeunits[hordeunitindex].pos = glm::vec3(randx, 0, randz);
			ec->hordeunits[hordeunitindex].alpha = 1;
			ec->hordeunits[hordeunitindex].rot = 0;
			ec->hordeunits[hordeunitindex].size = WISPSIZE;

			randx = 3.f - (float)std::rand() * multiplier * 6;
			randz = 3.f - (float)std::rand() * multiplier * 6;
			ec->hordeunitdir1[hordeunitindex] = glm::vec3(randx, 0, randz);

			randx = (float)std::rand() / RAND_MAX * (float)TWOPI;
			ec->hordeunitwavevar1[hordeunitindex] = randx;
		}
		
	}
	Particle p;
	UINT temp	= graphics->createVBufferDynamic( ec->pos.size()*HORDESIZE,					sizeof(p) );
	UINT temp2	= graphics->createVBufferDynamic( ec->pos.size()*HORDESIZE*WISPSPARKLES,	sizeof(p) );

	int glitteryAllocationSize = HORDESIZE * WISPSPARKLES;
	for(unsigned int i = 0; i < ec->pos.size(); i++)
	{
		//actual wisps
		ec->vBufferIndex.at(i)			= temp;
		ec->vertexStartIndex.at(i)		= i * HORDESIZE;
		ec->vertexAmount.at(i)			= HORDESIZE;
		//extra particles
		ec->vBufferExtraIndex.at(i)		= temp2;
		ec->vertexExtraStartIndex.at(i) = i * glitteryAllocationSize;
		ec->vertexExtraAmount.at(i)		= glitteryAllocationSize;
	}

	wispTech = new ShaderClass("wispVS", "wispGS", "", "", "wispPS", graphics, (int)LayoutTypes::LAYOUT_PARTICLE, false);
	ec->textureIndex.at(0) = graphics->getHandleIndex("wisp", ResourceType::ShaderResourceView);
	ec->textureIndex.at(1) = graphics->getHandleIndex("lightorb", ResourceType::ShaderResourceView);
	ec->textureIndex.at(2) = ec->textureIndex.at(1);
	ec->textureIndex.at(3) = ec->textureIndex.at(0);
	ec->textureIndex.at(4) = ec->textureIndex.at(0);
	cbWispIndex = graphics->getHandleIndex("cbWisps", ResourceType::ConstantBuffer);
	temp = 0;
	pe->allocate(0, wispAmount * WISPSPERSWARM);
	for(UINT i = 0; i < wispAmount; i++)
		for(UINT k = 0; k < WISPSPERSWARM; k++)
		{
			particleEffects.push_back(ParticleEffect(temp, i, k));
			particleEffects.back().init(graphics, pe, ec, temp*WISPSPARKLES, WISPSPARKLES);
			temp++;
		}

}
float L(float x) { return x > 0 ? x : -x; }
float L(glm::vec3 x) { return glm::dot(x, x); }
void EnemyAspect::runAspect(float _dt)
{
	if(_dt > 200)
		return;
	
	update(_dt);

	for(uint j = 0; j < ec->pos.size(); j++)
		for(uint i = 0; i < HORDESIZE; i++)
			ec->hordeunits[j*HORDESIZE + i].pos += ec->pos[j];

	for(UINT i = 0; i < particleEffects.size(); i++)
		particleEffects.at(i).update(_dt);

	Particle p;
	graphics->updateVBuffer(ec->vBufferIndex.at(0), (void**)&ec->hordeunits[0], ec->hordeunits.size(), sizeof(p));
	graphics->updateVBuffer( ec->vBufferExtraIndex.at(0), (void**)&pe->particleData[0], pe->particleData.size(), sizeof(p));
	for(uint j = 0; j < ec->pos.size();j++)
		for(uint i = 0; i < HORDESIZE; i++)
			ec->hordeunits[j*HORDESIZE + i].pos -= ec->pos[j];

	draw();
}

void EnemyAspect::updateLights()
{
	cbuffers->wispLightCount = ec->pos.size();
	UINT istart = cbuffers->energyLightCount;
	UINT index = 0;
	glm::vec4 pos;
	for (UINT i = istart; i < istart + cbuffers->wispLightCount; i++)
	{
		pos = glm::vec4(ec->pos.at(index), 1.0f);
		pos.y = ec->hordeunits.at(index*HORDESIZE).pos.y;
		cbuffers->cbLight.pos[i] = pos;
		cbuffers->cbLight.color[i] = glm::vec4(ec->color.at(index),1);
		cbuffers->cbLight.reach[i].x = 400;
		index++;
	}
}

void EnemyAspect::update( float _dt)
{
	int currentunitindex, 
		othersunitindex;

	float	distance, 
			alignment_count, 
			separation_count,
			randx,
			randy = 0,
			randz;

	float multiplier = 1.f / RAND_MAX;
	glm::vec3 alignment, separation, t;

	float dt = _dt / 1000.f;
	// horde behaviour
	for(uint j = 0; j < ec->pos.size(); j++)
	{
		// every horde is 20 or 10 units
		for(uint i = 0; i < HORDESIZE; i++)
		{
			currentunitindex = j * HORDESIZE + i;
			// Alignment
			alignment = glm::vec3(0.f, 0.f, 0.f);
			alignment_count = 0;
			// Separation
			separation = glm::vec3(0.f, 0.f, 0.f);
			separation_count = 0;

			for(uint l = 0; l < HORDESIZE; l++)
			{
				if(i == l)
					continue;
				othersunitindex = j*HORDESIZE + l;
				distance = glm::distance(ec->hordeunits[currentunitindex].pos, ec->hordeunits[othersunitindex].pos);
				// Alignment
				if(distance < 60.f)
				{
					alignment += ec->hordeunitdir1[othersunitindex];
					alignment_count++;
				}
				if(distance < 5.f)
				{
					separation += ec->hordeunits[othersunitindex].pos - ec->hordeunits[currentunitindex].pos;
					separation_count++;
				}
			}
			// Alignment
			if(alignment_count)
				ec->hordeunitdir1[currentunitindex] += glm::normalize(alignment / alignment_count) * 0.005f;

			// Cohesion
			t = ec->hordeunits[j*HORDESIZE].pos;
			t = t - ec->hordeunits[currentunitindex].pos;
			if(L(t) > 0.001f)
			{
				t = glm::normalize(t);
				ec->hordeunitdir1[currentunitindex] += t * 0.005f;
			}

			// Separation
			if(separation_count)
			{
				t = separation * -1.f / separation_count;
				if(L(t) > 0.001f)
				{
					t = glm::normalize(t);
					ec->hordeunitdir1[currentunitindex] += t * 0.05f;
				}
			}

			randx = 3 - (float)std::rand() * multiplier * 6.f;
			randz = 3 - (float)std::rand() * multiplier * 6.f;
			ec->hordeunitdir1[currentunitindex].y = 0;
			ec->hordeunitdir1[currentunitindex] = glm::normalize(ec->hordeunitdir1[currentunitindex] + glm::vec3(randx, randy, randz) * 0.01f) * 9.f * dt;

			ec->hordeunits[currentunitindex].pos += ec->hordeunitdir1[currentunitindex];
			ec->hordeunitwavevar1[currentunitindex] += dt;
			if(ec->hordeunitwavevar1[currentunitindex] > (float)TWOPI)
				ec->hordeunitwavevar1[currentunitindex] -= (float)TWOPI;
		}
		for(uint i = 0; i < HORDESIZE; i++)
		{
			ec->hordeunits[j*HORDESIZE + i].pos -= ec->hordeunits[HORDESIZE*j].pos;
		}
	}
	followTerrain(_dt);
	updateLights();
}

void EnemyAspect::draw()
{
	//-----------------------------------------------
	//					wisps
	//-----------------------------------------------

	UINT strides = sizeof(Particle);
	UINT offsets = 0;

	//graphics->setViewport( w, h );
	//graphics->setRenderTargetViews(0, 1, -1);
	UINT temp = graphics->getHandleIndex("postEffectRenderTarget",ResourceType::RenderTargetView);
	graphics->setTopology(1);
	graphics->setRenderTargetViews(temp, 1, -1);
	graphics->setDepthStencilState(2);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_PARTICLE);
	wispTech->use();
	graphics->setBlendState(2);
	graphics->setRasterState(2);
	graphics->setCBuffers(5, 1, cbWispIndex);
	graphics->setPixelShaderResource(4, graphics->getDepthResourceView(), 1);
	//extra particles
	CBWisp wosp;
	graphics->setPixelShaderResource(1, ec->textureIndex.at(1), 1);
	graphics->setVertexBuffers(0, ec->vBufferExtraIndex.at(0), 1, &strides, &offsets);
	wosp.color = glm::vec4(ec->color.at(0), 1);
	wosp.pos = glm::vec4(0, 0, 0, 1);
	graphics->updateCBuffer(cbWispIndex, &wosp);
	graphics->draw(ec->pos.size() * HORDESIZE * WISPSPARKLES, 0);
	//wisps
	graphics->setVertexBuffers(0, ec->vBufferIndex.at(0), 1, &strides, &offsets);
	graphics->setPixelShaderResource(1, ec->textureIndex.at(0), 1);
	graphics->draw(ec->pos.size() * HORDESIZE, 0);
	graphics->setPixelShaderResource(1, -1, 1);
	graphics->setPixelShaderResource(4, -1, 1);
}

#include "Utility.h"
void EnemyAspect::followTerrain(float _dt)
{
	TerrainContainer &terrain(*c.get<TerrainContainer>(Containers::TERRAIN));
	int index = 0;
	float height;
	glm::vec2 tempPos;
	for(uint j = 0; j < ec->pos.size(); j++)
	{
		for(uint k = 0; k < HORDESIZE; k++)
		{
			index = j*HORDESIZE + k;
			tempPos = glm::vec2(ec->pos[j].x + ec->hordeunits[index].pos.x,
								ec->pos[j].z + ec->hordeunits[index].pos.z);

			height = Utility::mapToTerrain(tempPos, terrain).y;
			height = std::max(50.f, height);
			ec->hordeunits[index].pos.y = height + 10 + std::sin(ec->hordeunitwavevar1[index] * 3) * 2;
		}
	}
}