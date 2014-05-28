#pragma once

#include <array>
#include "Container.h"
#include "NonaNode.h"
#include "GrassNode.h"

#define NBIOMS 5
#define BLENDTEXSIZE 64
typedef unsigned long ulong;

struct BiomMap
{
	// Desert, mountain, meadow and swamp, in that order!
	std::vector<glm::vec4> map1;
	// Foreset and potential others
	std::vector<glm::vec4> map2;
};
struct SingleBiom
{
	glm::vec4 map1, map2;
	void operator+=(SingleBiom sb){ map1 += sb.map1; map2 += sb.map2; }
	float maxBiomValue()
	{
		return std::max(std::max(std::max(map1.x, map1.y), std::max(map1.z, map1.w)), std::max(std::max(map2.x, map2.y), std::max(map2.z, map2.w)));
	}
	int dominantBiom()
	{
		int ret = 0;
		float maxVal = maxBiomValue();
		for(int i = 0; i < 4; i++)
		{
			if(maxVal == map1[i]) ret = i;
			if(maxVal == map2[i]) ret = i + 4;
		}
		return ret;
	}
};

struct TerrainContainer : Container
{
public:
	float tinySize;
	/*the tiny chunks and the tiny active chunks.
	  Active chunks are the chunks used by culling, and remains as the old chunks until the new ones are fully loaded*/
	std::array< Chunk<CHUNKSIZE, CHUNKSIZE>, 9> tiny, tinyActive;
	float mediumSize;
	std::array< Chunk<CHUNKSIZE, CHUNKSIZE>, 9> medium, mediumActive;
	float largeSize;
	std::array< Chunk<CHUNKSIZE, CHUNKSIZE>, 9> large, largeActive;
	std::array< std::array<float, CHUNKSIZE * CHUNKSIZE >, 27> heights;
	std::array< std::array< uint, 7 >, 7 > chunkSeeds;

	std::vector<NonaNode> smallNodes;
	std::vector<NonaNode> mediumNodes;
	std::vector<NonaNode> largeNodes;

	GrassNode *grass;

	std::vector<int> tesselationchunks;
	std::vector<grassVertexPerInstance> grassChunks;
	uint grassPatchTileSize;
	std::array<int, 9> grassTextureIDs;

	BiomMap biomMap, newBiomMap, shiftingBiomMap;

	// Recieve this from the server and initiate a shifting on bioms
	float duration;
	bool biomShifting;

	int cbTerrInfo, cbShifting;
	int grassBufferID, grassTextureID;
	int grassBufferPerInstance;
	ShaderClass *grassTessellation;
	ShaderClass *grassGeometryLOD0;
	ShaderClass *grassGeometryLOD1;
	ShaderClass *grassGeometryLOD2;
	ShaderClass *grassGeometryLOD3;

	ShaderClass *swampGrassGeometryLOD0;
	ShaderClass *swampGrassGeometryLOD1;
	ShaderClass *swampGrassGeometryLOD2;

	int nVertices;
	int nVerticesPerTiny;
	int bufferID, indexBufferID, biomMapTextureID1, biomMapTextureID2, biomMapID1, biomMapID2;

	ulong worldSeed;

	void addData(uint _index, uint _number) override {}
	void removeData(uint _index, uint _number) override {}
};
