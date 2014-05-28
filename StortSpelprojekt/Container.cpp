#include "Container.h"

void Container::allocate(uint _type, uint _number)
{
	uint data_index = 0;
	for(uint i(0); i < type_blocks.size(); i++)
	{
		data_index += type_blocks[i].second;
		if(type_blocks[i].first == _type)
		{
			addData(data_index, _number);
			return;
		}
	}

	// No type found, add new type
	type_blocks.push_back(std::make_pair(_type, _number));
	addData(data_index, _number);
};

void Container::deallocate(uint _type, uint _local_index, uint _number)
{
	uint data_index = 0;
	for(uint i(0); i < type_blocks.size(); i++)
	if(type_blocks[i].first == _type)
	{
		removeData(data_index + _local_index, _number);
		type_blocks[i].second -= _number;
		if(type_blocks[i].second <= 0)
			type_blocks.erase(type_blocks.begin() + i);
		if(type_blocks[i].second < 0)
			0 == 0; // TODO: Log error
		return;
	}
	else
		data_index += type_blocks[i].second;

	// Type not found
	; // TODO: Log error

	return;
};

/* Checks if a type has any allocated entities in container
*
* #_type, the type to check
* 
* #returns true if it does, false otherwise */
bool Container::containsType(uint _type)
{
	for(std::pair<uint, uint> p : type_blocks)
	if(p.first == _type)
		return true;
	return false;
}

uint Container::getTypeIndex(uint _type)
{
	uint data_index = 0;
	for(uint i(0); i < type_blocks.size(); i++)
	if(type_blocks[i].first == _type)
		break;
	else
		data_index += type_blocks[i].second;
	return data_index;
}

uint Container::getTypeSize(uint _type)
{
	for(std::pair<uint, uint> p : type_blocks)
	if(p.first == _type)
		return p.second;
	return 0;
}

uint Container::getType(uint _index)
{
	for(uint i(0), data_index(0); i < type_blocks.size(); i++)
	{
		data_index += type_blocks[i].second;
		if(data_index > _index)
			return type_blocks[i].first;
	}
	return 0;
}
