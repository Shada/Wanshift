#pragma once

#include <vector>
#include <utility>
#include <numeric>
#ifdef __DEBUG
#include <gtest\gtest.h>
#endif
#include "Container.h"

typedef unsigned int uint;

/* A simple example class to demonstrate containers and their use */
struct ExampleContainer : Container
{
	// Add data here
	std::vector< float > exampleData;

private:
	// See Container.h for documentation
	void addData(uint _index, uint _number)
	{
		insert<float>(exampleData, _index, _number, 0);
	}

	// See Container.h for documentation
	void removeData(uint _index, uint _number)
	{
		remove(exampleData, _index, _number);
	}
};

/*TEST( ExampleContainer, addAndRemoveData )
{
	ExampleContainer ec;
	uint t = 0;
	ec.
	( t, 10 );
	ASSERT_EQ( ec.exampleData.size(), 10 );
	
	ec.deallocate( t, 0, 5 );
	ASSERT_EQ( ec.exampleData.size(), 5 );

	ec.exampleData[2] = 5;
	ec.deallocate( t, 3, 2 );
	ASSERT_EQ( ec.exampleData.size(), 3 );
	ASSERT_EQ( ec.exampleData[2], 5 );
} */