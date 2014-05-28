#include "TerrainAspect.h"
#include "MainState.h"
#include <time.h>

RenderInterface* TerrainAspect::graphics = nullptr;

TerrainAspect::TerrainAspect()
: AspectInterface("Terrain", 100),
containers(Containers(std::make_pair(Containers::PLAYER, READ), std::make_pair(Containers::TERRAIN, MODIFY),std::make_pair(Containers::CONSTBUFFER, MODIFY), std::make_pair(Containers::BIOMS, MODIFY))),
playerContainer(*containers.get<PlayerContainer>(Containers::PLAYER)),
terrainContainer(*containers.get<TerrainContainer>(Containers::TERRAIN)),
cbuffers(*containers.get<ConstantBufferContainer>(Containers::CONSTBUFFER)),
range(0, 1)
{}

std::pair< std::pair<char, char>, std::pair<float, float> > biomMapping(BiomsContainer& _biom, glm::vec2 _position);

void TerrainAspect::init(MainState& _mainState)
{
	terrainContainer.worldSeed = 0;

	graphics = _mainState.graphics;
	terrainContainer.cbTerrInfo = graphics->getHandleIndex("cbTerrain",ResourceType::ConstantBuffer);
	terrainContainer.cbShifting = graphics->getHandleIndex("cbShifting", ResourceType::ConstantBuffer);

	CBBiomShift cb;
	cb.interpVal = 0.f;
	graphics->updateCBuffer(terrainContainer.cbShifting, &cb);

	terrainContainer.bufferID = -1;
	terrainContainer.biomMapTextureID1 = terrainContainer.biomMapTextureID2 = -1;
	tempNVerts = 0;

	buffer = std::vector<DefaultVertex>();
	std::vector<int>();
	terrainContainer.biomShifting = chunkGenerationLock = false;

	shiftingInProgress = false;
	elapsedTime = shiftingUpdateFreq = 0.f;

	// Initialize TerrainContainer
	terrainContainer.tinySize = (terrainContainer.tiny[0].size_x - 1) * (float)QUADSIZE;
	terrainContainer.mediumSize = terrainContainer.tinySize * 3;
	terrainContainer.largeSize = terrainContainer.mediumSize * 3;

	terrainContainer.nVertices = 0;
	terrainContainer.nVerticesPerTiny = (terrainContainer.tiny.at(0).size_x - 1) * (terrainContainer.tiny.at(0).size_y - 1) * 6;

	Containers c = Containers(std::make_pair(Containers::WIND, MODIFY));

	glm::vec3 playerPos = playerContainer.position;
	
	playerContainer.smallTerrainID = 4;
	playerContainer.position = playerPos;

	createTerrainFromPosition(glm::vec2(playerPos.x, playerPos.z));
	createTinyFromPosition(glm::vec2(playerPos.x, playerPos.z));

	graphics->addHandle("12", ResourceType::ShaderResourceView, terrainContainer.biomMapID1);
	graphics->addHandle("13", ResourceType::ShaderResourceView, terrainContainer.biomMapID2);

	calculateCullingFromSpawn(glm::vec2(playerPos.x, playerPos.z));

	createTerrainBuffer();

	playerChunkID = 4;
}

void TerrainAspect::updateCullingNodes()
{
	for(uint i = 0; i < terrainContainer.tinyActive.size(); i++)
		terrainContainer.smallNodes.at(0).children.at(i)->addChunk(&terrainContainer.tinyActive.at(i));

	for(uint i = 0; i < terrainContainer.mediumActive.size(); i++)
		terrainContainer.mediumNodes.at(0).children.at(i)->addChunk(&terrainContainer.mediumActive.at(i));

	for(uint i = 0; i < terrainContainer.largeActive.size(); i++)
		terrainContainer.largeNodes.at(0).children.at(i)->addChunk(&terrainContainer.largeActive.at(i));
}

void TerrainAspect::calculateBufferOffsets()
{
	int baseVBufferSize = terrainContainer.tiny[0].size;
	int baseIBufferSize = (terrainContainer.tiny[0].size_x - 1) * (terrainContainer.tiny[0].size_y - 1) * 6;
	for(int i = 0; i < 9; i++)
	{
		terrainContainer.tiny[i].bufferStartID = i * baseVBufferSize;
		terrainContainer.tiny[i].iBufferStartID = i * baseIBufferSize;
		terrainContainer.tiny[i].bufferAmount = baseVBufferSize;
		terrainContainer.tiny[i].iBufferAmount = baseIBufferSize;

		terrainContainer.medium[i].bufferStartID = (i + 9) * baseVBufferSize;
		terrainContainer.medium[i].iBufferStartID = (i + 9) * baseIBufferSize;
		terrainContainer.medium[i].bufferAmount = baseVBufferSize;
		terrainContainer.medium[i].iBufferAmount = baseIBufferSize;

		terrainContainer.large[i].bufferStartID = (i + 18) * baseVBufferSize;
		terrainContainer.large[i].iBufferStartID = (i + 18) * baseIBufferSize;
		terrainContainer.large[i].bufferAmount = baseVBufferSize;
		terrainContainer.large[i].iBufferAmount = baseIBufferSize;
	}

	buffer.resize(baseVBufferSize * 27);
	indexBuffer.resize(baseIBufferSize * 27);

	tempNVerts = indexBuffer.size();
}

std::vector<bool> steps = std::vector<bool>(5, false);
void TerrainAspect::runAspect(float _dt)
{
	_dt *= 0.001f;

	// Amagerd, need to create new chunks!
	if(playerContainer.smallTerrainID != 4 && !chunkGenerationLock)
	{
		//if(reCreateChunkThread.joinable())
		//	reCreateChunkThread.join();

		//reCreateChunkThread = std::thread(&TerrainAspect::recreateChunks, this);
		//chunkGenerationLock = true;
		//recreateChunks();
		setHeightsID(&terrainContainer.tiny, terrainContainer.tinySize);
		
		steps.at(0) = true;
		//chunkGenerationLock = false;
	}
	else if(steps.at(0))
	{
		steps.at(0) = false;
		steps.at(1) = true;

		for(int j = 0; j < 9; j++)
			graphics->updateTexture2D(terrainContainer.grassTextureIDs[j], 256, 256, (void**)&terrainContainer.heights[terrainContainer.tiny[j].heightsID][0], sizeof(float));

		buffer.resize(terrainContainer.tiny[0].size * 27);
		indexBuffer.resize((terrainContainer.tiny[0].size_x - 1) * (terrainContainer.tiny[0].size_y - 1) * 6 * 27);
	
		tempNVerts = indexBuffer.size();
	}
	else if(steps.at(1))
	{
		steps.at(1) = false;
		steps.at(2) = true;

		createChunks(terrainContainer.tiny);
	}
	else if(steps.at(2))
	{
		steps.at(2) = false;
		steps.at(3) = true;
		createChunks(terrainContainer.medium);
	}
	else if(steps.at(3))
	{
		steps.at(3) = false;
		steps.at(4) = true;
		createChunks(terrainContainer.large);
	}
	else if(steps.at(4))
	{
		steps.at(4) = false;
		correctEdges();
		createTerrainBuffer();

		terrainContainer.tinyActive		= terrainContainer.tiny;
		terrainContainer.mediumActive	= terrainContainer.medium;
		terrainContainer.largeActive	= terrainContainer.large;
		playerContainer.playerCullingPos = playerContainer.position;
		updateCullingNodes();
	}

	elapsedTime += _dt;

	if(!terrainContainer.biomShifting && elapsedTime > timeToNextShift)
	{
		terrainContainer.biomShifting = true;
		shiftingDuration = terrainContainer.duration = 10;
		elapsedTime = 0.f;
	}

	if(shiftingInProgress)
	{
		CBBiomShift cb;
		int blendSize = BLENDTEXSIZE * 3 + 1;
		float biomStep = terrainContainer.largeSize * 3 / blendSize;

		glm::vec3 startPos = playerContainer.position - terrainContainer.largeActive[0].position;

		int x = (int)(startPos.x / biomStep + 0.5f);
		int y = (int)(startPos.z / biomStep + 0.5f);

		if(elapsedTime >= shiftingUpdateFreq)
		{
			interpolateBiomShift();

			cb.interpVal = biomInterpolationStep.at(x + y * blendSize).z * -1 > 0 ? 0.f : -0.001f;
			
			terrainContainer.duration -= shiftingUpdateFreq;
			elapsedTime = 0.f;
			if(terrainContainer.duration <= 0)
			{
				terrainContainer.duration = 0.f;
				shiftingInProgress = terrainContainer.biomShifting = false;

				//cb.interpVal = 0.f;

				BiomsContainer& biom = *Containers(std::make_pair(Containers::BIOMS, READ)).get<BiomsContainer>(Containers::BIOMS);
				std::vector<glm::vec2> temp = biom.desertPoints;
				biom.desertPoints = biom.meadowPoints;
				biom.meadowPoints = temp;
				terrainContainer.biomMap.map1 = newBiomVector;

				//timeToNextShift = 300.f - (float)(time(0) % 300);
				timeToNextShift = 1;
			}
		}
		else
		{
			float val = biomInterpolationStep.at(x + y * blendSize).z * -1;
			cb.interpVal = val * (elapsedTime / shiftingUpdateFreq);
		}
		graphics->updateCBuffer(terrainContainer.cbShifting, &cb);
	}
	else if(terrainContainer.biomShifting)
	{
		shiftingUpdateFreq = 0.5f;
		shiftingInProgress = true;

		BiomsContainer& biom = *Containers(std::make_pair(Containers::BIOMS, READ)).get<BiomsContainer>(Containers::BIOMS);
		newMeadowPoints = biom.desertPoints;
		newDesertPoints = biom.meadowPoints;

		initBiomShift();
		elapsedTime = shiftingUpdateFreq;
		//interpolateBiomShift();
	}
}

