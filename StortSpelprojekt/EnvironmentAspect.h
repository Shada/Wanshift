#pragma once

#include <thread>
#include <random>
#include <functional>
#include "AspectInterface.h"
#include "RenderFactory.h"
#include "glm/gtx/norm.hpp"
#include "Containers.h"
#include "Utility.h"

std::pair< std::pair<char,char>, std::pair<float,float> > biomMapping(BiomsContainer& _biom, glm::vec2 _position);

class SandStorm;
class EnvironmentAspect : public AspectInterface
{
public:
	EnvironmentAspect();
	~EnvironmentAspect();

	void runAspect(float _dt) override;

	void init(MainState& _mainState)override;

private:

	//for grass effect
	/*	will generate the buffer containing the grass root positions
		also sets things for instancing of the grass*/
	void generateGrassBuffer();

	//end of for grass effect

	void UpdateLocalWind(Containers& _c, float _dt);
	void UpdateGlobalWind(Containers& _c);
	void UpdateClouds(Containers& _c, float _dt);
	void UpdateStorm(Containers& _c, float _dt, float desert);

	template < class T, class Y >
	void GenerateSandMap(T& _map, Y& _randomMap1, Y& _randomMap2, float _offset);
	void UpdateSand(float _dt);
	void InitializeDetailLayer();

	std::mt19937 numberGenerator;
	//	std::default_random_engine numberGenerator;
	std::uniform_real_distribution<float> range;

	Containers containers;
	TerrainContainer& terrainContainer;
	PlayerContainer &playerContainer;
	static RenderInterface *graphics;

#ifdef __DEBUG
	FRIEND_TEST(InitializeDetailLayer, detailMap);
	FRIEND_TEST(runAspect, correctChunkGeneration);
#endif
};
