#pragma once
#include <Model.h>

class ModelLoaderInterface
{
public:
	ModelLoaderInterface();
	virtual ~ModelLoaderInterface();

	virtual float *loadFBX(const char* _filepath, Model *_model) = 0; // will remake so it returns the data directly nullptr if unsuccessfull
};