#pragma once
#include "CullingNode.h"
//#include "PlayerContainer.h"
//#include "TerrainContainer.h"

#define QUADSIZE 20
#define CHUNKSIZE 256
#define TINYSPLITTING 15	//the amount of sub-chunks per axis in a small chunk	(eg, 15 means there are 15x15 sub-chunks in the chunk)
#define MEDIUMSPLITTING 3	//the amount of sub-chunks per axis in a medium chunk	(eg, 3 means there are 3x3 sub-chunks in the chunk)
#define LARGESPLITTING 3	//the amount of sub-chunks per axis in a large chunk	(eg, 3 means there are 3x3 sub-chunks in the chunk)

typedef unsigned int uint;

//a chunk of the terrain
template < uint X, uint Y >
struct Chunk
{
	static const uint size_x = X;	//width of the chunk
	static const uint size_y = Y;	//height of the chunk
	static const uint size = X * Y; //overall size of the chunk
	glm::vec3 position;				//position of the chunk in the top left corner

	uint bufferStartID;				//the start index in the vertex buffer
	uint bufferAmount;				//the vertex amount for this chunk
	uint iBufferStartID;			//the start index in the index buffer
	uint iBufferAmount;				//the index amount for this chunk
	
	uint heightsID;
	int heightmapShaderResource;
	int grassBlendShaderResource;
	/* 1: small, 2: medium, 3:large */
	int level;
};

class NonaNode
	:public CullingNode
{
private:

	const Chunk<CHUNKSIZE, CHUNKSIZE>	*chunk;
public:
	std::vector<int> *tesselationchunks;
	//child nodes
	std::vector<NonaNode*> children;
	/* constructor
	#_graphics is a pointer to the renderer*/
	NonaNode(RenderInterface *_graphics, std::vector<int> *_tesselationchunks);
	/* detects children or draws stuff
	#_frustum is a pointer to the generated frustum from the camera*/
	void draw(Frustum *_frustum);
	/* adds a child node
	#_child is the node to be added*/
	void addNode(NonaNode *_child);
	/* adds a chunk of terrain to the node
	#_chunk is the chunk to be added to this node */
	void addChunk(Chunk<CHUNKSIZE, CHUNKSIZE> *_chunk);
	/*returns the index in question */
	int getChunkVStartIndex();
	/*returns the chunk buffer amount */
	int getChunkVSize();
	~NonaNode();
};

