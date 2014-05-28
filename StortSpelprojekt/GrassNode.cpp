#include "GrassNode.h"
#include "Containers.h"
#include "TerrainContainer.h"
#include "Utility.h"

uint GrassNode::LOD0count = 0;
uint GrassNode::LOD1count = 0;
uint GrassNode::LOD2count = 0;

//TerrainContainer *terrain;

GrassNode::GrassNode(RenderInterface *_graphics, bool _motherNode)
:CullingNode(_graphics)
{
	Containers c(std::make_pair(Containers::TERRAIN, READ), std::make_pair(Containers::TERRAIN, READ), std::make_pair(Containers::WIND, READ), std::make_pair(Containers::BIOMS, READ));
	terrain = c.get<TerrainContainer>(Containers::TERRAIN);
	bioms = c.get<BiomsContainer>(Containers::BIOMS);
	camera = c.get<CameraContainer>(Containers::CAMERA);
	wind = c.get<WindContainer>(Containers::WIND);
	motherNode = _motherNode;
	if(_motherNode)
	{
		for(int i = 0; i<25; i++)
			children.push_back( new GrassNode(_graphics,false));
	}
}
void GrassNode::draw(Frustum *_frustum)
{
	if(motherNode)
	{
		//find out where to put grasschunks
		updateGrassPerInstanceBuffer(_frustum);
		//draw the positions still left to be drawn
		if(LOD0count != 0 || LOD1count != 0 || LOD2count != 0)
		{
			graphics->setCBuffers(5, 1, terrain->cbTerrInfo);
			graphics->setCBuffers(6, 1, terrain->cbShifting);

			graphics->setInputLayout((int)LayoutTypes::LAYOUT_GRASS);
			graphics->setRasterState(2);
			graphics->setPixelShaderResource(1, terrain->grassTextureID, 1);
			graphics->setGeometryShaderResource(0, graphics->getHandleIndex("seed", ResourceType::ShaderResourceView), 1);
			for(uint i = 0; i < terrain->tiny.size(); i++)
				graphics->setGeometryShaderResource(i+1, terrain->tiny[i].heightmapShaderResource, 1);
			//graphics->setGeometryShaderResource(1,terrain->localHeightMapResource,1);
			graphics->setGeometryShaderResource(9, terrain->tiny[0].grassBlendShaderResource, 1);
			graphics->setGeometryShaderResource(10, terrain->biomMapID1, 1);
			graphics->setGeometryShaderResource(11, terrain->biomMapID2, 1);
			graphics->setGeometryShaderResource(12, wind->windMapShaderResource, 1);

			UINT strides[2];
			strides[0] = sizeof(grassVertex);
			strides[1] = sizeof(grassVertexPerInstance);
			UINT offsets[2];
			offsets[0] = 0;
			offsets[1] = 0;

			graphics->setVertexBuffers(0, terrain->grassBufferID, 1, &strides[0], &offsets[0]);
			graphics->setVertexBuffers(1, terrain->grassBufferPerInstance, 1, &strides[1], &offsets[1]);
		}
		if(LOD0count != 0)
		{
			if (swamp)
			{
				terrain->swampGrassGeometryLOD0->use();
				graphics->drawInstanced(80 * 80, LOD0count, 0, 0);
			}
			else
			{
				terrain->grassGeometryLOD0->use();
				graphics->drawInstanced(175 * 175, LOD0count, 0, 0);
			}
		}
		if(LOD1count != 0)
		{
			if (swamp)
			{
				terrain->swampGrassGeometryLOD1->use();
				graphics->drawInstanced(40 * 40, LOD1count, 0, LOD0count);
			}
			else
			{
				terrain->grassGeometryLOD1->use(); 
				graphics->drawInstanced(150 * 150, LOD1count, 0, LOD0count);
			}
		}
		if(LOD2count != 0)
		{
			if (swamp)
			{
				terrain->swampGrassGeometryLOD2->use();
				graphics->drawInstanced(20 * 20, LOD2count, 0, LOD0count + LOD1count);
			}
			else
			{
				terrain->grassGeometryLOD2->use();
				graphics->drawInstanced(125 * 125, LOD2count, 0, LOD0count + LOD1count); //129 // 198
			}
		}
	}

}
void GrassNode::updateGrassPerInstanceBuffer(Frustum *_frustum)
{
	std::vector<glm::vec3> potentialPositions = std::vector<glm::vec3>();
	glm::vec3 terrainPos = (glm::vec3(camera->vEye) - glm::vec3(1275, 0, 1275)) / (float)terrain->grassPatchTileSize;

	terrainPos = vec3floor(terrainPos) * (float)terrain->grassPatchTileSize;
	terrainPos.y = 0;

	std::vector<glm::vec4> &biomPoints	= terrain->biomMap.map1;
	std::vector<glm::vec4> &biomPoints2 = terrain->biomMap.map2;

	int index, blendSize = BLENDTEXSIZE * 3 + 1;
	float dist, biomStep = 9 * (CHUNKSIZE / BLENDTEXSIZE) * QUADSIZE;

	glm::vec3 instPos;
	bool last;
	swamp = false;

	glm::vec2 startPos = glm::vec2(terrain->large[0].position.x, terrain->large[0].position.z);
	for(uint y = 0; y < 10; y++)
	{
		for(uint x = 0; x < 10; x++)
		{
			instPos = terrainPos + glm::vec3(x * terrain->grassPatchTileSize, 0, y * terrain->grassPatchTileSize);
			SingleBiom b = Utility::getBlendMapValue(bioms, instPos);

			if(b.map1.z < 0.4f && b.map1.w < 0.4f && b.map2.x < 0.4f)
				continue;
			if(b.map1.w > 0.4f)
				swamp = true;

			//index for the height in the middle of this chunk of grass
			//index = (int)((y * terrain->grassPatchTileSize + terrain->grassPatchTileSize * 0.5f) / QUADSIZE) * CHUNKSIZE +
			//		(int)((x * terrain->grassPatchTileSize + terrain->grassPatchTileSize * 0.5f) / QUADSIZE);
			//instPos.y = terrainPos.y + terrain->heights[terrain->tiny[4].heightsID][index];

			instPos.y = Utility::mapToTerrain(glm::vec2(instPos.x, instPos.z), *terrain).y;

			if(potentialPositions.size() > 0)
			{
				last = true;
				for(uint j = 0; j < potentialPositions.size(); j++)
				{
					dist = glm::length(glm::vec3(camera->vEye) - (instPos + glm::vec3(terrain->grassPatchTileSize * 0.5f, 0, terrain->grassPatchTileSize * 0.5f)));
					if(dist < glm::length(glm::vec3(camera->vEye) - (potentialPositions.at(j) + glm::vec3(terrain->grassPatchTileSize * 0.5f, 0, terrain->grassPatchTileSize * 0.5f))))
					{
						potentialPositions.emplace(potentialPositions.begin() + j, instPos);
						last = false;
						break;
					}
				}
				if(last)
					potentialPositions.push_back(instPos);
			}
			else
				potentialPositions.push_back(instPos);
		}
	}

	LOD0count = 0;
	LOD1count = 0;
	LOD2count = 0;
	uint totalCount = 0;
	//potentialPositions need to be sorted for this to work
	for( int y = 0; y < std::min(25, (int)potentialPositions.size()); y++)
	{
		GrassChunk gChunk;
		gChunk.position = potentialPositions.at(y);
		gChunk.tileSize = 510.0f;
		children.at(y)->addChunk(&gChunk);

		if(!children.at(y)->hitTestFrustum(_frustum))
			continue;
		glm::vec3 instPos = potentialPositions.at(y);//glm::vec3(x * terrain->grassPatchTileSize, 0, y * terrain->grassPatchTileSize);
		float dist = glm::length(glm::vec3(camera->vEye) - (instPos + glm::vec3( terrain->grassPatchTileSize / 2, 0, terrain->grassPatchTileSize / 2)));
		if(dist> 2250)
			break; //if you get to a grasschunk that is this far away there is no need to continue.
		if(dist > 900)
		{
			terrain->grassChunks.at(totalCount).pos = instPos;
			totalCount++;
			LOD2count++;
		}
		else if(dist > 450)
		{
			terrain->grassChunks.at(totalCount).pos = instPos;
			totalCount++;
			LOD1count++;
		}
		else 
		{
			terrain->grassChunks.at(totalCount).pos = instPos;
			totalCount++;
			LOD0count++;
		}
	}
	graphics->updateVBuffer(terrain->grassBufferPerInstance, (void**)&terrain->grassChunks[0], totalCount, sizeof(terrain->grassChunks[0]));
}
void GrassNode::addNode(GrassNode *_child)
{
	children.push_back(_child);
}

void GrassNode::addChunk(GrassChunk *_chunk)
{
	chunk = _chunk;
	min = glm::vec3( _chunk->position.x, -1000,_chunk->position.z );
	max = glm::vec3(_chunk->position.x + _chunk->tileSize,2000 /* should be max height grass can be on */ ,_chunk->position.z + _chunk->tileSize);
}

GrassNode::~GrassNode()
{
	for(uint i = 0; i < children.size(); i++)
		SAFE_DELETE(children.at(i));
	if(motherNode)
		SAFE_DELETE(terrain->grass);
}
