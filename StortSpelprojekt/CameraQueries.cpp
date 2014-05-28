#include "CameraQueries.h"
#include "CameraContainer.h"
#include "PlayerContainer.h"
#include "MainState.h"

CameraQueries::CameraQueries(void)
{
}


CameraQueries::~CameraQueries(void)
{
}

bool CameraQueries::init(Console* _console, MainState& _mainState)
{
	if(_console == nullptr)
		return false;

	cameraState = CameraState::DEBUG_MODE;

	textStart = 4;
	numTexts = 6;

	for(unsigned int i = 0; i < numTexts; ++i)
		_console->createText(30,textStart + i);

	mainState = &_mainState;

	return true;
}

bool CameraQueries::command(std::string& _command)
{
	char delimiter = ' ';
	int sep = _command.find_first_of(delimiter);
	if(sep <= 0)
		return false;

	std::string key, val;
	key = _command.substr(_command.find_first_not_of(delimiter), sep);
	val = _command.substr(sep+1, _command.find_last_not_of(delimiter));

	if(key == "cam")
	{
		if(val == "free" && cameraState == CameraState::PLAYER_MODE)
		{
			cameraState = CameraState::DEBUG_MODE;
			mainState->removeAspect("Player");
			mainState->removeAspect("PlayerCam");
			mainState->addAspect("DebugCam", true);

			printf("free camera\n");

		}
		else if(val == "lock" && cameraState == CameraState::DEBUG_MODE)
		{
			cameraState = CameraState::PLAYER_MODE;
				
			mainState->removeAspect("DebugCam");
			mainState->addAspect("Player", false);
			mainState->addAspect("PlayerCam", true);

			printf("locked camera\n");
		}
		else return false;

		return true;
	}
	else if(key == "spawn")
	{
		if(val == "cam")
		{
			//mainState->addAspect("Player", false);
			//glm::vec3 posT = glm::vec3( containers->get<CameraContainer>(Containers::Type::CAMERA)->vEye );
			//containers->get<PlayerContainer>(Containers::Type::PLAYER)->position = posT;
		}
		else return false;

		return true;
	}

	return false;
}

void CameraQueries::update(Console* _console)
{
	int x,y,z;
	std::string toString;

	_console->loadText("Camera Debug", textStart, 0);

	if(cameraState == CameraState::DEBUG_MODE)
		toString = "Camera Mode: Free";
	else
		toString = "Camera Mode: Locked to Player";

	_console->loadText(toString, textStart + 1, 0);

	PlayerContainer* player = mainState->c.get<PlayerContainer>(Containers::PLAYER);
	
	x = (int)player->position.x;
	y = (int)player->position.y;
	z = (int)player->position.z;
	toString = "Avatar: {" + std::to_string(x) + ":" + 
									std::to_string(y) + ":" +
									std::to_string(z) + "}";

	_console->loadText(toString, textStart + 2, 0);

	CameraContainer* camera = mainState->c.get<CameraContainer>(Containers::CAMERA);
	x = (int)camera->vEye.x;
	y = (int)camera->vEye.y;
	z = (int)camera->vEye.z;
	toString = "Camera: {" + std::to_string(x) + ":" +
									std::to_string(y) + ":" +
									std::to_string(z) + "}";

	_console->loadText(toString, textStart + 3, 0);


	

	//toString = "Sun: {" + std::to_string(x) + ":" +
	//							std::to_string(y) + ":" +
	//							std::to_string(z) + "}";

//	_console->loadText(toString, textStart + 4, 0);

	float time	= mainState->timeOfDay * 24.0f;
	toString	= "Time of Day: " + std::to_string(time) + " h";

	_console->loadText(toString, textStart + 4, 0);
}

void CameraQueries::draw(Console* _console)
{
	for(unsigned int i = 0; i < numTexts; ++i)
	{
		unsigned int index	= i + textStart;
		unsigned int indY	= 5 + i * 20;
		_console->draw(index, glm::vec2(5,indY), glm::vec4(1,0,0,1));
	}
}