void TerrainAspect::createTerrainFromPosition(glm::vec2 _spawnPos)
{
	float playerChunkX = _spawnPos.x / terrainContainer.largeSize;
	float playerChunkZ = _spawnPos.y / terrainContainer.largeSize;

	int x = (int)playerChunkX - (playerChunkX < 0 ? 1 : 0);
	int z = (int)playerChunkZ - (playerChunkZ < 0 ? 1 : 0);
	glm::vec3 largeMid = terrainContainer.large[4].position = glm::vec3(x * terrainContainer.largeSize, 175.f, z * terrainContainer.largeSize);

	int offset = (int)(terrainContainer.largeSize / 10);
	int range = (int)(terrainContainer.largeSize * 3) - offset * 2;
	timeToNextShift = 10000;

	// make sure all the terrain have shifted the same for all players
	//if((time(0) % 300) % 2 == 1)
	//{
	//	std::vector<float> temp = d;
	//	d = g;
	//	g = temp;
	//}

	playerChunkX = (terrainContainer.large[4].position.x + terrainContainer.largeSize - _spawnPos.x) / terrainContainer.largeSize;
	playerChunkZ = (terrainContainer.large[4].position.z + terrainContainer.largeSize - _spawnPos.y) / terrainContainer.largeSize;

	int idX = playerChunkX < 0.33f ? 2 : playerChunkX < 0.67f ? 1 : 0;
	int idZ = playerChunkZ < 0.33f ? 2 : playerChunkZ < 0.67f ? 1 : 0;

	glm::vec3 mediumMid = terrainContainer.medium[4].position = largeMid + glm::vec3(idX * (terrainContainer.mediumSize), -115.f, idZ * (terrainContainer.mediumSize));

	playerChunkX = (terrainContainer.large[4].position.x + terrainContainer.mediumSize * (idX + 1) - _spawnPos.x) / terrainContainer.mediumSize;
	playerChunkZ = (terrainContainer.large[4].position.z + terrainContainer.mediumSize * (idZ + 1) - _spawnPos.y) / terrainContainer.mediumSize;

	idX = playerChunkX < 0.33f ? 2 : playerChunkX < 0.67f ? 1 : 0;
	idZ = playerChunkZ < 0.33f ? 2 : playerChunkZ < 0.67f ? 1 : 0;

	glm::vec3 tinyMid = terrainContainer.tiny[4].position = mediumMid + glm::vec3(terrainContainer.tinySize * idX, -mediumMid.y, terrainContainer.tinySize * idZ);

	float tinySize = terrainContainer.tinySize;
	float mediumSize = terrainContainer.mediumSize;
	float largeSize = terrainContainer.largeSize;
	for(int i = 0; i < 9; i++)
	{
		terrainContainer.tiny[i].level = 1;
		terrainContainer.medium[i].level = 2;
		terrainContainer.large[i].level = 3;

		terrainContainer.tiny[i].heightsID = i;
		terrainContainer.medium[i].heightsID = 9 + i;
		terrainContainer.large[i].heightsID = 18 + i;

		if(i == 4)
			continue;

		terrainContainer.tiny[i].position = glm::vec3(tinyMid.x + (i / 3 - 1) * tinySize, tinyMid.y, tinyMid.z + ((i % 3) - 1) * tinySize);
		terrainContainer.medium[i].position = glm::vec3(mediumMid.x + (i / 3 - 1) * mediumSize, mediumMid.y, mediumMid.z + ((i % 3) - 1) * mediumSize);
		terrainContainer.large[i].position = glm::vec3(largeMid.x + (i / 3 - 1) * largeSize, largeMid.y, largeMid.z + ((i % 3) - 1) * largeSize);
	}

	// Create the blend texture for all bioms on the map
	createBiomMap();

	std::vector<std::thread> threads(9);
	for(int i = 0; i < 9; i++)
	{
		auto &big = terrainContainer.large[i];

		int t = int(big.position.x / terrainContainer.tinySize + big.position.y / terrainContainer.tinySize * 57);
		void* t2 = &t;
		ulong arg = terrainContainer.worldSeed + *(ulong*)t2;
		threads.at(i) = std::thread(&TerrainAspect::createChunkMap, this, std::ref(big), std::ref(arg));
	}

	for(int i = 0; i < 9; i++)
		threads.at(i).join();
	stitchAllChunks(&terrainContainer.large);

	for(int i = 0; i < 9; i++)
	{
		auto &med = terrainContainer.medium[i];

		int t = int(med.position.x / terrainContainer.tinySize + med.position.y / terrainContainer.tinySize * 57);
		void* t2 = &t;
		ulong arg = terrainContainer.worldSeed + *(ulong*)t2;
		threads.at(i) = std::thread(&TerrainAspect::createChunkMap, this, std::ref(med), std::ref(arg));
	}

	for(int i = 0; i < 9; i++)
		threads.at(i).join();
	stitchAllChunks(&terrainContainer.medium);
}

