#pragma once
#include "Container.h"

#include <map>

//struct FogSim
//{
//	glm::vec3	color;
//	float		startDepth;
//	glm::vec3	highlightColor;
//	float		globalDensity;
//	glm::vec3	sunDir;
//	float		heightFalloff;
//};

class FogContainer :
	public Container
{
public:
	FogContainer(void);
	~FogContainer(void);

	FogSim getFogType(std::string _name);

private:
	std::map<std::string, FogSim>	fogTypes;

	void addData(uint _index, uint _number) {}
	void removeData(uint _index, uint _number) {}

	void setData(std::string _file);
	
	friend class GameSettings;
};

