#include "BaseBiom.h"
#include "ModelLoaderFactory.h"

BaseBiom::BaseBiom() : range(0,10000)
{
	basePos = glm::vec3(-10000,0,0);
}

void BaseBiom::init(RenderInterface *_graphics)
{
	//todo: load mesh and such

	Containers containers(std::make_pair(Containers::FOREST, MODIFY),std::make_pair(Containers::TERRAIN, READ));

	graphics			= _graphics;
	terrainContainer	= containers.get<TerrainContainer>(Containers::TERRAIN);
	forestContainer		= containers.get<ForestContainer>(Containers::FOREST);

	forestContainer->allocate(0, 1);
	containerIndex = forestContainer->size-1;

	treeTech = new ShaderClass( "modelInstancingVS", "", "", "", "modelPS", graphics,(int)LayoutTypes::LAYOUT_MODEL_INSTANCED, false );
	surfaceTech = new ShaderClass( "surfaceInstancingVS", "", "", "", "surfacePS", graphics, (int)LayoutTypes::LAYOUT_SURFACE_INSTANCED, false );

}

void BaseBiom::createBuffers(std::vector<ModelInstance>& temp)
{
	instancingIndexTree = graphics->createVBufferImmutable((void**)&temp[0], temp.size(), sizeof(temp.at(0) ) );
}

BaseBiom::~BaseBiom()
{
	SAFE_DELETE(treeTech);
}
