#include "EnvironmentAspect.h"
#include "ParticleAspect.h"
#include "MainState.h"
#include "PostEffectsAspect.h"
RenderInterface* EnvironmentAspect::graphics = nullptr;

EnvironmentAspect::EnvironmentAspect()
: AspectInterface("World", 100),
containers(Containers(std::make_pair(Containers::PLAYER, READ), std::make_pair(Containers::TERRAIN, MODIFY))),
playerContainer(*containers.get<PlayerContainer>(Containers::PLAYER)),
terrainContainer(*containers.get<TerrainContainer>(Containers::TERRAIN)),
range(0, 1)
{}

std::pair< std::pair<char, char>, std::pair<float, float> > biomMapping(BiomsContainer& _biom, glm::vec2 _position);

void EnvironmentAspect::init(MainState& _mainState)
{
	graphics = _mainState.graphics;

	Containers c = Containers(std::make_pair(Containers::WIND, MODIFY), std::make_pair(Containers::BIOMS, MODIFY), std::make_pair(Containers::SANDSTORM, MODIFY));
	c.get<SandStormContainer>(Containers::SANDSTORM)->insideSandstorm = false;

	// Initialize wind container
	WindContainer& wind(*c.get<WindContainer>(Containers::WIND));
	wind.globalWindDirection = glm::vec2(-1, 0);
	wind.globalWindForce = 5.f;

	wind.cellSize = 5.f;
	wind.mapPosition = glm::vec2(playerContainer.position.x - (wind.mapSize / 2) * wind.cellSize, playerContainer.position.z - (wind.mapSize / 2) * wind.cellSize);

	glm::vec3 noll = glm::vec3(0.f, 0.f, 0.f);
	for(auto& a : wind.windMap)
		for(auto& f : a)
			f = noll;

	// Initialize detail layer
	//InitializeDetailLayer();

	generateGrassBuffer();

	for(int j = 0; j < 9; j++)
	{
		terrainContainer.grassTextureIDs[j] = graphics->createTexture2D(256, 256, (void**)&terrainContainer.heights[terrainContainer.tiny[j].heightsID][0]);
		terrainContainer.tiny[j].heightmapShaderResource = graphics->createShaderResourceViewTest(terrainContainer.grassTextureIDs[j]);
		//should be unique and possibly generated?
		terrainContainer.tiny[j].grassBlendShaderResource = graphics->createShaderResourceViewFromFile("Textures/grassblend2.png");
	}
	wind.windMapTexture = graphics->createTexture2D32(128,128,NULL);
	wind.windMapShaderResource = graphics->createShaderResourceView32(wind.windMapTexture);
	// Initialize clouds
	BiomsContainer& biom(*c.get<BiomsContainer>(Containers::BIOMS));

	for (auto& a : biom.bufferCloudMap)
		for (float& f : a)
			f = 0;

	// Add random generated positions for clouds
	std::uniform_real_distribution<float> rangePosition(-1,1);
	for (uint i(0); i < 10; i++)
	{
		biom.clouds[i].position = glm::vec2(rangePosition(numberGenerator), rangePosition(numberGenerator));
		biom.clouds[i].radius = 2000 + range(numberGenerator) * 10000;
		biom.clouds[i].strenght = 0.2f + range(numberGenerator) * 0.5f;
	}

	std::array< std::array< float, BiomsContainer::cloudSize >, BiomsContainer::cloudSize > randomMap;
	for (uint x(0); x < randomMap.size(); x++)
	{
		for (uint y(0); y < randomMap[x].size(); y++)
		{
			randomMap[x][y] = range(numberGenerator);
			biom.baseCloudMap[x][y] = 0;
		}
	}
	Utility::perlinNoiseClouds(biom.baseCloudMap, randomMap, biom.baseCloudMap.size(), biom.baseCloudMap[0].size(), 2, 32, 0.5f);

	//biom.desertStorm.direction = glm::vec2(-1, 0);
	//biom.desertStorm.position.x = playerContainer.position.x + 10000;
	//biom.desertStorm.size = 2000;
	//biom.desertStorm.speed = 100;

	c.get<SandStormContainer>(Containers::SANDSTORM)->insideSandstorm = false;
}

