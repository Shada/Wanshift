#pragma once

#ifdef __DEBUG
#include <gtest/gtest.h>
#endif

#include <vector>
#include <map>
#include "Object2DContainer.h"
#include "TextContainer.h"
#include "FontContainer.h"
#include "TerrainContainer.h"
#include "CameraContainer.h"
#include "PlayerContainer.h"
#include "DetailLayerContainer.h"
#include "ParticleContainer.h"
#include "SandStormContainer.h"
#include "WindContainer.h"
#include "RainSimContainer.h"
#include "AtmosphereContainer.h"
#include "BiomsContainer.h"
#include "ConstantBufferContainer.h"
#include "EnemyContainer.h"
#include "ForestContainer.h"
#include "FogContainer.h"
#include "ParticleEffectContainer.h"

/* Different access modes to containers */
enum Mode { READ, MODIFY, NONE };

/* The class that all containers are accessed through
 * How to add a new container:
 *		Follow the ExampleContainer.h style
 *		Add include in Containers.h
 *		Add a new type in enum Type
 *		Add a new call to AddToMap in InitializeMap implemented in Containers.cpp */
class Containers
{
public:
	/* An enum containing all the types of the containers */
	enum Type
	{
		EXAMPLE,
		TERRAIN,
		OBJECT2D,
		FONT,
		TEXT,
		CAMERA,
		PLAYER,
		PARTICLE,
		SANDSTORM,
		DETAIL_LAYER,
		DEBUG,
		WIND,
		RAIN_SIM,
		ATMOSPHERE_SIM,
		BIOMS,
		CONSTBUFFER,
		ENEMY,
		FOREST,
		FOG_SIM,
		PARTICLE_EFFECT
	};

	/* Allot of Container constructors to get around no access to Initialize_list */
	typedef std::pair< Type, Mode > pair;
	Containers(pair o1);
	Containers(pair o1, pair o2);
	Containers(pair o1, pair o2, pair o3);
	Containers(pair o1, pair o2, pair o3, pair o4);
	Containers(pair o1, pair o2, pair o3, pair o4, pair o5);
	Containers(pair o1, pair o2, pair o3, pair o4, pair o5, pair o6);

	/* Container constructor calls this to add all arguments to access list
	 *
	 * #list, a complete list of all containers that will be accessed */
	void initializeContainers(std::vector< pair > list);

	/* Return a pointer to requested container, no access check is currently performed but will be added in the future
	 *
	 * #t, the type of the component to be returned as per defined in the enum Type
	 *
	 * #returns a pointer to the component requested */
	template < class T >
	T* get(Type t)
	{
		return (T*)map[t];
	}

	/* Is called to initialize static map with correct values in .cpp file */
	typedef std::map< Type, void* > map_type;
	static map_type initializeMap();

	std::vector< pair > access;
private:
	static map_type map;
	/* Simplifies adding the pair t,v to map */
	static void addToMap(map_type& _map, Type _t, void* _v)
	{
		_map.insert(std::make_pair(_t, _v));
	}
};
