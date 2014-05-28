#pragma once
#include "Container.h"
#include "ShaderClass.h"

/* A simple example class to demonstrate containers and their use */

struct shaderData
{
	std::vector< int > bufferIndex;
	std::vector< ShaderClass* > shaders;
	std::vector< int > renderTargetViews;
};

struct ParticleContainer : Container
{
public:
	// Add data here

	std::vector< shaderData* > particleShaders;

	void addShader()
	{
		particleShaders.push_back(new shaderData());
	}

	shaderData* getShader(int _index)
	{
		return particleShaders.at(_index);
	}


private:
	// See Container.h for documentation
	void addData(uint _index, uint _number) {}

	// See Container.h for documentation
	void removeData(uint _index, uint _number) {}
};