void EnvironmentAspect::generateGrassBuffer()
{
	terrainContainer.grassGeometryLOD0 = new ShaderClass("grassGeometryVS", "grassGeometryLOD0GS", "", "", "grassGeometryPS", graphics, (int)LayoutTypes::LAYOUT_GRASS, false);
	terrainContainer.grassGeometryLOD1 = new ShaderClass("grassGeometryVS", "grassGeometryLOD1GS", "", "", "grassGeometryPS", graphics, (int)LayoutTypes::LAYOUT_GRASS, false);
	terrainContainer.grassGeometryLOD2 = new ShaderClass("grassGeometryVS", "grassGeometryLOD2GS", "", "", "grassGeometryPS", graphics, (int)LayoutTypes::LAYOUT_GRASS, false);
	//terrainContainer.grassGeometryLOD3 = new ShaderClass("grassGeometryVS", "grassGeometryLOD3GS", "", "", "grassGeometryPS", graphics, (int)LayoutTypes::LAYOUT_GRASS, false);
	terrainContainer.grassTextureID = graphics->createShaderResourceViewFromFile("Textures/grasskd.png");

	terrainContainer.swampGrassGeometryLOD0 = new ShaderClass("grassGeometryVS", "SwampgrassGeometryLOD0GS", "", "", "SwampgrassGeometryPS", graphics, (int)LayoutTypes::LAYOUT_GRASS, false);
	terrainContainer.swampGrassGeometryLOD1 = new ShaderClass("grassGeometryVS", "SwampgrassGeometryLOD1GS", "", "", "SwampgrassGeometryPS", graphics, (int)LayoutTypes::LAYOUT_GRASS, false);
	terrainContainer.swampGrassGeometryLOD2 = new ShaderClass("grassGeometryVS", "SwampgrassGeometryLOD2GS", "", "", "SwampgrassGeometryPS", graphics, (int)LayoutTypes::LAYOUT_GRASS, false);

	uint grassPatchSize = 175;
	terrainContainer.grassPatchTileSize = 600;
	std::vector<grassVertex> initData = std::vector<grassVertex>(grassPatchSize * grassPatchSize);
	float multiplier = (float)terrainContainer.grassPatchTileSize / RAND_MAX;
	for(uint i = 0; i < grassPatchSize * grassPatchSize; i++)
	{
		initData.at(i).pos = glm::vec3((float)rand() * multiplier, 0, (float)rand() * multiplier);
	}
	terrainContainer.grassBufferID = graphics->createVBufferImmutable((void**)&initData[0], initData.size(), sizeof(initData[0]));

	// create thew buffer that will contain the perinstance data, it will get grassbufferID + 1 as long as we dont thread xD
	// should perhaps be one of these per lod. one drawcall per lod. and make them dynamic and put chunks of grass in here depending
	// on which lod they should be in per frame. view frustum culling should also be added.
	std::vector<grassVertexPerInstance> initDataInst = std::vector<grassVertexPerInstance>(25);
	terrainContainer.grassBufferPerInstance = graphics->createVBufferDynamic(initDataInst.size(), sizeof(initDataInst[0]));
}