void TerrainAspect::createTinyFromPosition(glm::vec2 _spawnPos)
{
	if(reCreateChunkThread.joinable())
		reCreateChunkThread.join();

	for(int i = 0; i < 9; i++)
	{
		auto &tiny = terrainContainer.tiny[0];

		int t = int(tiny.position.x / terrainContainer.tinySize + tiny.position.y / terrainContainer.tinySize * 57);
		void* t2 = &t;
		ulong arg = terrainContainer.worldSeed + *(ulong*)t2;
		//threads.at(i) = std::thread(&TerrainAspect::createChunkMap, this, std::ref(tiny), std::ref(arg));
		createChunkMap(terrainContainer.tiny[i], arg);
	}

	//for(int i = 0; i < 9; i++)
	//	threads.at(i).join();
	stitchAllChunks(&terrainContainer.tiny);

	// calculate all indices for buffer and indexbuffer
	calculateBufferOffsets();

	std::thread t1(&TerrainAspect::createChunks, this, std::ref(terrainContainer.tiny));
	std::thread t2(&TerrainAspect::createChunks, this, std::ref(terrainContainer.medium));
	std::thread t3(&TerrainAspect::createChunks, this, std::ref(terrainContainer.large));

	if(terrainContainer.biomMapTextureID1 == -1)
	{
		terrainContainer.biomMapTextureID2 = graphics->createTexture2D32(BLENDTEXSIZE * 3 + 1, BLENDTEXSIZE * 3 + 1, (void**)&terrainContainer.newBiomMap.map2[0]);
		terrainContainer.biomMapTextureID1 = graphics->createTexture2D32(BLENDTEXSIZE * 3 + 1, BLENDTEXSIZE * 3 + 1, (void**)&terrainContainer.newBiomMap.map1[0]);
		terrainContainer.biomMapID1 = graphics->createShaderResourceView32(terrainContainer.biomMapTextureID1);
		terrainContainer.biomMapID2 = graphics->createShaderResourceView32(terrainContainer.biomMapTextureID2);

		newBiomMapTextureID = graphics->createTexture2D32(BLENDTEXSIZE * 3 + 1, BLENDTEXSIZE * 3 + 1, (void**)&terrainContainer.newBiomMap.map1[0]);
		newBiomMapID = graphics->createShaderResourceView32(terrainContainer.biomMapTextureID1);
	}
	else
	{
		for(int j = 0; j < 9; j++)
		{
			graphics->updateTexture2D(terrainContainer.grassTextureIDs[j], 256, 256, (void**)&terrainContainer.heights[terrainContainer.tiny[j].heightsID][0], sizeof(float));
			//graphics->replaceShaderResourceViewTest(terrainContainer.tiny[j].heightmapShaderResource, terrainContainer.grassTextureIDs[j]);
		}
	}

	createTextureFromBiomMap(terrainContainer.biomMapTextureID1, terrainContainer.biomMapID1, terrainContainer.biomMapTextureID1, terrainContainer.biomMapID1, &terrainContainer.newBiomMap.map1);
	createTextureFromBiomMap(terrainContainer.biomMapTextureID2, terrainContainer.biomMapID2, terrainContainer.biomMapTextureID2, terrainContainer.biomMapID2, &terrainContainer.newBiomMap.map2);

	for(UINT i = 0; i < terrainContainer.tiny.size(); i++)
		cbuffers.cbTerrain.tinyChunkPos[i] = glm::vec4(terrainContainer.tiny.at(i).position.x, terrainContainer.tiny.at(i).position.y, terrainContainer.tiny.at(i).position.z, 1);
	cbuffers.cbTerrain.startPos = glm::vec2(terrainContainer.large[0].position.x, terrainContainer.large[0].position.z);
	graphics->updateCBuffer(terrainContainer.cbTerrInfo, &cbuffers.cbTerrain);

	//playerContainer.playerCullingPos = playerContainer.position;

	t1.join();
	t2.join();
	t3.join();

	correctEdges();
}

void TerrainAspect::calculateCullingFromSpawn(glm::vec2 _spawnPos)
{
	terrainContainer.tinyActive = terrainContainer.tiny;
	terrainContainer.mediumActive = terrainContainer.medium;
	terrainContainer.largeActive = terrainContainer.large;

	std::vector<NonaNode> largeNodes, mediumNodes, tinyNodes;

	tinyNodes.push_back(NonaNode(graphics, &terrainContainer.tesselationchunks));
	for(uint i = 0; i < terrainContainer.tiny.size(); i++)
		tinyNodes.at(0).addNode(new NonaNode(graphics, &terrainContainer.tesselationchunks));

	mediumNodes.push_back(NonaNode(graphics, &terrainContainer.tesselationchunks));
	for(uint i = 0; i < terrainContainer.medium.size(); i++)
		mediumNodes.at(0).addNode(new NonaNode(graphics, &terrainContainer.tesselationchunks));

	largeNodes.push_back(NonaNode(graphics, &terrainContainer.tesselationchunks));
	for(uint i = 0; i < terrainContainer.large.size(); i++)
		largeNodes.at(0).addNode(new NonaNode(graphics, &terrainContainer.tesselationchunks));

	terrainContainer.grass = new GrassNode(graphics, true);

	playerContainer.playerCullingPos = glm::vec3(_spawnPos.x, 1000, _spawnPos.y);
	terrainContainer.biomMap = terrainContainer.newBiomMap;
	terrainContainer.newBiomMap.map1.clear();
	terrainContainer.newBiomMap.map1.shrink_to_fit();

	for(uint i = 0; i < terrainContainer.tinyActive.size(); i++)
		tinyNodes.at(0).children.at(i)->addChunk(&terrainContainer.tinyActive.at(i));

	for(uint i = 0; i < terrainContainer.mediumActive.size(); i++)
		mediumNodes.at(0).children.at(i)->addChunk(&terrainContainer.mediumActive.at(i));

	for(uint i = 0; i < terrainContainer.largeActive.size(); i++)
		largeNodes.at(0).children.at(i)->addChunk(&terrainContainer.largeActive.at(i));

	terrainContainer.smallNodes = tinyNodes;
	terrainContainer.mediumNodes = mediumNodes;
	terrainContainer.largeNodes = largeNodes;

	tinyNodes.clear(); tinyNodes.shrink_to_fit();
	mediumNodes.clear(); mediumNodes.shrink_to_fit();
	largeNodes.clear(); largeNodes.shrink_to_fit();
}

void TerrainAspect::recreateChunks()
{
	setHeightsID(&terrainContainer.tiny, terrainContainer.tinySize);
	for(int j = 0; j < 9; j++)
		graphics->updateTexture2D(terrainContainer.grassTextureIDs[j], 256, 256, (void**)&terrainContainer.heights[terrainContainer.tiny[j].heightsID][0], sizeof(float));

	buffer.resize(terrainContainer.tiny[0].size * 27);
	indexBuffer.resize((terrainContainer.tiny[0].size_x - 1) * (terrainContainer.tiny[0].size_y - 1) * 6 * 27);
	
	tempNVerts = indexBuffer.size();

	//std::thread t1(&TerrainAspect::createChunks, this, std::ref(terrainContainer.tiny));
	//std::thread t2(&TerrainAspect::createChunks, this, std::ref(terrainContainer.medium));
	//std::thread t3(&TerrainAspect::createChunks, this, std::ref(terrainContainer.large));
	//
	//
	//t1.join();
	//t2.join();
	//t3.join();

	createChunks(terrainContainer.tiny);
	createChunks(terrainContainer.medium);
	createChunks(terrainContainer.large);
	
	correctEdges();
	createTerrainBuffer();

	terrainContainer.tinyActive		= terrainContainer.tiny;
	terrainContainer.mediumActive	= terrainContainer.medium;
	terrainContainer.largeActive	= terrainContainer.large;
	playerContainer.playerCullingPos = playerContainer.position;
	updateCullingNodes();

	chunkGenerationLock = false;
}

void pushBack(std::vector<int> &chunks, int i1, int i2, int i3, int i4 = -1, int i5 = -1)
{
	chunks.push_back(i1);
	chunks.push_back(i2);
	chunks.push_back(i3);
	if(i4 != -1 && i5 != -1)
	{
		chunks.push_back(i4);
		chunks.push_back(i5);
	}
}

