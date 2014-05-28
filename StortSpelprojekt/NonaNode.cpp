#include "NonaNode.h"
#include "Containers.h"
#include "Utility.h"

Containers *containers;
PlayerContainer						*player;
TerrainContainer					*terrain;
BiomsContainer						*bioms;

NonaNode::NonaNode(RenderInterface *_graphics, std::vector<int> *_tesselationchunks)
	:CullingNode(_graphics)
{
	this->tesselationchunks = _tesselationchunks;

	containers = new Containers(std::make_pair(Containers::PLAYER, READ), std::make_pair(Containers::TERRAIN, READ));
	player = containers->get<PlayerContainer>(Containers::PLAYER);
	terrain = containers->get<TerrainContainer>(Containers::TERRAIN);
	bioms = containers->get<BiomsContainer>(Containers::BIOMS);
}

void NonaNode::addNode(NonaNode *_child)
{
	children.push_back(_child);
}

void NonaNode::addChunk(Chunk<CHUNKSIZE, CHUNKSIZE> *_chunk)
{
	this->chunk = _chunk;
	this->min = _chunk->position;//														      no! ---v
	this->max = glm::vec3(_chunk->position.x + (_chunk->size_x-1)*pow(3,chunk->level-1)*QUADSIZE, 10000 ,_chunk->position.z + (_chunk->size_y-1)*pow(3,chunk->level-1)*QUADSIZE);
}//																								     ^--- stop this madness. Only you can do it, future me!

void NonaNode::draw(Frustum *_frustum)
{
	if(children.size() == 0)
	{
		int splitting;
		int quadSize;
		int playerLocalX;
		int playerLocalZ;
		if(chunk->level == 1) // small chunk
		{
			splitting = TINYSPLITTING;
			quadSize = QUADSIZE;
			playerLocalX = (int)(player->position.x - chunk->position.x);
			playerLocalZ = (int)(player->position.z - chunk->position.z);
		}
		else if(chunk->level == 2)
		{
			splitting = MEDIUMSPLITTING;
			quadSize = QUADSIZE*3;
			playerLocalX = (int)(player->playerCullingPos.x - chunk->position.x);
			playerLocalZ = (int)(player->playerCullingPos.z - chunk->position.z);
		}
		else
		{
			splitting = LARGESPLITTING;
			quadSize = QUADSIZE*9;
			playerLocalX = (int)(player->playerCullingPos.x - chunk->position.x);
			playerLocalZ = (int)(player->playerCullingPos.z - chunk->position.z);
		}
		//int activeTessChunk;
		
		int tempX = quadSize * (chunk->size_x-1) / splitting;
		int tempZ = quadSize * (chunk->size_y-1) / splitting;
		int miniChunkIndexSize = chunk->iBufferAmount/(splitting*splitting);

		int playerSubChunkX;
		int playerSubChunkZ;

		int temp1 = 0, temp2 = 0;

		if(playerLocalX < 0)
			temp1 = 1;

		playerSubChunkX = playerLocalX/tempX - temp1;
		if(playerLocalZ < 0)
			temp2 = 1;

		playerSubChunkZ = playerLocalZ/tempZ - temp2;

		if(chunk->iBufferAmount < 0 && !((playerSubChunkX > -1 - temp1 && playerSubChunkX < splitting + 1) && (playerSubChunkZ > -1 - temp2 && playerSubChunkZ < splitting + 1)))
			graphics->drawIndexed(chunk->iBufferAmount, chunk->iBufferStartID, 0);
		else
		{
			std::vector<int> ignores;
			glm::vec3 terrPos = chunk->position;
			//glm::vec3 startPos = terrain->large[0].position;
			//float largeSize = (terrain->largeSize - QUADSIZE * 9) * 3;
			//float tinySize = terrain->tinySize - QUADSIZE;
			//int blendSize = BLENDTEXSIZE * 3 + 1;
			for(int y = -1; y < 2; y++)
				for(int x = -1; x < 2; x++)
				{
					if(playerSubChunkX + x >= 0 && playerSubChunkX + x < splitting && playerSubChunkZ + y >= 0 && playerSubChunkZ + y < splitting)
					{
						if(chunk->level == 1)
						{
							glm::vec3 pos = glm::vec3(terrPos.x + (float)(playerSubChunkX + x) / splitting, 0, terrPos.z + ((float)(playerSubChunkZ + y) / splitting));
							SingleBiom blend = Utility::getBlendMapValue(bioms, pos);

							if(blend.map1.x > 0.1f)
								ignores.push_back((playerSubChunkZ + y) * splitting + playerSubChunkX + x);
						}
						else
							ignores.push_back((playerSubChunkZ + y) * splitting + playerSubChunkX + x);
					}
				}
			int startIndex = 0;
			int amount;
			if(chunk->level == 1)
				for(uint i = 0; i < ignores.size(); i++)
				{
					tesselationchunks->push_back(ignores.at(i) * miniChunkIndexSize + chunk->iBufferStartID);
				}
			for(uint i = 0; i < ignores.size(); i++)
			{
				amount = ignores.at(i) * miniChunkIndexSize - startIndex;
				if(amount > 0)
					graphics->drawIndexed(amount, chunk->iBufferStartID + startIndex, 0);
				startIndex = (ignores.at(i)+1) * miniChunkIndexSize;

			}
			if(chunk->iBufferAmount - startIndex > 0 && ignores.size() == 0)
			{
				graphics->drawIndexed(chunk->iBufferAmount - startIndex, chunk->iBufferStartID, 0);
				return;
			}
			startIndex = (ignores.at(ignores.size()-1)+1) * miniChunkIndexSize;
			amount = chunk->iBufferAmount - startIndex;
			if(amount > 0)
				graphics->drawIndexed(amount, chunk->iBufferStartID + startIndex, 0);
		}
		
	}
	else
		for(uint i = 0; i < children.size(); i++)
		{
			if(children.at(i)->hitTestFrustum(_frustum))
				children.at(i)->draw(_frustum);
		}
}

int NonaNode::getChunkVStartIndex()
{
	return chunk->bufferStartID;
}
int NonaNode::getChunkVSize()
{
	return chunk->bufferAmount;
}

NonaNode::~NonaNode()
{
}