template < class T, class Y >
void EnvironmentAspect::GenerateSandMap(T& _map, Y& _randomMap1, Y& _randomMap2, float _offset)
{
	auto interpolate = [](float a, float b, float x)->float
	{
		return a*(1 - x) + b*x;
	};
	numberGenerator.seed(1000);
	std::array< std::array< float, DetailLayerContainer::sizeY >, DetailLayerContainer::sizeX > randomMap;
	for(uint x(0); x < DetailLayerContainer::sizeX; x++)
	{
		for(uint y(0); y < DetailLayerContainer::sizeY; y++)
		{
			if(_offset * 2 < 1.f)
				randomMap[x][y] = interpolate(_randomMap1[x][y], _randomMap2[x][y], _offset * 2);
			else
				randomMap[x][y] = interpolate(_randomMap2[x][y], _randomMap1[x][y], _offset * 2 - 1);
		}
	}
	Utility::perlinNoiseSeamless(_map, randomMap, DetailLayerContainer::sizeX, DetailLayerContainer::sizeY, 32, 128, 1); // 16 64
	float period = 48.f;
	for(uint x(0); x < DetailLayerContainer::sizeX; x++)
	{
		for(uint y(0); y < DetailLayerContainer::sizeY; y++)
		{
			_map[x][y].w = (sinf((_map[x][y].w + float(x) * period / DetailLayerContainer::sizeX + _offset * 2) * float(std::_Pi)) + 1) / 2;
		}
	}
	// Generate normal map part of detail map
	auto foo = [](float x){ return ((std::sin(x * std::_Pi / 64) + 1) / 2); };
	float t;
	glm::vec3 t1;
	glm::vec3 t2;
	glm::vec3 t3;
	for(uint x(0); x < _map.size() - 1; x++)
	{
		for(uint y(0); y < _map[x].size() - 1; y++)
		{
			t = .7f; //.5f
			t1 = glm::vec3(t, _map[x + 1][y].w - _map[x][y].w, 0);
			t2 = glm::vec3(0, _map[x][y].w - _map[x][y + 1].w, t);
			t3 = glm::normalize(glm::cross(t1, t2));
			float f = t3.x;
			t3.x = t3.z;
			t3.z = f;

			_map[x][y] = glm::vec4(t3, _map[x][y].w / 3); // / 2
		}
	}
	for(uint x(0); x < _map.size(); x++)
	{
		_map[x][_map.size() - 1] = _map[x][0];
	}
	for(uint y(0); y < _map[0].size(); y++)
	{
		_map[_map[0].size() - 1][y] = _map[0][y];
	}
}

void EnvironmentAspect::UpdateSand(float _dt)
{
	DetailLayerContainer& detailLayer = *Containers(std::make_pair(Containers::DETAIL_LAYER, MODIFY)).get<DetailLayerContainer>(Containers::DETAIL_LAYER);
	auto& map(detailLayer.detailMap);
	auto interpolate = [](float a, float b, float x)->float
	{
		return a*(1 - x) + b*x;
	};
	static float dt = 0;
	dt += _dt;
	static uint current = 0;
	float limit = 3000.f;
	auto wrap = [&detailLayer](uint _i){ return _i == detailLayer.sandMap.size() ? 0 : _i; };
	if (dt > limit)
	{
		dt = 0.f;
		current = wrap(current + 1);
	}
	for (uint x(0); x < DetailLayerContainer::sizeX; x++)
		for (uint y(0); y < DetailLayerContainer::sizeY; y++)
		{
			map[x][y].x = interpolate(detailLayer.sandMap[current][x][y].x, detailLayer.sandMap[wrap(current + 1)][x][y].x, dt / limit);
			map[x][y].y = interpolate(detailLayer.sandMap[current][x][y].y, detailLayer.sandMap[wrap(current + 1)][x][y].y, dt / limit);
			map[x][y].z = interpolate(detailLayer.sandMap[current][x][y].z, detailLayer.sandMap[wrap(current + 1)][x][y].z, dt / limit);
			map[x][y].w = interpolate(detailLayer.sandMap[current][x][y].w, detailLayer.sandMap[wrap(current + 1)][x][y].w, dt / limit);
		}


	graphics->updateTexture2D(detailLayer.textureIndex, 256, 256, (void**)&detailLayer.detailMap[0][0], sizeof(float)* 4);
}

