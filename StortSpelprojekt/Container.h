#pragma once

#include <vector>
#include <glm\glm.hpp>
#include "ShaderClass.h"

typedef unsigned int uint;
typedef unsigned long long ull;

struct Container
{
	virtual ~Container() {}

	/* Functions to be overloaded in the containers, see ExampleContainer for example */
	/* Add a number of entities at index
	 *
	 * #_index, the index to add entities to
	 * #_number, the number of entities to add */
	virtual void addData(uint _index, uint _number) = 0;

	/* Remove a number of entities at index
	 *
	 * #_index, the index to begin removing entities at
	 * #_number, the number of entities to remove */
	virtual void removeData(uint _index, uint _number) = 0;

	/* Allocates a number of data entities in container,
	 * needed to preserve internal 1:1 mapping and speed up allocation
	 *
	 * #_type, the type identifier for the type to be added
	 * #_number, the number of entities to be allocated resources for */
	void allocate(uint _type, uint _number);

	/* Deallocates a number of data entities in container,
	 * needed to preserve internal 1:1 mapping and speed up deallocation
	 *
	 * #_type, the identifier for the type that entities will be removed from
	 * #_local_index, the index inside the type where 0 is the first element of that type
	 * #_number, the number of entities to be removed starting at and including _local_index */
	void deallocate(uint _type, uint _local_index, uint _number = 1);

	/* Checks if a type has any allocated entities in container
	 *
	 * #_type, the type to check
	 *
	 * #returns true if it does, false otherwise */
	bool containsType(uint _type);

	/* Get the global index for a type
	 *
	 * #_type, the type to get index for
	 *
	 * #returns an global index to the start of the type block, may become invalid */
	uint getTypeIndex(uint _type);

	/* Get the number of entities of a type contained in the container
	 *
	 * #_type, specifies the type to check number of entities of
	 *
	 * #return the number of entities of the type in the container */
	uint getTypeSize(uint _type);

	/* Get the type of a specific index
	 *
	 * #_index, the index to check what type it has
	 *
	 * #returns the type of the entity at index*/
	uint getType(uint _index);

	/* Small helper function for shorter vector call */
	template <class T> void insert(std::vector<T>& _v, uint _i, uint _n, T _t)
	{
		_v.insert(_v.begin() + _i, _n, _t);
	}
	/* Small helper function for shorter vector call */
	template <class T> void remove(std::vector<T>& _v, uint _i, uint _n)
	{
		_v.erase(_v.begin() + _i, _v.begin() + _i + _n);
	}

private:
	/* Maps a type to a specific block in memory,
	 * the first uint is the type identifier the next is the number of that type in the order they apper in memory */
	std::vector< std::pair< uint, uint > > type_blocks;
};
