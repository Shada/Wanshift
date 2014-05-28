#include "SRVDebug.h"
#include "MainState.h"


SRVDebug::SRVDebug()
: DebugDomain()
{
}


SRVDebug::~SRVDebug()
{
}

bool SRVDebug::init(Console* _console, MainState& _mainState)
{
	graphics = _console->graphics;

	shaders = new ShaderClass("DebugVS", "DebugGS", "", "", "DebugPS", graphics, (int)LayoutTypes::LAYOUT_DEFAULT, false);

	mainState = &_mainState;

	input.pos = glm::vec3(0,0,0);
	input.normal = glm::vec3(0, 0, 0);
	input.texCoord = glm::vec2(0, 0);
	
	
	textStart = 10;
	numTexts = 6;

	for (unsigned int i = 0; i < numTexts; ++i)
		_console->createText(30, textStart + i);


	bufferIndex = graphics->createVBufferDynamic(1, sizeof(DefaultVertex));
	graphics->updateVBuffer(bufferIndex, (void**)&input, 1, sizeof(DefaultVertex));

	std::string t = "show shadowcompile"; // shadowmapsrv shadowcompile computetexture
	command(t);
	return true;
}

void SRVDebug::update(Console* _console)
{
	float time = mainState->cbContainer->cbEveryFrame.timeofDay;
	std::string toString = "Time of Day: " + std::to_string(time) + " h";

	_console->loadText(toString, textStart + 1, 0);

}

bool SRVDebug::onShow(const std::string& _subject, int& _index)
{
	_index = graphics->getHandleIndex(_subject, ResourceType::ShaderResourceView);
	if(_index >= 0)
		return true;

	return false;
}

bool SRVDebug::command(std::string& _command)
{
	char delimiter = ' ';
	int sep = _command.find_first_of(delimiter);
	std::string command = _command.substr(0,sep);
	std::string subject = _command.substr(sep+1, _command.length()-1);

	if(sep <= 0)
		return false;

	int temp;

	if(command == "show")
	{
	
		temp = graphics->getHandleIndex(subject, ResourceType::ShaderResourceView);
		if (temp != -1)
		{
			srvIndex = temp;
			input.pos.x = 0;
			graphics->updateVBuffer(bufferIndex, (void**)&input, 1, sizeof(DefaultVertex));
			return true;
		}

		temp = graphics->getHandleIndex(subject, ResourceType::ShaderResourceViewArray);
		if (temp != -1)
		{
			srvIndex = temp;
			input.pos.x = 1;
			graphics->updateVBuffer(bufferIndex, (void**)&input, 1, sizeof(DefaultVertex));
			return true;
		}
		else
		{
			std::cout << "LOL nopp : The resource view " << _command.c_str() << " does not exist. \n";
			return false;
		}
	}

	return false;
}


void SRVDebug::draw(Console* _console)
{
	shaders->use();

	//graphics->setToScreenRender(-1);
	graphics->setRenderTargetViews(backBufferRTV,1,-1);
	graphics->setInputLayout((int)LayoutTypes::LAYOUT_DEFAULT);

	graphics->setTopology(1);

	//graphics->setRasterState(2);

	UINT stride = sizeof(DefaultVertex);
	UINT offset = 0;
	

	if (input.pos.x == 0)
	{
		graphics->setPixelShaderResource(1, srvIndex, 1);
	}
	else if (input.pos.x == 1)
	{
		graphics->setPixelShaderResource(2, srvIndex, 1);
	}

	graphics->setVertexBuffers(0, bufferIndex , 1, &stride, &offset);

	graphics->setBlendState(1);

	graphics->draw(1, 0);

	for (unsigned int i = 0; i < numTexts; ++i)
	{
		unsigned int index = i + textStart;
		unsigned int indY = 5 + i * 20;
		_console->draw(index, glm::vec2(5, indY), glm::vec4(1, 0, 0, 1));
	}
}