void EnvironmentAspect::InitializeDetailLayer()
{
	DetailLayerContainer& detailLayer = *Containers(std::make_pair(Containers::DETAIL_LAYER, MODIFY)).get<DetailLayerContainer>(Containers::DETAIL_LAYER);
	numberGenerator.seed(1000);
	std::array< std::array< float, DetailLayerContainer::sizeY >, DetailLayerContainer::sizeX > randomMap1, randomMap2;
	for (uint x(0); x < DetailLayerContainer::sizeX; x++)
	{
		for (uint y(0); y < DetailLayerContainer::sizeY; y++)
		{
			randomMap1[x][y] = range(numberGenerator);
			randomMap2[x][y] = range(numberGenerator);
		}
	}
	for (uint i(0); i < detailLayer.sandMap.size(); i++)
		GenerateSandMap(detailLayer.sandMap[i], randomMap1, randomMap2, (float(i) / detailLayer.sandMap.size()));
	UpdateSand(0.f);
	detailLayer.textureIndex = graphics->createTexture2D32(256, 256, (void**)&detailLayer.detailMap[0][0]);
	detailLayer.shaderResourceIndex = graphics->createShaderResourceView32(detailLayer.textureIndex);
}

void EnvironmentAspect::UpdateLocalWind(Containers& _c, float _dt)
{
	WindContainer& wind(*_c.get<WindContainer>(Containers::WIND));
	DetailLayerContainer& detailLayer(*_c.get<DetailLayerContainer>(Containers::DETAIL_LAYER));
	PlayerContainer& player(*_c.get<PlayerContainer>(Containers::PLAYER));

	// Check if the wind map should be moved
	//float distanceX = glm::length(player.position.x - wind.mapPosition.x);
	//float distanceY = glm::length(player.position.z - wind.mapPosition.y);
	//float limit = 100.f;

	//auto moveMap = [&wind](uint _xFrom, uint _xTo, uint _yFrom, uint _yTo, int _xOffset, int _yOffset)
	//{
	//	for (uint x(_xFrom); x < _xTo; x++)
	//	{
	//		for (uint y(_yFrom); y < _yTo; y++)
	//		{
	//			wind.windMap[x + _xOffset][y + _yOffset] = wind.windMap[x][y];
	//		}
	//	}
	//};
	//auto zeroMap = [&wind](uint _xFrom, uint _xTo, uint _yFrom, uint _yTo)
	//{
	//	for (uint x(_xFrom); x < _xTo; x++)
	//	{
	//		for (uint y(_yFrom); y < _yTo; y++)
	//		{
	//			wind.windMap[x][y] = glm::vec3(0.f, 0.f, 0.f);
	//		}
	//	}
	//};
	//int t = uint(limit / wind.cellSize);
	//uint size = wind.windMap.size();
	//if (distanceX > limit)
	//{
	//	wind.mapPosition.x += limit;
	//	moveMap(t, size, 0, size, -t, 0);
	//	zeroMap(0, t, 0, size);
	//}
	//else if (distanceX < -limit)
	//{
	//	wind.mapPosition.x -= limit;
	//	moveMap(0, size-t, 0, size, t, 0);
	//	zeroMap(size-t, size, 0, size);
	//}
	//if (distanceY > limit)
	//{
	//	wind.mapPosition.y += limit;
	//	moveMap(0, size, t, size, 0, -t);
	//	zeroMap(0, size, 0, t);
	//}
	//else if (distanceY < -limit)
	//{
	//	wind.mapPosition.y -= limit;
	//	moveMap(0, size, 0, size-t, 0, t);
	//	zeroMap(0, size, size-t, size);
	//}



	// Generate gust of winds
	static std::mt19937 numberGenerator;
	static std::uniform_int_distribution<> range(0, WindContainer::mapSize - 1);
	static std::mt19937 numberGenerator2;
	static std::uniform_real_distribution<float> range2(-1, 1);

	for (uint i(0); i < 20; i++)
		wind.windMap[range(numberGenerator)][range(numberGenerator)] = glm::vec3(glm::normalize(glm::vec2(range2(numberGenerator2), range2(numberGenerator2))), 2.f);

	// Update wind positions
	auto& map(wind.windMap);
	auto move = [](glm::vec3& _f, glm::vec3& _t, float dt)
	{
		if (_f.z < 0.0001f)
		{ 
			_f.z = 0;
			return;
		}
		if (dt < 0.001f || dt > 1.0f)
			return;
		float f = _f.z * dt * 0.1f; // /4
		float i = f / (f + _t.z);
		_t.x += _f.x * i;
		_t.y += _f.y * i;
		glm::vec2 t = glm::normalize(glm::vec2(_t.x, _t.y));
		_t.x = t.x;
		_t.y = t.y;
		_t.z += f;
		_f.z -= f * 1.2f;
	};
	for (uint x(1); x < map.size() - 1; x++)
	{
		for (uint y(1); y < map[x].size() - 1; y++)
		{
			move(map[x][y], map[x + 0][y + 1], _dt);
			move(map[x][y], map[x + 0][y - 1], _dt);
			move(map[x][y], map[x + 1][y + 0], _dt);
			move(map[x][y], map[x - 1][y + 0], _dt);
			//			spawnParticles(t, glm::vec2( x * wind.cellSize, y * wind.cellSize ) + wind.mapPosition, glm::vec2(1,0));
		}
	}
	//update shaderresource here!:
	graphics->updateTexture2D(wind.windMapTexture,128,128, (void**)&wind.windMap[0],sizeof(float)*3);
}

