#include "RenderDebug.h"
#include "MainState.h"

#include "Utility_PostEffect.h"
#include "Utility.h"

RenderDebug::RenderDebug()
	: graphics(nullptr)
{
}

RenderDebug::~RenderDebug()
{
}

bool RenderDebug::init(Console* _console, MainState& _mainState)
{
	graphics = _mainState.graphics;
	textStart	= 16;
	numTexts	= 6;
	for(uint i = 0; i < numTexts; ++i)
		_console->createText(50, textStart + i);

	_console->loadText("RenderDebug", textStart, 0);
	return true;
}

bool RenderDebug::command(std::string& _command)
{
	char delimiter = ' ';
	int sep = _command.find_last_of(delimiter);
	std::string key, val;
	key = _command.substr(_command.find_first_not_of(delimiter), sep);
	val = _command.substr(sep + 1, _command.find_last_not_of(delimiter));

	printf("@Render Debug\nLOL no [%s] command!\n", _command.c_str());
	return false;
}

void RenderDebug::update(Console* _console)
{
	/* Find the major & minor */
	Containers containers( std::make_pair(Containers::BIOMS, READ),std::make_pair(Containers::PLAYER, READ),std::make_pair(Containers::TERRAIN, READ), std::make_pair(Containers::BIOMS, READ));
	PlayerContainer& pc		= *containers.get<PlayerContainer>(Containers::PLAYER);
	TerrainContainer& tc	= *containers.get<TerrainContainer>(Containers::TERRAIN);
	BiomsContainer& bc		= *containers.get<BiomsContainer>(Containers::BIOMS);
	
	glm::vec3 pos	= pc.position;
	//SingleBiom& b	= Utility::_getBlendMapValue(&tc,pos);
	SingleBiom& b	= Utility::getBlendMapValue(&bc, pos);
	glm::vec4 v		= b.map1;

	int major			= POST::getDominantBiomId(v);
	float majorVal		= v[major];

	POST::PWEATHER bw	= (POST::PWEATHER)major;
	if(majorVal == 0)
		bw = POST::PWEATHER::FORREST;

	std::string currentBiome = "Not Valid";
	switch(bw)
	{
	case POST::PWEATHER::DESERT:		// haze
			currentBiome = "Desert";
		break;
	case POST::PWEATHER::MEADOW:		// rain
			currentBiome  = "Meadow";
		break;
	case POST::PWEATHER::MOUNTAIN:	// snow
			currentBiome = "Mountain";
		break;
	case POST::PWEATHER::SWAMP:		// rain
			currentBiome = "Swamp";
		break;
	case POST::PWEATHER::FORREST:		// rain
			currentBiome = "Forrest";
		break;
	}

	
	_console->loadText(currentBiome, textStart, 0);

	currentBiome = "{" + std::to_string(v.x) + "," + std::to_string(v.y) + "," + std::to_string(v.z) + "," + std::to_string(v.w) + "}";
	_console->loadText(currentBiome, textStart + 1, 0);
	float totVRAM, memTex, memBuffer, memShader;

	memTex			= graphics->getMemTextures() / 1024.0f / 1024.0f;
	memBuffer		= ( graphics->getMemBuffers() + graphics->getMemBackBuffer() ) / 1024.0f / 1024.0f;
	memShader		= graphics->getMemShaders() / 1024.0f / 1024.0f;

	totVRAM = memTex + memBuffer + memShader;

	/* TOTAL VRAM */
	std::string text = "Total VRAM: " + std::to_string(totVRAM) + " MB";
	_console->loadText(text, textStart + 2, 0);

	/* BUFFER MEM */
	text = "Buffer Mem: " + std::to_string(memBuffer) + " MB";
	_console->loadText(text, textStart + 3, 0);

	/* TEXTURE MEM */
	text = "Texture Mem: " + std::to_string(memTex) + " MB";
	_console->loadText(text, textStart + 4, 0);

	/* SHADER MEM */
	text = "Shader Mem: " + std::to_string(memShader) + " MB";
	_console->loadText(text, textStart + 5, 0);
}

void RenderDebug::draw(Console* _console)
{
	for(uint i = 0; i < numTexts; ++i)
	{
		uint index	= i + textStart;
		uint indY	= 5 + i * 20;
		_console->draw(index, glm::vec2(5, indY), glm::vec4(0, 0, 1, 1));
	}
}