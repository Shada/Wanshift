#pragma once

#include "Container.h"
#include "glm\glm.hpp"
#include "ParticleEffectContainer.h"

#define HORDESIZE (25)

enum EntityAction
{
	eaNEW = 1,
	eaIDLE,
	eaWALKING,
	eaRUNNING,
	eaGESTURE1,
	eaGESTURE2,
	eaGESTURE3,
	eaATTACK,
	eaATTACKED,
	eaJUMP,
	eaFEEDING,
	eaDIE,
};

struct EnemyContainer : Container
{
	std::vector<glm::vec3>	pos;
	std::vector<glm::vec3>	targetPos;
	std::vector<glm::vec3>	color;
	std::vector<int>		action;
	std::vector<std::pair<ull, ull>>	ids;
	
	std::vector<Particle>	hordeunits;
	std::vector<glm::vec3>	hordeunitdir1;
	std::vector<float>		hordeunitwavevar1;

	//wisp particles
	std::vector<UINT>	vBufferIndex;
	std::vector<UINT>	vertexStartIndex;
	std::vector<UINT>	vertexAmount;
	std::vector<UINT>	textureIndex;

	//extra particles for nice effects
	std::vector<UINT>	vBufferExtraIndex;
	std::vector<UINT>	vertexExtraStartIndex;
	std::vector<UINT>	vertexExtraAmount;

	void addData(uint _index, uint _number) 
	{
		glm::vec3 noll;
		insert<glm::vec3>(	pos,		_index, _number, noll);
		insert<glm::vec3>(	targetPos,	_index, _number, noll);
		insert<glm::vec3>(	color,		_index, _number, noll);
		insert<int>(		action,		_index, _number, EntityAction::eaIDLE);
		insert<std::pair<ull, ull>>(ids, _index, _number, std::pair<ull, ull>(0, 0));

		insert<Particle>(	hordeunits,			_index * HORDESIZE, _number * HORDESIZE, Particle());
		insert<glm::vec3>(	hordeunitdir1,		_index * HORDESIZE, _number * HORDESIZE, noll);
		insert<float>(		hordeunitwavevar1,	_index * HORDESIZE, _number * HORDESIZE, 0);

		insert<UINT>(	vBufferIndex,		_index, _number, 0 );
		insert<UINT>(	vertexStartIndex,	_index, _number, 0 );
		insert<UINT>(	vertexAmount,		_index, _number, 0 );
		insert<UINT>(	textureIndex,		_index, _number, 0 );

		insert<UINT>(	vBufferExtraIndex,		_index, _number, 0 );
		insert<UINT>(	vertexExtraStartIndex,	_index, _number, 0 );
		insert<UINT>(	vertexExtraAmount,		_index, _number, 0 );
	}

	// See Container.h for documentation
	void removeData(uint _index, uint _number)
	{
		remove(pos,			_index, _number);
		remove(targetPos,	_index, _number);
		remove(color,		_index, _number);
		remove(action,		_index, _number);
		remove(ids, 		_index, _number);

		remove(hordeunits,			_index * HORDESIZE, _number * HORDESIZE);
		remove(hordeunitdir1,		_index * HORDESIZE, _number * HORDESIZE);
		remove(hordeunitwavevar1,	_index * HORDESIZE, _number * HORDESIZE);

		remove(vBufferIndex,		_index, _number);
		remove(vertexStartIndex,	_index, _number);
		remove(vertexAmount,		_index, _number);
		remove(textureIndex,		_index, _number);

		remove(vBufferExtraIndex,		_index, _number);
		remove(vertexExtraStartIndex,	_index, _number);
		remove(vertexExtraAmount,		_index, _number);
	}
};