void EnvironmentAspect::UpdateGlobalWind(Containers& _c)
{
	WindContainer& wind(*_c.get<WindContainer>(Containers::WIND));
	DetailLayerContainer& detailLayer(*_c.get<DetailLayerContainer>(Containers::DETAIL_LAYER));
	PlayerContainer& player(*_c.get<PlayerContainer>(Containers::PLAYER));

	static std::mt19937 numberGenerator;
	static std::uniform_real_distribution<float> range1(-100,100);
	static std::uniform_real_distribution<float> range2(-500,500);
	static std::uniform_real_distribution<float>* range;

	if (_c.get<SandStormContainer>(Containers::SANDSTORM)->insideSandstorm)
		range = &range1;
	else
		range = &range2;

	// Spawn particles depending on global wind and terrain
	if (wind.windParticlesQue.size() < 100)
		wind.windParticlesQue.push_back(WindContainer::Particle(
			Utility::mapToTerrain(glm::vec2(player.position.x + (*range)(numberGenerator), player.position.z + (*range)(numberGenerator)), terrainContainer),
			glm::vec3(wind.globalWindDirection.x, .2f, wind.globalWindDirection.y) * wind.globalWindForce));
}

void EnvironmentAspect::UpdateClouds(Containers& _c, float _dt)
{
	PlayerContainer& player(*_c.get<PlayerContainer>(Containers::PLAYER));
	BiomsContainer& biom(*_c.get<BiomsContainer>(Containers::BIOMS));
	WindContainer& wind(*_c.get<WindContainer>(Containers::WIND));

	// Move cloudsMap position slightly depending on global wind
	biom.cloudMapPosition += wind.globalWindDirection * wind.globalWindForce * _dt * 1.f;

	// Check if the player or cloudsMap have moved far enough to update the cloudsMap
	// If they have, move (depending on positions) parts of the map to the front/back
	float cellSize = CLOUD_CELLSIZE;
	float distanceX = (player.position.x - biom.cloudMapPosition.x - biom.cloudSize * cellSize / 2);
	float distanceY = (player.position.z - biom.cloudMapPosition.y - biom.cloudSize * cellSize / 2);

	float limit = cellSize;
	uint size = biom.cloudMap.size();
	if(distanceX > limit || distanceX < -limit)
	{
		biom.cloudMapPosition.x += int(distanceX / limit) * limit;
		for (uint y(0); y < size; y++)
		{
			std::swap(biom.baseCloudMap[0][y], biom.baseCloudMap[size - 1][y]);
		}
	}
	if(distanceY > limit || distanceY < -limit)
	{
		biom.cloudMapPosition.y += int(distanceY / limit) * limit;
		for (uint x(0); x < size; x++)
		{
			std::swap(biom.baseCloudMap[x][0], biom.baseCloudMap[x][size - 1]);
		}
	}

	// Move clouds position sligthly depending on global wind
	// then Check if the are to far away, if so move them
	glm::vec2 t;
	for (auto& c : biom.clouds)
	{
		c.position += wind.globalWindDirection * wind.globalWindForce * _dt * 0.1f;
		t = glm::vec2(player.position.x, player.position.z) - c.position;
		if (glm::length(t) > cellSize * 150)
			c.position -= t * 1.5f;
	}

	// Update cloudsMap depending on cloud positions, radius and strength
	float value, t0;
	for (uint x(0); x < size; x++)
	{
		for (uint y(0); y < size; y++)
		{
			value = 0;
			for (uint i(0); i < biom.clouds.size(); i++)
			{
				t0 = glm::length(biom.cloudMapPosition + glm::vec2(x, y)*cellSize - biom.clouds[i].position);
				if (t0 < biom.clouds[i].radius)
				{
					t0 = biom.clouds[i].strenght;
					if (t0 > value)
					{
						value = biom.clouds[i].strenght;
					}
				}
			}
			biom.cloudMap[x][y] = value;
		}
	}

	// Update bufferCloudMap
	for (uint x(0); x < size; x++)
	{
		for (uint y(0); y < size; y++)
		{
			t0 = biom.baseCloudMap[x][y] * biom.cloudMap[x][y];
			if(t0 < 0.1)
				t0 = 0;
			biom.bufferCloudMap[x][y] = t0;
		}
	}
}