template < uint X, uint Y >
void TerrainAspect::setHeightsID(std::array<Chunk<X, Y>, 9> *_chunks, float _size)
{
	std::vector<int> outdatedChunks;

	int newChunk = getNewChunk(_chunks, _size);

	if(newChunk == 4)
		return;

	if(newChunk == 0)			pushBack(outdatedChunks, 0, 1, 2, 3, 6);// +4
	else if(newChunk == 1)		pushBack(outdatedChunks, 0, 1, 2);		// +3
	else if(newChunk == 2)		pushBack(outdatedChunks, 0, 1, 2, 5, 8);// +2
	else if(newChunk == 3)		pushBack(outdatedChunks, 0, 3, 6);		// +1
	else if(newChunk == 5)		pushBack(outdatedChunks, 2, 5, 8);		// -1
	else if(newChunk == 6)		pushBack(outdatedChunks, 0, 3, 6, 7, 8);// -2
	else if(newChunk == 7)		pushBack(outdatedChunks, 6, 7, 8);		// -3
	else if(newChunk == 8)		pushBack(outdatedChunks, 2, 5, 6, 7, 8);// -4

	_chunks->at(4).position = _chunks->at(newChunk).position;
	glm::vec3 midPos = _chunks->at(4).position;
	for(int i = 0; i < 9; i++)
	{
		if(i != 4)
			_chunks->at(i).position = glm::vec3(midPos.x + (i / 3 - 1) * _size, midPos.y, midPos.z + ((i % 3) - 1) * _size);
	}

	if (_chunks->at(0).level == 3)
	{
		createBiomMap();
		if(terrainContainer.biomShifting)
			initBiomShift();
		createTextureFromBiomMap(terrainContainer.biomMapTextureID1, terrainContainer.biomMapID1, terrainContainer.biomMapTextureID1, terrainContainer.biomMapID1, &terrainContainer.newBiomMap.map1);
		createTextureFromBiomMap(terrainContainer.biomMapTextureID2, terrainContainer.biomMapID2, terrainContainer.biomMapTextureID2, terrainContainer.biomMapID2, &terrainContainer.newBiomMap.map2);
	}

	for(uint i = 0; i < terrainContainer.tiny.size(); i++)
		cbuffers.cbTerrain.tinyChunkPos[i] = glm::vec4(terrainContainer.tiny.at(i).position, 1);

	cbuffers.cbTerrain.startPos = glm::vec2(terrainContainer.large[0].position.x, terrainContainer.large[0].position.z);

	graphics->updateCBuffer(terrainContainer.cbTerrInfo, &cbuffers.cbTerrain);

	int at = 0;
	std::vector<int> ids;
	if((newChunk - 4) * -1 < 0)
	{
		for(int i = 0; i < 9; i++)
		{
			if(i + (newChunk - 4) * -1 >= 0 && i + (newChunk - 4) * -1 < 9)
				_chunks->at(i + (newChunk - 4) * -1).heightsID = _chunks->at(i).heightsID;
			else
				ids.push_back(_chunks->at(i).heightsID);
		}
		for(int i = 8; i > 8 + (newChunk - 4) * -1; i--)
			_chunks->at(i).heightsID = ids.at(8 - i);
	}
	else
	{
		for(int i = 8; i >= 0; i--)
		{
			if(i + (newChunk - 4) * -1 >= 0 && i + (newChunk - 4) * -1 < 9)
				_chunks->at(i + (newChunk - 4) * -1).heightsID = _chunks->at(i).heightsID;
			else
				ids.push_back(_chunks->at(i).heightsID);
		}
		for(int i = 8; i > 8 - (newChunk - 4) * -1; i--)
			_chunks->at(i + (newChunk - 4) * -1 - 9).heightsID = ids.at(8 - i);
	}

	// This needs to happen as soon as possible, to erase the possibility of ending up in the wrong index
	if(_chunks->at(0).level == 1)
	{
		playerContainer.terrainStartPos = terrainContainer.tiny[4].position;
		playerContainer.smallTerrainID = 4;

		setHeightsID(&terrainContainer.large, terrainContainer.largeSize);
		setHeightsID(&terrainContainer.medium, terrainContainer.mediumSize);
	}

	//std::vector<std::thread> threads(outdatedChunks.size());
	for(uint i = 0; i < outdatedChunks.size(); i++)
	{
		auto terr = _chunks->at(outdatedChunks.at(i));
		int t = int(terr.position.x / terrainContainer.tinySize + terr.position.y / terrainContainer.tinySize * 57);
		void* t2 = &t;
		ulong arg = terrainContainer.worldSeed + *(ulong*)t2;
		//if(_chunks->at(0).level != 1)
		//	threads.at(i) = std::thread(&TerrainAspect::createChunkMap, this, std::ref(terr), std::ref(arg));
		//else
			createChunkMap(terr, terrainContainer.worldSeed + *(ulong*)t2);
	}

	//if(threads.at(0).joinable())
	//{
	//	for(uint i = 0; i < outdatedChunks.size(); i++)
	//		threads.at(i).join();
	//}

	stitchAllChunks(_chunks);
}

int TerrainAspect::getNewChunk(std::array<Chunk<CHUNKSIZE, CHUNKSIZE>, 9> *_chunks, float _size)
{
	glm::vec3 playerPos = playerContainer.position;
	if(_chunks->at(0).level != 1)
	{
		for(int i = 0; i < 9; i++)
		{
			if(playerPos.x < _chunks->at(i).position.x + _size && playerPos.x > _chunks->at(i).position.x &&
				playerPos.z < _chunks->at(i).position.z + _size && playerPos.z > _chunks->at(i).position.z)
			{
				return i;
			}
		}
	}
		
	return playerContainer.smallTerrainID;
}

void TerrainAspect::createChunks(std::array< Chunk< CHUNKSIZE, CHUNKSIZE >, 9 > &_chunks)
{
	for(int i = 0; i < 9; i++)
	{
		createTerrainChunk(_chunks[i]);
		createTerrainNormals(_chunks[i]);
	}
}

template < uint X, uint Y >
void TerrainAspect::stitchAllChunks(std::array<Chunk<X, Y>, 9> *_chunks)
{
	// Stitching --
	for(int i = 1; i < 9; i++)
	{
		if(i % 3 != 0)
			stitchChunks(_chunks->at(i - 1), _chunks->at(i), _chunks->at(i - 1).bufferStartID, _chunks->at(i).bufferStartID);
	}

	// Stitching |
	for(int i = 3; i < 9; i++)
	{
		stitchChunks(_chunks->at(i), _chunks->at(i - 3), _chunks->at(i).bufferStartID, _chunks->at(i - 3).bufferStartID);
	}
}

