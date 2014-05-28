#pragma once
#include "Container.h"
#include <map>

struct RainSimSettings
{
	float		simulationSpeed;
	UINT		groupSize;
	UINT		gridSize;
	std::string	textureName;
	std::string noiseMap;
};

struct RainSimVariables
{
	float		verticalSpeed;
	float		maxWindEffect;
	float		maxWindVariance;
	float		rainDensity;
	glm::vec3	boundHalfSize;
	UINT		heightMapSize;
	float		stretch;
	glm::vec3	centerOffset;
};

struct RainSimConstants
{
	float				particleScale;	
	glm::vec4			color;
};

class RainSimContainer :
	public Container
{
public:
	RainSimContainer(void);
	~RainSimContainer(void);

	RainSimSettings		getSettings(const std::string& _name);
	RainSimVariables	getRainVariables(const std::string& _name);
	RainSimConstants	getRainConstants(const std::string& _name);

private:
	RainSimSettings					settings;
	RainSimConstants				constants;

	std::map<std::string, RainSimSettings>	weatherSettings;
	std::map<std::string, RainSimConstants>	weatherConstants;
	std::map<std::string,RainSimVariables>	weatherTypes;

	void addData(uint _index, uint _number) {}
	void removeData(uint _index, uint _number) {}
};