void EnvironmentAspect::UpdateStorm(Containers& _c, float _dt, float _desert)
{
	BiomsContainer& biom(*_c.get<BiomsContainer>(Containers::BIOMS));
	PlayerContainer& player(*_c.get<PlayerContainer>(Containers::PLAYER));
	SandStormContainer& sandStormContainer(*_c.get<SandStormContainer>(Containers::SANDSTORM));

	auto start = [&sandStormContainer](glm::vec3 _position)
	{
		sandStormContainer.emitterPositions.at(0) = _position;
		sandStormContainer.firstTime.at(0) = true;
//		sandStormContainer.particleData.clear();
		CloudParticle pData;
		pData.pos = sandStormContainer.emitterPositions.at(0);
		pData.age = 5.f;
		pData.rot = 7.f;
		pData.size = glm::vec2(250, 250);
		pData.type = 0;
		sandStormContainer.particleData.at(0) = pData;
		graphics->updateVBuffer(sandStormContainer.bufferIndex, (void**)&sandStormContainer.particleData[0], sandStormContainer.particleData.size(), sizeof(CloudParticle));
	};
	auto end = [&sandStormContainer]()
	{
//		sandStormContainer.particleData.clear();
		CloudParticle pData;
		pData.pos = sandStormContainer.emitterPositions.at(0);
		pData.age = 5.f;
		pData.rot = 7.f;
		pData.size = glm::vec2(0, 0);
		pData.type = 1;
		sandStormContainer.particleData.at(0) = pData;
		graphics->updateVBuffer(sandStormContainer.bufferIndex, (void**)&sandStormContainer.particleData[0], sandStormContainer.particleData.size(), sizeof(CloudParticle));
	};

	biom.rainStorm.position += biom.rainStorm.direction * biom.rainStorm.speed * _dt;
	biom.desertStorm.position.x += biom.desertStorm.direction.x * biom.desertStorm.speed * _dt;
	sandStormContainer.emitterPositions.at(0) = glm::vec3(biom.desertStorm.position.x, 100, player.position.z);

	if (_desert != 1)
		return;

	float answer = biom.desertStorm.position.x - player.position.x;

	enum { nothing, incomming, inside, movingAway, farAway} static state = nothing;

	float limit = 30000;
	if (state != incomming && answer > 0 && answer < limit)
	{
		state = incomming;
		// Start sandstorm: incomming
		sandStormContainer.insideSandstorm = false;
		start(glm::vec3(biom.desertStorm.position.x, 100, player.position.z));
	}
	else if (state != inside && answer > -biom.desertStorm.size && answer < 0)
	{
		state = inside;
		// Inside sandstorm, remove sandstorm
		sandStormContainer.insideSandstorm = true;
		end();
	}
	else if (state != movingAway && answer > -limit - biom.desertStorm.size && answer < -biom.desertStorm.size)
	{
		state = movingAway;
		// Start sandstorm: moving away
		sandStormContainer.insideSandstorm = false;
		start(glm::vec3(biom.desertStorm.position.x + biom.desertStorm.size + 1000, 100, player.position.z));
	}
	else if (state != farAway && answer < -limit - biom.desertStorm.size)
	{
		state = farAway;
		// Sandstorm to far away, remove sandstorm
		sandStormContainer.insideSandstorm = false;

		end();
	}
}