void TerrainAspect::createChunkMap(Chunk< CHUNKSIZE, CHUNKSIZE > &_terrain, ulong _seed)
{
	uint X = CHUNKSIZE, Y = CHUNKSIZE;
	std::mt19937 numberGenerator;
	numberGenerator.seed(_seed);
	std::vector<float> randomMap((X + 1) * (Y + 1));
	for(uint x(0); x < X + 1; x++)
	{
		for(uint y(0); y < Y + 1; y++)
		{
			randomMap[y*(Y + 1) + x] = range(numberGenerator);
			for(int i(0); i < _terrain.level - 1; i++)
				range(numberGenerator);
		}
		for(uint i(0); i < (_terrain.level - 1) * (Y + 1); i++)
			range(numberGenerator);
	}

	std::vector<float> tempA(X * Y), tempB(X * Y), tempC(X * Y), tempD(X * Y);
	auto interpolate = [](float a, float b, float x)->float
	{
		//float t = (1 - cos(x * 3.1415927f)) * .5f;
		float t = x;
		return a*(1 - t) + b*t;
	};
	if(_terrain.level == 1)
	{
		// Find which large chunk its contained in and were in that chunk it is
		glm::vec2 pos(_terrain.position.x, _terrain.position.z);
		for(int i(0); i < 9; i++)
		{
			glm::vec2 posLarge(terrainContainer.medium[i].position.x, terrainContainer.medium[i].position.z);
			float largeSize(terrainContainer.mediumSize);
			if(pos.x + 300 < posLarge.x + largeSize && pos.x + 300 >= posLarge.x)
			{
				if(pos.y + 300 < posLarge.y + largeSize && pos.y + 300 >= posLarge.y)
				{
					// Found it
					// Set temp values to interpolated values from the large chunk
					//					auto large = [&](uint x, uint y)->float&{ return terrainContainer.medium[i].heights[terrainContainer.medium[i].size_y*y + x]; };
					auto large = [&](uint x, uint y)->float&{ return terrainContainer.heights.at(terrainContainer.medium[i].heightsID)[Y*y + x]; };
					auto result = [&](uint x, uint y)->float&{ return tempA[Y*y + x]; };
					for(uint x(0); x < X; x++)
					{
						result(x, 0) = large(x, 0);
						result(X - x - 1, 0) = large(X - x - 1, 0);
					}
					for(uint y(0); y < Y; y++)
					{
						result(0, y) = large(0, y);
						result(Y - y - 1, 0) = large(Y - y - 1, 0);
					}
					for(uint x(1); x < X - 1; x++)
					{
						for(uint y(1); y < Y - 1; y++)
						{
							float sizeDifference = largeSize / terrainContainer.tinySize;
							glm::vec2 pointPos;
							pointPos.x = Utility::length(pos.x - posLarge.x) / (QUADSIZE * 3.f) + x / sizeDifference;
							pointPos.y = Utility::length(pos.y - posLarge.y) / (QUADSIZE * 3.f) + y / sizeDifference;

							uint intX = uint(pointPos.x);
							float fracX = pointPos.x - float(intX);

							uint intY = uint(pointPos.y);
							float fracY = pointPos.y - float(intY);

							result(x, y) = interpolate(
								interpolate(
								large(intX + 0, intY + 0),
								large(intX + 1, intY + 0),
								fracX),
								interpolate(
								large(intX + 0, intY + 1),
								large(intX + 1, intY + 1),
								fracX),
								fracY);
						}
					}
					tempD = tempC = tempB = tempA;
					break;
				}
			}
		}
		Utility::perlinNoise(tempA, randomMap, X, Y, 16, 64, 64);
		Utility::perlinNoise(tempB, randomMap, X, Y, 32, 64, 128);
		//Utility::perlinNoise(tempC, randomMap, X, Y, 8, 32, 64);
		//Utility::perlinNoise(tempD, randomMap, X, Y, 32, 64, 64);
	}
	else if(_terrain.level == 2)
	{
		// Find which large chunk its contained in and were in that chunk it is
		glm::vec2 pos(_terrain.position.x, _terrain.position.z);
		for(int i(0); i < 9; i++)
		{
			glm::vec2 posLarge(terrainContainer.large[i].position.x, terrainContainer.large[i].position.z);
			float largeSize(terrainContainer.largeSize);
			if(pos.x + 300 < posLarge.x + largeSize && pos.x + 300 >= posLarge.x)
			{
				if(pos.y + 300 < posLarge.y + largeSize && pos.y + 300 >= posLarge.y)
				{
					// Found it
					// Set temp values to interpolated values from the large chunk
					//					auto large = [&](uint x, uint y)->float&{ return terrainContainer.large[i].heights[terrainContainer.large[i].size_y*y + x]; };
					auto large = [&](uint x, uint y)->float&{ return terrainContainer.heights.at(terrainContainer.large[i].heightsID)[Y*y + x]; };
					auto result = [&](uint x, uint y)->float&{ return tempA[Y*y + x]; };
					for(uint x(0); x < X; x++)
					{
						result(x, 0) = large(x, 0);
						result(X - x - 1, 0) = large(X - x - 1, 0);
					}
					for(uint y(0); y < Y; y++)
					{
						result(0, y) = large(0, y);
						result(Y - y - 1, 0) = large(Y - y - 1, 0);
					}
					for(uint x(1); x < X - 1; x++)
					{
						for(uint y(1); y < Y - 1; y++)
						{
							float sizeDifference = largeSize / terrainContainer.mediumSize;
							glm::vec2 pointPos;
							pointPos.x = Utility::length(pos.x - posLarge.x) / (QUADSIZE * 9.f) + x / sizeDifference;
							pointPos.y = Utility::length(pos.y - posLarge.y) / (QUADSIZE * 9.f) + y / sizeDifference;

							uint intX = uint(pointPos.x);
							float fracX = pointPos.x - float(intX);

							uint intY = uint(pointPos.y);
							float fracY = pointPos.y - float(intY);

							result(x, y) = interpolate(
								interpolate(
								large(intX + 0, intY + 0),
								large(intX + 1, intY + 0),
								fracX),
								interpolate(
								large(intX + 0, intY + 1),
								large(intX + 1, intY + 1),
								fracX),
								fracY);
						}
					}
					tempD = tempC = tempB = tempA;
					break;
				}
			}
		}
		Utility::perlinNoise(tempA, randomMap, X, Y, 8, 32, 128);
		Utility::perlinNoise(tempB, randomMap, X, Y, 16, 32, 256);
		//Utility::perlinNoise(tempC, randomMap, X, Y, 4, 16, 128);
		//Utility::perlinNoise(tempD, randomMap, X, Y, 16, 32, 128);
	}
	else if(_terrain.level == 3)
	{
		memset(&tempA[0], 0, X*Y*sizeof(float));
		memset(&tempB[0], 0, X*Y*sizeof(float));
		memset(&tempC[0], 0, X*Y*sizeof(float));
		memset(&tempD[0], 0, X*Y*sizeof(float));
		Utility::perlinNoise(tempA, randomMap, X, Y, 4, 16, 256);
		Utility::perlinNoise(tempB, randomMap, X, Y, 16, 32, 2048);
		Utility::erosion(tempB, X, Y, 100, 10);
		Utility::perlinNoise(tempC, randomMap, X, Y, 4, 4, 80); // 2 8
		Utility::perlinNoise(tempD, randomMap, X, Y, 16, 32, 1024);
	}
	randomMap.clear();

	if(_terrain.level != 3)
		mergePerlinMaps(_terrain, tempA, tempB, X, Y, _terrain.level);
	else
		mergeLargePerlinMaps(_terrain, tempA, tempB, tempC, tempD, X, Y);
	tempA.clear();
	tempB.clear();
	tempC.clear();
	tempD.clear();

	int sizeXY = _terrain.size, sizeX = _terrain.size_x, sizeY = _terrain.size_y;
	auto &heights = terrainContainer.heights[_terrain.heightsID];
	for(int i = 0; i < sizeX; i++)
	{
		heights[i] = heights[i + sizeX];
		heights[sizeXY - i - 1] = heights[sizeXY - i - 1 - sizeX];

		heights[i * sizeY] = heights[i * sizeY + 1];
		heights[i * sizeY + sizeX - 1] = heights[i * sizeY + sizeX - 2];
	}
	heights[0] = heights[sizeX];
}

template < uint X, uint Y >
void TerrainAspect::createTerrainChunk(Chunk< X, Y > &_terrain)
{
	uint nPoints = (X - 1) * (Y - 1);

	std::array<float, X * Y> &heights = terrainContainer.heights[_terrain.heightsID];

	glm::vec3 startPos = _terrain.position;
	float pointStep = (float)(QUADSIZE * pow(3, _terrain.level - 1));

	int offset, i;
	offset = i = _terrain.bufferStartID;
	int ii = _terrain.iBufferStartID;

	int index1, index2;

	for(int y = 0; y < Y; y++)
	{
		for(int x = 0; x < X; x++)
		{
			buffer.at(i).pos = glm::vec3(startPos.x + x * pointStep, heights[i - offset] + startPos.y, startPos.z + y * pointStep);
			buffer.at(i).texCoord.x = (buffer.at(i).pos.x + (0.5f * X)) / X;
			buffer.at(i).texCoord.y = (buffer.at(i).pos.z - (0.5f * Y)) / -(int)Y;
			i++;
		}
	}

	i = offset;

	//small chunk (splits to 15x15 sub-chunks), medium or large chunk (splits to 3x3 sub-chunks)
	int chunkSizeX = _terrain.level == 1 ? X / 15 : X / 3;
	int chunkSizeY = _terrain.level == 1 ? Y / 15 : Y / 3;

	int xCond = _terrain.level == 1 ? 15 : 3;
	int yCond = _terrain.level == 1 ? 15 : 3;

	for(int chunky = 0; chunky < yCond; chunky++)
	{
		for(int chunkx = 0; chunkx < xCond; chunkx++)
		{
			for(int y = chunkSizeY * chunky; y < chunkSizeY * (chunky + 1); y++)
			{
				for(int x = chunkSizeX * chunkx; x < chunkSizeX * (chunkx + 1); x++)
				{
					if(x == X - 1 || y == Y - 1)
						continue;
					index1 = offset + x + y * X;
					index2 = offset + x + (y + 1) * X;

					indexBuffer.at(ii++) = index1;
					indexBuffer.at(ii++) = index2 + 1;
					indexBuffer.at(ii++) = index2;
					indexBuffer.at(ii++) = index1;
					indexBuffer.at(ii++) = index1 + 1;
					indexBuffer.at(ii++) = index2 + 1;

					i++;
				}
			}
		}
	}
}

template < uint X, uint Y >
void TerrainAspect::createTerrainNormals(Chunk< X, Y > &_terrain)
{
	uint nPoints = X * Y;

	std::vector<int> indices = std::vector<int>(nPoints);
	std::vector<glm::vec3> norms = std::vector<glm::vec3>(nPoints);

	int offset = _terrain.bufferStartID;

	glm::vec3 norm;
	int index1, index2, i = offset;

	for(int y = 0, i = offset; y < Y - 1; y++)
	{
		for(int x = 0; x < X - 1; x++)
		{
			index1 = (int)(y		* X + x + offset);
			index2 = (int)((y + 1)	* X + x + offset);

			norm = glm::cross(buffer.at(index1).pos - buffer.at(index2).pos,
				buffer.at(index1).pos - buffer.at(index2 + 1).pos);

			// ADD norm to indices that are in the triangle, so it can be averaged. Not yet to be mapped in
			norms.at(index1 - offset) += norm;
			norms.at(index2 + 1 - offset) += norm;
			norms.at(index2 - offset) += norm;

			norm = glm::cross(buffer.at(index1).pos - buffer.at(index2 + 1).pos,
				buffer.at(index1).pos - buffer.at(index1 + 1).pos);

			// ADD norm to indices that are in the triangle, so it can be averaged. Not yet to be mapped in
			norms.at(index1 - offset) += norm;
			norms.at(index1 + 1 - offset) += norm;
			norms.at(index2 + 1 - offset) += norm;

			buffer.at(index1).normal = glm::normalize(norms.at(index1 - offset));
		}
	}
	for(i = index1; i < index2; i++)
	{
		buffer.at(i).normal = glm::normalize(norms.at(i - offset));
		buffer.at((i - index1) * Y + X - 1 + offset).normal = glm::normalize(norms.at((i - index1) * Y + X - 1));
	}

	buffer.at(X * Y - 1 + offset).normal = glm::normalize(norms.at(X * Y - 1));
	buffer.at(X * Y - 2 + offset).normal = glm::normalize(norms.at(X * Y - 2));
}

