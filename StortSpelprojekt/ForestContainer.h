#pragma once
#include "Container.h"
#include <Model.h>
struct ModelInstance
{
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;
};
struct ForestContainer :Container
{
	//mapped per biom
	std::vector< int > treeType;
	std::vector< UINT > startIDLod0;
	std::vector< UINT > startIDLod1;
	std::vector< UINT > startIDLod2;
	std::vector< UINT > startIDFoliage;
	std::vector< UINT > sizeLod0;
	std::vector< UINT > sizeLod1;
	std::vector< UINT > sizeLod2;
	std::vector< UINT > sizeFoliage;

	//shared between bioms
	int size;
	std::vector< ModelInstance >	treeList;
	std::vector< Model >			modelList;
	std::vector< int >				ActiveListLod0;
	std::vector< int >				ActiveListLod1;
	std::vector< int >				ActiveListLod2;


	void addData(uint _index, uint _number)
	{
		insert<int>(treeType,			_index, _number, 0);
		insert<UINT>(startIDLod0,		_index, _number, 0);
		insert<UINT>(startIDLod1,		_index, _number, 0);
		insert<UINT>(startIDLod2,		_index, _number, 0);
		insert<UINT>(startIDFoliage,	_index, _number, 0);
		insert<UINT>(sizeLod0,			_index, _number, 0);
		insert<UINT>(sizeLod1,			_index, _number, 0);
		insert<UINT>(sizeLod2,			_index, _number, 0);
		insert<UINT>(sizeFoliage,		_index, _number, 0);
		size = treeType.size();
	}

	// See Container.h for documentation
	void removeData(uint _index, uint _number)
	{
		remove(treeType,		_index, _number);
		remove(startIDLod0,		_index, _number);
		remove(startIDLod1,		_index, _number);
		remove(startIDLod2,		_index, _number);
		remove(startIDFoliage,	_index, _number);
		remove(sizeLod0,		_index, _number);
		remove(sizeLod1,		_index, _number);
		remove(sizeLod2,		_index, _number);
		remove(sizeFoliage,		_index, _number);
		size = treeType.size();
	}
};