static float dt = 0;
void EnvironmentAspect::runAspect(float _dt)
{
	Containers c = Containers(
		std::make_pair(Containers::WIND, MODIFY),
		std::make_pair(Containers::TERRAIN, READ),
		std::make_pair(Containers::PLAYER, READ),
		std::make_pair(Containers::BIOMS, MODIFY),
		std::make_pair(Containers::SANDSTORM, MODIFY),
		std::make_pair(Containers::DETAIL_LAYER, MODIFY));

	//static bool a = false;
	//if (!a)
	//{
	//	a = true;
	//	SandStormContainer& sandStormContainer = *c.get<SandStormContainer>(Containers::SANDSTORM);
	//	sandStormContainer.emitterPositions.at(0) = glm::vec3(70000,100,80000);
	//	sandStormContainer.firstTime.at(0) = true;
	//	sandStormContainer.particleData.clear();
	//	CloudParticle pData;
	//	pData.pos = sandStormContainer.emitterPositions.at(0);
	//	pData.age = 5.f;
	//	pData.rot = 7.f;
	//	pData.size = glm::vec2(250, 250);
	//	pData.type = 0;
	//	sandStormContainer.particleData.push_back(pData);
	//	graphics->updateVBuffer(sandStormContainer.bufferIndex, (void**)&sandStormContainer.particleData[0], sandStormContainer.particleData.size(), sizeof(CloudParticle));
	//}

	PlayerContainer& player = *c.get<PlayerContainer>(Containers::PLAYER);
	BiomsContainer& bioms = *c.get<BiomsContainer>(Containers::BIOMS);
	SandStormContainer &storm(*c.get<SandStormContainer>(Containers::SANDSTORM));
	SingleBiom& b = Utility::getBlendMapValue(&bioms, player.position);

	UpdateStorm(c, _dt / 1000, b.map1.r);

	dt += _dt;
	if (dt < 50)
		return;

#ifndef _DEBUG
	if (b.map1.r > 0.001)
		UpdateSand(dt);
#endif
	//if (b.map1.b > 0.001)
	//	UpdateLocalWind(c, dt / 1000);
	if (!storm.isMultiPlayer && b.map1.r > 0.001)
		UpdateGlobalWind(c);
	//UpdateClouds(c, dt / 1000);

	dt = 0;
}

EnvironmentAspect::~EnvironmentAspect()
{
}