template < uint X, uint Y >
void TerrainAspect::stitchChunks(Chunk< X, Y > &_chunk1, Chunk< X, Y > &_chunk2, int _offset1, int _offset2)
{
	if(_chunk1.position.x != _chunk2.position.x && _chunk1.position.z != _chunk2.position.z && _chunk1.position.y == _chunk2.position.y)
		throw new std::exception("Cannot stich chunks that are not adjecent.");

	double mult = 0.0625 / pow(2, _chunk1.level - 1);
	uint yCond = _chunk1.position.x != _chunk2.position.x ? Y : (int)(mult * Y);
	uint xCond = _chunk1.position.z != _chunk2.position.z ? X : (int)(mult * X);

	int index1, index2, size = X * Y;

	float percentage, percentageStep, percent, div, yPos = _chunk1.position.y;

	div = 1.f / (std::min(xCond - 1, yCond - 1) / 3 * 7.5f);
	percentage = percent = 1;
	percentageStep = -1.f / std::min(xCond - 1, yCond - 1);

	std::array<float, X * Y> &heights1 = terrainContainer.heights[_chunk1.heightsID];
	std::array<float, X * Y> &heights2 = terrainContainer.heights[_chunk2.heightsID];

	// Stitch the 3.125% (2^-5) of each chunk
	if(_chunk1.position.x != _chunk2.position.x)
	{
		for(uint y = 0; y < yCond; y++)
		{
			float baseHeight = (heights1[y * Y + 2] + heights2[y * Y + X - 3]) / 2;

			heights1[y * Y] = heights2[y * Y + X - 1] = baseHeight;
			percent += percentageStep;

			for(uint x = X - 2; x >= X - xCond; x--)
			{
				index1 = y * Y + X - x - 1;
				index2 = y * Y + x;

				float _percent = pow(percent, 1 + (X - x - (float)xCond / 2) * div);
				heights1[index1] = (heights1[index1]) * (1 - _percent) + baseHeight * _percent;
				heights2[index2] = (heights2[index2]) * (1 - _percent) + baseHeight * _percent;

				percent += percentageStep;
				if(percent < 0.0001)
					percent = 0;
			}
			percent = percentage;
		}
	}
	else
	{
		for(uint x = 0; x < xCond; x++)
		{
			float baseHeight = (heights1[(Y - 3) * Y + x] + heights2[x + Y + Y]) / 2;

			heights1[(Y - 1) * Y + x] = heights2[x] = baseHeight;
			percent += percentageStep;

			for(uint y = Y - 2; y >= Y - yCond; y--)
			{
				index1 = y * Y + x;
				index2 = (Y - y - 1) * Y + x;

				float _percent = pow(percent, 1 + (Y - y - (float)yCond / 2) * div);

				heights1[index1] = (heights1[index1]) * (1 - _percent) + baseHeight * _percent;
				heights2[index2] = (heights2[index2]) * (1 - _percent) + baseHeight * _percent;

				percent += percentageStep;
				if(percent < 0.0001)
					percent = 0;
			}
			percent = percentage;
		}
	}
}

void TerrainAspect::createTerrainBuffer()
{
	if(terrainContainer.bufferID == -1)
	{
		terrainContainer.bufferID = graphics->createVBufferImmutable((void**)&buffer[0], buffer.size(), sizeof(buffer[0]));
		tempVBufferID = graphics->createVBufferImmutable((void**)&buffer[0], 1, sizeof(buffer[0]));

		buffer.clear();
		buffer.shrink_to_fit();

		terrainContainer.indexBufferID = graphics->createIndexBufferImmutable((void**)&indexBuffer[0], indexBuffer.size());
		tempIBufferID = graphics->createIndexBufferImmutable((void**)&indexBuffer[0], 1);

		indexBuffer.clear();
		indexBuffer.shrink_to_fit();
	}
	else
	{
		graphics->replaceImmutableVBuffer(tempVBufferID, (void**)&buffer[0], buffer.size(), sizeof(buffer[0]));
		buffer.clear();
		buffer.shrink_to_fit();

		graphics->replaceImmutableIBuffer(tempIBufferID, (void**)&indexBuffer[0], indexBuffer.size());
		indexBuffer.clear();
		indexBuffer.shrink_to_fit();

		int temp1 = tempVBufferID, temp2 = tempIBufferID;
		tempVBufferID = terrainContainer.bufferID;
		tempIBufferID = terrainContainer.indexBufferID;
		terrainContainer.bufferID = temp1;
		terrainContainer.indexBufferID = temp2;

		// What should this arbitrary buffer contain? Why, 42 of course!
		std::vector<int> tempBuffer(1, 42);

		graphics->replaceImmutableIBuffer(tempVBufferID, (void**)&tempBuffer[0], 1);
		graphics->replaceImmutableIBuffer(tempIBufferID, (void**)&tempBuffer[0], 1);
	}

	terrainContainer.nVertices = tempNVerts;
}

void TerrainAspect::correctEdges()
{
	int offsets1[] = { terrainContainer.tiny[0].bufferStartID, terrainContainer.tiny[3].bufferStartID, terrainContainer.tiny[6].bufferStartID };
	int offsets2[] = { terrainContainer.tiny[2].bufferStartID, terrainContainer.tiny[5].bufferStartID, terrainContainer.tiny[8].bufferStartID };
	int offsets3[] = { terrainContainer.tiny[0].bufferStartID, terrainContainer.tiny[1].bufferStartID, terrainContainer.tiny[2].bufferStartID };
	int offsets4[] = { terrainContainer.tiny[6].bufferStartID, terrainContainer.tiny[7].bufferStartID, terrainContainer.tiny[8].bufferStartID };

	int offsets5[] = { terrainContainer.medium[0].bufferStartID, terrainContainer.medium[3].bufferStartID, terrainContainer.medium[6].bufferStartID };
	int offsets6[] = { terrainContainer.medium[2].bufferStartID, terrainContainer.medium[5].bufferStartID, terrainContainer.medium[8].bufferStartID };
	int offsets7[] = { terrainContainer.medium[0].bufferStartID, terrainContainer.medium[1].bufferStartID, terrainContainer.medium[2].bufferStartID };
	int offsets8[] = { terrainContainer.medium[6].bufferStartID, terrainContainer.medium[7].bufferStartID, terrainContainer.medium[8].bufferStartID };

	float add1 = 20.f, add2 = 120.f;

	float sizeT = QUADSIZE, sizeM = QUADSIZE * 3;
	glm::vec3 tiny = terrainContainer.tiny[0].position, medium = terrainContainer.medium[0].position;

	for(int i = 0; i < 3; i++)
	{
		int sizeXY = terrainContainer.tiny[0].size, sizeX = terrainContainer.tiny[0].size_x, sizeY = terrainContainer.tiny[0].size_y;
		for(int l = 0; l < sizeX; l++)
		{
			glm::vec2 pos1 = glm::vec2(tiny.x + (l + i * sizeX) * sizeT, tiny.z);
			glm::vec2 pos2 = glm::vec2(tiny.x + (l + i * sizeX) * sizeT, tiny.z + 3 * (sizeX - 1) * sizeT);
			glm::vec2 pos3 = glm::vec2(tiny.x, tiny.z + (l + i * sizeY) * sizeT);
			glm::vec2 pos4 = glm::vec2(tiny.x + 3 * (sizeY - 1) * sizeT, tiny.z + (l + i * sizeY) * sizeT);

			glm::vec2 pos5 = glm::vec2(medium.x + (l + i * sizeX) * sizeM, medium.z);
			glm::vec2 pos6 = glm::vec2(medium.x + (l + i * sizeX) * sizeM, medium.z + 3 * (sizeX - 1) * sizeM);
			glm::vec2 pos7 = glm::vec2(medium.x, medium.z + (l + i * sizeY) * sizeM);
			glm::vec2 pos8 = glm::vec2(medium.x + 3 * (sizeY - 1) * sizeM, medium.z + (l + i * sizeY) * sizeM);

			buffer[offsets1[i] + l].pos.y						= Utility::mapToTerrainForced(pos1, terrainContainer, 2) + 2.f;
			buffer[offsets2[i] + sizeXY - sizeX + l].pos.y		= Utility::mapToTerrainForced(pos2, terrainContainer, 2) + 2.f;
			buffer[offsets3[i] + l * sizeY].pos.y				= Utility::mapToTerrainForced(pos3, terrainContainer, 2) + 2.f;
			buffer[offsets4[i] + l * sizeY + sizeX - 1].pos.y	= Utility::mapToTerrainForced(pos4, terrainContainer, 2) + 2.f;

			buffer[offsets5[i] + l].pos.y						= Utility::mapToTerrainForced(pos5, terrainContainer, 3) + 15.f;
			buffer[offsets6[i] + sizeXY - sizeX + l].pos.y		= Utility::mapToTerrainForced(pos6, terrainContainer, 3) + 15.f;
			buffer[offsets7[i] + l * sizeY].pos.y				= Utility::mapToTerrainForced(pos7, terrainContainer, 3) + 15.f;
			buffer[offsets8[i] + l * sizeY + sizeX - 1].pos.y	= Utility::mapToTerrainForced(pos8, terrainContainer, 3) + 15.f;
		}
		buffer[offsets1[i]].pos.y = Utility::mapToTerrainForced(glm::vec2(tiny.x + i * sizeX * sizeT, tiny.z), terrainContainer, 2);
		buffer[offsets5[i]].pos.y = Utility::mapToTerrainForced(glm::vec2(medium.x + i * sizeX * sizeM, medium.z), terrainContainer, 3);
	}
}

