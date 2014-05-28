#pragma once

#include "Container.h"
#include <map>

struct AtmosphereQuality
{
	UINT	nSamples;
	float	scaleDepth;
	float	scaleOverDepth;
	float	padding;
};

struct AtmosphereWorld
{
	float innerRadius;
	float outerRadius;
	float reflectance;
	float brightness;
};

struct ScatterType
{
	float		kr4PI;
	float		km4PI;
	float		krEsun;
	float		kmEsun;
	float		gM;
	glm::vec3	wM;
	float		exposure;
	glm::vec3	wR;
};

class AtmosphereContainer :
	public Container
{
public:
	AtmosphereContainer(void);
	~AtmosphereContainer(void);

	AtmosphereQuality	getAtmosphereQuality();
	AtmosphereWorld		getAtmosphereWorld();
	ScatterType			getScatter(UINT _index);
	ScatterType			getScatter(std::string _name);

private:

	AtmosphereQuality			atmosphereQuality;
	AtmosphereWorld				atmosphereWorld;
	std::map<std::string,ScatterType>	scatterTypes;

	void setData(std::string _file);

	void addData(uint _index, uint _number) {}
	void removeData(uint _index, uint _number) {}

	friend class GameSettings;
};

