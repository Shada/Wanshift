#pragma once

#include <thread>
#include <random>
#include <functional>
#include "AspectInterface.h"
#include "RenderFactory.h"
#include "RenderInterface.h"
#include "glm/gtx/norm.hpp"
#include "Utility.h"
#include "NonaNode.h"

std::pair< std::pair<char,char>, std::pair<float,float> > biomMapping(BiomsContainer& _biom, glm::vec2 _position);

class TerrainAspect : public AspectInterface
{
public:
	TerrainAspect();
	~TerrainAspect();

	void runAspect(float _dt) override;

	void init(MainState& _mainState)override;

	void createTerrainBuffer();
	void createTerrainFromPosition(glm::vec2 _spawnPos);
	void createTinyFromPosition();
	void calculateCullingFromSpawn(glm::vec2 _spawnPos);

private:

	std::vector<DefaultVertex> buffer;
	std::vector<int> indexBuffer;

	/* Send in a reference to the chunk that is to be generated
	#_terrain: the chunk
	#_seed: the seed of life
	*/
	void createChunkMap(Chunk< CHUNKSIZE, CHUNKSIZE > &_terrain, ulong _seed);

	/*update the culling tree nodes for the terrain*/
	void updateCullingNodes();
		
	/* Send in a reference to the chunk that is to be generated
	#_terrain: the chunk
	#_seed: the seed of life
	*/
	template < uint X, uint Y >
	void createTerrainChunk(Chunk< X, Y > &_terrain);

	template < uint X, uint Y >
	void createTerrainNormals(Chunk< X, Y > &_terrain);

	template < class T, class Y >
	void mergePerlinMaps(T& _result, Y& _mapA, Y& _mapB, uint _sizeX, uint _sizeY, uint _level);

	template < class T, class Y >
	void mergeLargePerlinMaps(T& _result, Y& _mapA, Y& _mapB, Y& _mapC, Y& _mapD, uint _sizeX, uint _sizeY);

	void recreateChunks();

	// Offset the values where 2 different sized chunks meet
	void correctEdges();

	void calculateBufferOffsets();

	//std::default_random_engine numberGenerator;
	std::uniform_real_distribution<float> range;

	/*Throws exception if 2 chunks that are not adjecent are sent in*/
	template < uint X, uint Y >
	void stitchChunks(Chunk< X, Y > &_chunk1, Chunk< X, Y > &_chunk2, int _offset1, int _offset2);

	void createChunks(std::array< Chunk< CHUNKSIZE, CHUNKSIZE >, 9 > &_chunks);

	template < uint X, uint Y >
	void stitchAllChunks(std::array<Chunk<X, Y>, 9> *_chunks);

	template < uint X, uint Y >
	void setHeightsID(std::array<Chunk<X, Y>, 9> *_chunks, float _size);

	int getNewChunk(std::array<Chunk<CHUNKSIZE, CHUNKSIZE>, 9> *_chunks, float _size);

	void initBiomShift();
	void interpolateBiomShift();

	void createBiomMap(bool _shifting = false);
	void createTextureFromBiomMap(int _texID, int _shaderResourceID, int _biomTexID, int _biomID, std::vector<glm::vec4> *_biomMap);

	void createBiomPoints(int _ix, int _iy);

	Containers containers;
	TerrainContainer &terrainContainer;
	PlayerContainer &playerContainer;
	ConstantBufferContainer &cbuffers;
	static RenderInterface *graphics;

	std::vector<glm::vec2> newDesertPoints, newMeadowPoints;
	std::vector<glm::vec4> biomInterpolationStep, newBiomVector;
	int newBiomMapTextureID, newBiomMapID;
	bool shiftingInProgress;
	float shiftingUpdateFreq, elapsedTime, timeToNextShift, shiftingDuration;

	std::thread reCreateChunkThread;
	bool chunkGenerationLock,  grassGrowing;

	uint playerChunkID, tempIBufferID, tempVBufferID, tempNVerts;

#ifdef __DEBUG
	FRIEND_TEST(InitializeDetailLayer, detailMap);
	FRIEND_TEST(runAspect, correctChunkGeneration);
#endif
};
