#pragma once
#include "CullingNode.h"

#include "CameraContainer.h"
#include "WindContainer.h"
struct TerrainContainer;
struct BiomsContainer;

struct GrassChunk
{
	glm::vec3 position;
	float tileSize;
};

class GrassNode : public CullingNode
{
public:
	GrassNode(RenderInterface *_graphics, bool _motherNode);
	~GrassNode();

	void addNode(GrassNode *_child);
	void addChunk(GrassChunk *_chunk);
	void draw(Frustum *_frustum);

private:
	std::vector<GrassNode*> children;
	TerrainContainer *terrain;
	BiomsContainer *bioms;
	CameraContainer	*camera;
	WindContainer *wind;
	const GrassChunk *chunk;
	bool motherNode;
	static uint LOD0count, LOD1count, LOD2count;

	bool swamp; // om fler gräs typer ska läggas till tänk på möjligheten att använda enum states

	/*	updates the buffer used to determine where to put the differnet instances of grass
		and which lod it should have.
		Will put how many was placed in the different lods in the appropriate inout parameters*/
	void updateGrassPerInstanceBuffer(Frustum *_frustum);

	glm::vec3 vec3floor(glm::vec3 vector)
	{
		return glm::vec3(floor(vector.x),floor(vector.y),floor(vector.z));
	};
};