float interpolate(float a, float b, float x)
{
	return a*(1 - x) + b*x;
}

void TerrainAspect::initBiomShift()
{
	createBiomMap(true);
	biomInterpolationStep.resize(newBiomVector.size());

	for(uint i = 0; i < newBiomVector.size(); i++)
		biomInterpolationStep.at(i) = (terrainContainer.biomMap.map1.at(i) - newBiomVector.at(i)) / (terrainContainer.duration * 2);
}

void TerrainAspect::interpolateBiomShift()
{
	std::vector<glm::vec4> &biomMap = terrainContainer.biomMap.map1;
	for(uint i = 0; i < newBiomVector.size(); i++)
		biomMap.at(i) -= biomInterpolationStep.at(i);

	createTextureFromBiomMap(newBiomMapTextureID, newBiomMapID, terrainContainer.biomMapTextureID1, terrainContainer.biomMapID1, &terrainContainer.biomMap.map1);

	int temp1 = terrainContainer.biomMapID1, temp2 = terrainContainer.biomMapTextureID1;
	terrainContainer.biomMapID1 = newBiomMapID;
	terrainContainer.biomMapTextureID1 = newBiomMapTextureID;
	newBiomMapID = temp1;
	newBiomMapTextureID = temp2;
}

SingleBiom calcBlend(const std::vector<float> &_lengths)
{
	SingleBiom out;
	for(int i = 0; i < NBIOMS; i++)
	{
		bool b = true;
		std::vector<float> lens;
		for(int j = 0; j < NBIOMS; j++)
		{
			if(j == i)
				continue;
			if(_lengths.at(j) < _lengths.at(i) - 5000)
			{
				i > 3 ? out.map2[i - 4] = 0.f : out.map1[i] = 0.f;
				b = false;
				break;
			}
			if(abs(_lengths.at(i) - _lengths.at(j)) < 5000)
				lens.push_back(_lengths.at(j));
		}
		if(!b)
			continue;
		if(lens.size() == 0)
		{
			i > 3 ? out.map2[i - 4] = 1.f : out.map1[i] = 1.f;
			continue;
		}

		float min = lens.at(0);
		for(uint k = 1; k < lens.size(); k++)
		{
			if(lens.at(k) < min)
				min = lens.at(k);
		}

		float val = 0.5f + (abs(_lengths.at(i) - min) / 10000) * (_lengths.at(i) < min ? 1 : -1);

		i > 3 ? out.map2[i - 4] = val : out.map1[i] = val;
	}

	float len = 1.f / (glm::length(out.map1 + out.map2));
	out.map1 *= len;
	out.map2 *= len;

	return out;
}

void minLengthToBiom(float &_len, const std::vector<glm::vec2> &_biom, glm::vec2 _pos)
{
	for(uint i = 0; i < _biom.size(); i++)
	{
		float len = glm::length(_pos - _biom.at(i));
		if(len < _len)
			_len = len;
	}
}

void TerrainAspect::createBiomMap(bool _shifting)
{	
	glm::vec2 startPos = glm::vec2(terrainContainer.large[0].position.x, terrainContainer.large[0].position.z);
	int ix = int(terrainContainer.large[4].position.x / terrainContainer.largeSize);
	int iy = int(terrainContainer.large[4].position.z / terrainContainer.largeSize);
	ix -= terrainContainer.large[4].position.x < 0 ? 1 : 0;
	iy -= terrainContainer.large[4].position.z < 0 ? 1 : 0;
	
	if(!_shifting)
		createBiomPoints(ix, iy);

	BiomsContainer& biom = *Containers(std::make_pair(Containers::BIOMS, READ)).get<BiomsContainer>(Containers::BIOMS);

	std::vector<glm::vec4> &biomMap1 = !_shifting ? terrainContainer.newBiomMap.map1 : newBiomVector, &biomMap2 = terrainContainer.newBiomMap.map2;
	std::vector<glm::vec2> &desert = !_shifting ? biom.desertPoints : newDesertPoints, &meadow = !_shifting ? biom.meadowPoints : newMeadowPoints;
	std::vector<glm::vec2> &mountain = biom.mountainPoints, &swamp = biom.swampPoints, &forest = biom.forestPoints;

	int size = BLENDTEXSIZE * 3 + 1;
	biomMap1.resize(size * size); biomMap2.resize(size * size);

	float step = 9 * (CHUNKSIZE / BLENDTEXSIZE) * QUADSIZE;
	for(int y = 0; y < size; y++)
	{
		for(int x = 0; x < size; x++)
		{
			glm::vec2 pos = glm::vec2(x * step, y * step) + startPos;

			std::vector<float> lens(NBIOMS, D3D11_FLOAT32_MAX);

			minLengthToBiom(lens.at(0), desert, pos);
			minLengthToBiom(lens.at(1), mountain, pos);
			minLengthToBiom(lens.at(2), meadow, pos);
			minLengthToBiom(lens.at(3), swamp, pos);
			minLengthToBiom(lens.at(4), forest, pos);

			SingleBiom sb = calcBlend(lens);
			biomMap1.at(y * size + x) = sb.map1;
			biomMap2.at(y * size + x) = sb.map2;
		}
	}
}

void TerrainAspect::createTextureFromBiomMap(int _texID, int _shaderResourceID, int _biomTexID, int _biomID, std::vector<glm::vec4> *_biomMap)
{
	graphics->replaceTexture2D32(_biomTexID, BLENDTEXSIZE * 3 + 1, BLENDTEXSIZE * 3 + 1, (void**)&_biomMap->at(0));
	graphics->replaceShaderResourceView32(_biomID, _biomTexID);
}

