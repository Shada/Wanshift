#pragma once

#include <array>
#include "Container.h"
#include "glm\glm.hpp"
#include "RenderInterface.h"

struct ConstantBufferContainer : Container
{
	CBOnceData cbOnce;
	CBTerrainInfo cbTerrain;
	CBLightInfoData cbLight;
	CBEveryFrame	cbEveryFrame;

	UINT energyLightCount;
	UINT wispLightCount;
	ConstantBufferContainer()
	{
		energyLightCount = 20;
		wispLightCount = 0;
		cbLight.lightCount = 25;
	}
private:
	void addData(uint _index, uint _number) override {}
	void removeData(uint _index, uint _number) override {}
};