#pragma once

#include "Containers.h"

/* A wrapper around a function to store access information */
struct InfoFunction
{
	void(*func)(Containers&);
	InfoFunction(void(*_func)(Containers&), Containers _containers)
		: func(_func), containers(_containers)
	{}

	void operator()()
	{
		func(containers);
	}

	Containers containers;
};