std::pair< std::pair<char, char>, std::pair<float, float> > biomMapping(BiomsContainer& _biom, glm::vec2 _position)
{
	char closest = 0, nextClosest = 0;
	float distanceClosest, distanceNextClosest;
	distanceClosest = distanceNextClosest = std::numeric_limits<float>::infinity();

#define FIND_CLOSESTS(POINTS,TYPE) \
	for(auto i : POINTS) \
	{ \
	float t = glm::length(i - _position); \
	if(t < distanceClosest) \
	{ \
	distanceNextClosest = distanceClosest; \
	nextClosest = closest; \
	distanceClosest = t; \
	closest = TYPE; \
	} \
		else if(t < distanceNextClosest) \
	{ \
	distanceNextClosest = t; \
	nextClosest = TYPE; \
	} \
	}

	FIND_CLOSESTS(_biom.desertPoints, Biom::Desert);
	FIND_CLOSESTS(_biom.mountainPoints, Biom::Mountain);

#undef FIND_CLOSESTS

	return std::make_pair(std::make_pair(closest, nextClosest), std::make_pair(distanceClosest, distanceNextClosest));
}

void TerrainAspect::createBiomPoints(int _ix, int _iy)
{
	BiomsContainer& b = *containers.get<BiomsContainer>(Containers::BIOMS);
	b.desertPoints.clear();
	b.mountainPoints.clear();
	b.meadowPoints.clear();
	b.swampPoints.clear();
	b.forestPoints.clear();

	float largeSize = terrainContainer.largeSize;

	std::mt19937 numberGenerator;
	std::uniform_real_distribution<float> range(0, largeSize);
	std::uniform_int_distribution<int> randomBiom(0, 8);
	
	for(int x(0); x < 5; x++)
	{
		for(int y(0); y < 5; y++)
		{
			for(int i(0); i < 2; i++)
			{
				glm::vec2 position;
				position.x = float(x + _ix - 2);
				position.y = float(y + _iy - 2);
				unsigned long t = unsigned long(position.x) + unsigned long(position.y) * 59;
				if(t == 0)
					t = 42;
				numberGenerator.seed(t);
				position *= largeSize;

				if(x == 0 && y == 0)
					std::cout << "Biom points start: " << position.x << "  " << position.y << std::endl;

				position.x += range(numberGenerator);
				position.y += range(numberGenerator);

				numberGenerator.seed(t);
				switch(randomBiom(numberGenerator))
				{
				case 0:
				case 1:
					b.desertPoints.push_back(position);
					break;
				case 2:
					b.mountainPoints.push_back(position);
					break;
				case 3:
				case 4:
					b.meadowPoints.push_back(position);
					break;
				case 5:
				case 6:
					b.swampPoints.push_back(position);
					break;
				case 7:
				case 8:
					b.forestPoints.push_back(position);
					break;
				}
			}
		}
	}
}

template < class T, class Y >
void TerrainAspect::mergeLargePerlinMaps(T& _result, Y& _mapA, Y& _mapB, Y& _mapC, Y& _mapD, uint _sizeX, uint _sizeY)
{
	auto r = [&](uint x, uint y)->float&{ return terrainContainer.heights[_result.heightsID][_sizeY*y + x]; };
	auto a = [&](uint x, uint y)->float&{ return _mapA[_sizeY*y + x]; };
	auto b = [&](uint x, uint y)->float&{ return _mapB[_sizeY*y + x]; };
	auto c = [&](uint x, uint y)->float&{ return _mapC[_sizeY*y + x]; };
	auto d = [&](uint x, uint y)->float&{ return _mapD[_sizeY*y + x]; };

	BiomsContainer& biom = *Containers(std::make_pair(Containers::BIOMS, READ)).get<BiomsContainer>(Containers::BIOMS);

	glm::vec2 startPos = glm::vec2(_result.position.x, _result.position.z) - glm::vec2(terrainContainer.large[0].position.x, terrainContainer.large[0].position.z);

	int xVal = (int)(startPos.x / terrainContainer.largeSize * BLENDTEXSIZE);
	int yVal = (int)(startPos.y / terrainContainer.largeSize * BLENDTEXSIZE);

	float div = 1.f / (256 / BLENDTEXSIZE);

	uint size = BLENDTEXSIZE * 3 + 1;
	auto valueM = [&](uint x, uint y)->float&{ return terrainContainer.newBiomMap.map1[size * y + x].y; };
	auto valueS = [&](uint x, uint y)->float&{ return terrainContainer.newBiomMap.map1[size * y + x].w; };
	auto valueF = [&](uint x, uint y)->float&{ return terrainContainer.newBiomMap.map2[size * y + x].x; };
	auto interpolate = [](float a, float b, float x)->float
	{
		float t = x; // (1 - cos(x * 3.1415927f)) * .5f;
		return a*(1 - t) + b*t;
	};
	for(uint x(0); x < _sizeX; x++)
	{
		for(uint y(0); y < _sizeY; y++)
		{
			uint intX = uint(xVal + x * div);
			float fracX = xVal + x * div - float(intX);

			uint intY = uint(yVal + y * div);
			float fracY = yVal + y * div - float(intY);

			float m = interpolate(	interpolate(valueM(intX + 0, intY + 0), valueM(intX + 1, intY + 0), fracX),
									interpolate(valueM(intX + 0, intY + 1), valueM(intX + 1, intY + 1), fracX), fracY);

			float s = interpolate(	interpolate(valueS(intX + 0, intY + 0), valueS(intX + 1, intY + 0), fracX),
									interpolate(valueS(intX + 0, intY + 1), valueS(intX + 1, intY + 1), fracX), fracY);

			float f = interpolate(	interpolate(valueF(intX + 0, intY + 0), valueF(intX + 1, intY + 0), fracX),
									interpolate(valueF(intX + 0, intY + 1), valueF(intX + 1, intY + 1), fracX), fracY);
			float val1 = interpolate(a(x, y), b(x, y), m * m);
			float val2 = interpolate(a(x, y), c(x, y) - 125, s);
			float val3 = interpolate(a(x, y), d(x, y), f);
			r(x, y) = a(x, y) * (1 - m - s - f) + val1 * m + val2 * s + val3 * f;
		}
	}
}

template < class T, class Y >
void TerrainAspect::mergePerlinMaps(T& _result, Y& _mapA, Y& _mapB, uint _sizeX, uint _sizeY, uint _level)
{
	auto r = [&](uint x, uint y)->float&{ return terrainContainer.heights.at(_result.heightsID)[_sizeY*y + x]; };
	auto a = [&](uint x, uint y)->float&{ return _mapA[_sizeY*y + x]; };
	auto b = [&](uint x, uint y)->float&{ return _mapB[_sizeY*y + x]; };

	BiomsContainer& biom = *Containers(std::make_pair(Containers::BIOMS, READ)).get<BiomsContainer>(Containers::BIOMS);

	auto interpolate = [](float a, float b, float x)->float
	{
		float t = (1 - std::cosf(x * 3.1415927f)) * .5f;
		return a*(1 - t) + b*t;
		//return a*(1 - x) + b*x;
	};
	for(uint x(0); x < _sizeX; x++)
	{
		for(uint y(0); y < _sizeY; y++)
		{
			//r(x, y) = a(x, y);
			//continue;

			// Find closest points
			glm::vec2 position(_result.position.x + x * QUADSIZE * std::powf(3, float(_level - 1)), _result.position.z + y * QUADSIZE * std::powf(3, float(_level - 1)));

			float distanceDesert = glm::length(biom.desertPoints[0] - position);
			for(uint i(1); i < biom.desertPoints.size(); i++)
			{
				float t = glm::length(biom.desertPoints[i] - position);
				if(t < distanceDesert)
				{
					distanceDesert = t;
				}
			}
			float distanceMountain = glm::length(biom.mountainPoints[0] - position);
			for(uint i(1); i < biom.mountainPoints.size(); i++)
			{
				float t = glm::length(biom.mountainPoints[i] - position);
				if(t < distanceMountain)
				{
					distanceMountain = t;
				}
			}

			auto& m = biomMapping(biom, position);

			float t = 20000.f; // pow(5000.f, 2);

			if(m.first.first == Biom::Mountain)
			{
				float length = Utility::length(m.second.first);
				if(length < t)
					r(x, y) = interpolate(a(x, y), b(x, y), (t - length) / t);
				else
					r(x, y) = a(x, y);
			}
			else
			{
				float length = Utility::length(m.second.second);
				if(length < t)
					r(x, y) = interpolate(a(x, y), b(x, y), (t - length) / t);
				else
					r(x, y) = a(x, y);
			}
		}
	}
}

TerrainAspect::~TerrainAspect()
{
	if(reCreateChunkThread.joinable())
		reCreateChunkThread.join();
}
