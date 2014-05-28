#include "AspectCollection.h"
#include "Containers.h"
#include "MainState.h"
#include "ReaderINI.h"
#include "RenderFactory.h"
#include "Utility_INI.h"
#include "WanInput\Common.h"
#include "ConstantBufferContainer.h"
#include "SoundEngine.h"

#ifdef __DEBUG
#include <gtest/gtest.h>
#endif

InputInterface* pInput; // this is not good practice

#include "Object2DAspect.h"

#include <glm/gtc/matrix_transform.hpp>

//#ifdef __DEBUG
//TEST(initDebugger, MainStateTest)
//{
//	Debugger dbg;
//	bool res = dbg.init(nullptr,nullptr);
//	ASSERT_FALSE(res);
//}
//#endif
MainState::MainState() : isActive(true), isRunning(true), timeOfDay(0),
c(std::make_pair(Containers::CAMERA, READ), std::make_pair(Containers::PLAYER, READ), std::make_pair(Containers::CONSTBUFFER, MODIFY))
{

}

MainState::~MainState()
{

}

void MainState::shutDown()
{
	for(uint i = 0; i < 10; i++)
	{
		SAFE_DELETE(aspects[i]);
	}
	SoundEngine::getInstance()->destroy();
	SAFE_DELETE(graphics);
	SAFE_DELETE(input);
	SAFE_DELETE(timer);
	removeAspect("DeferredRenderingAspect");
	removeAspect("World");
	removeAspect("ZParticles");
	removeAspect("Player");
	removeAspect("DebugCam");
	removeAspect("network");
	removeAspect("PostEffects");
}

bool MainState::init()
{
	bool initResult; // use this for init sanity checks

	/* Not sure why this is here */
	CameraContainer &camera(*c.get<CameraContainer>(Containers::CAMERA));
	PlayerContainer &player(*c.get<PlayerContainer>(Containers::PLAYER));
	SandStormContainer &storm(*c.get<SandStormContainer>(Containers::SANDSTORM));
	cbContainer = c.get<ConstantBufferContainer>(Containers::CONSTBUFFER);

	cbContainer->cbEveryFrame.padding.x = 1;

	/* Open the master .ini suite (Default + Debug + User) */
	initializer.generateIniData();
	RootINI r = initializer.getInitData();

	/* INI - Graphics */ 
	Node_INI node = r.getNode("Graphics");
	std::string rawRes = node.getValue("resolution");

	int vIndex;
	std::string w, h;
	vIndex = INI::getValueByIndex(w, rawRes);
	vIndex = INI::getValueByIndex(h, rawRes, vIndex);
	std::string temp = node.getValue("windowed");

	screenWidth = std::stoi(w);
	screenHeight = std::stoi(h);
	windowed = temp == "1";

	/* Open Window */
	initResult = createWindow(screenWidth, screenHeight, false);
	if(!initResult)
		return false;	// window failed to register

	/* Register mouse + keyboard  */
	pInput = Input::InputAPI::createInput(IType::RAW_KEYBOARD_MOUSE);
	initResult = pInput->init(hWnd);

	if(!initResult)
		return false;

	input = pInput;

	/* Open DirectX */
	Render::RenderAPI::print();
	graphics = NULL;
	graphics = Render::RenderAPI::createRenderer(Version::DX11);

	initResult = graphics->inititalizeDirectX(screenWidth, screenHeight, windowed, hWnd);

	if (!initResult)
		return false;

	/* Create all aspects up front */
	MultiplayerAspect* t = new MultiplayerAspect();
	if(t->connect())
	{
		aspects[0] = t;
		storm.isMultiPlayer = true;
	}
	else
	{
		storm.isMultiPlayer = false;
		delete t;
		aspects[0] = new SingleplayerAspect();
	}

	aspects[1] = new TerrainAspect();
	aspects[2] = new DeferredRenderingAspect();
	aspects[3] = new EnemyAspect();
	aspects[4] = new Object2DAspect();
	aspects[5] = new PlayerAspect();
	aspects[6] = new PostEffectsAspect();
	aspects[7] = new PlayerCamAspect();
	aspects[8] = new DebugCamAspect();
	aspects[9] = new EnvironmentAspect();

	/* Initialize all aspects */
	for(AspectInterface* asp : aspects)
	{
		if(asp != nullptr)
		{
			printf("\n--- init: %s ---\n", asp->getName().c_str());
			asp->init(*this);
			printf("\n--- done: %s ---\n", asp->getName().c_str());
		}
	}

	/* Add the active aspects to frame update */
	
	addAspect("DeferredRenderingAspect");
	addAspect("Terrain");
	addAspect("World");
	addAspect("Player");
	addAspect("PostEffects");
	addAspect("network");
	addAspect("ZZEnemy");

	/* [UGLY HACK]
	PLEASE NOTE!!! This is not my fault!!!
	- For some reason debugCam must be the first camera or else
	  certain particle/post effects and the sky wont get rendered */
	addAspect("DebugCam");

	/* Create Timer */	
	timer = new WinTimer();

	/* Create sound engine */
	printf("\n--- init: Sound ---\n");
	SoundEngine::getInstance()->initSystem();
	printf("System\n");
	SoundEngine::getInstance()->initMusic();
	printf("Music\n");
	SoundEngine::getInstance()->initEffects();
	printf("Effects\n--- done: Sound ---\n");

	/* Not sure why this is here */
	cbEveryFrameIndex = graphics->getHandleIndex("cbEveryframe", ResourceType::ConstantBuffer);
	cbOnceIndex = graphics->getHandleIndex("cbOnce", ResourceType::ConstantBuffer);
	cbViewIndex = graphics->getHandleIndex("cbView", ResourceType::ConstantBuffer);
	debugger.init(*this);
	
	cbContainer->cbOnce.padding = glm::vec3(0, 0,0);
	cbContainer->cbOnce.mProj = camera.projMatrix;
	cbContainer->cbOnce.mProjInv = glm::inverse(camera.projMatrix);
	cbContainer->cbOnce.resolution = glm::vec2((float)screenWidth, (float)screenHeight);

	graphics->updateCBuffer(cbOnceIndex, &cbContainer->cbOnce);

	/* Check allocated vram for funsies */
	float totVRAM = graphics->totalAllocatedVRAM();
	printf("Total allocated VRAM(MB):%f\n", totVRAM / 1024 / 1024); 

	/* Lastly update timer one frame to minimize dt */
	timer->frame();

	return true;
}

bool MainState::addAspect(const std::string& _aspectName, bool _reInit)
{
	/* aspect already in use */
	if(activeAspects.count(_aspectName) > 0)
	{
		return false;
	}
	else
	{
		AspectInterface* t = nullptr;
		for(auto a : aspects)
		{
			if(a == nullptr)
				continue;

			if(a->getName() == _aspectName)
			{
				t = a;
				if(_reInit)
					t->init(*this);
				activeAspects.insert(std::make_pair(_aspectName, t));
				return true;
			}
		}
	}
	return false;
}

bool MainState::removeAspect(const std::string& _aspectName)
{
	for(auto a : aspects)
	{
		if(a == nullptr)
			continue;

		if(a->getName() == _aspectName)
		{
			activeAspects.erase(_aspectName);
			return true;
		}
	}
	return false;
}

void MainState::run()
{
	/* time */
	timer->frame();
	float dt = timer->getTime();

	/*	[UGLY CAMHACK] */
	/*	Not my fault this has to be done but the player-centric camera
		can't be the initial camera for some reason */
	static float spawnTime;	/* pretty name for ugly solution */

	static bool once;
	if (!once)
	{
		spawnTime += dt;
		if (spawnTime > 100)
		{
			removeAspect("DebugCam");
			addAspect("Player");
			addAspect("PlayerCam", true);
			once = true;
		}
	}
	/*	[/UGLY HACK]*/

	/* temporary solution for quick shutdown - remove eventually */
	if(input->isNewKeyPress(KeyCode::ESCAPE, KeyState::PRESSED))
	{
		isActive = false;
	}

	if(!isActive)
	{
		PostQuitMessage(0);
		return;
	}



	debugger.update(dt);

	// Clear all the render targets
	graphics->clearRenderTargets(glm::vec4(0.0f, 0.0f, 0.0f, 0.f));
	
	/* updateCamera */
	updateCamera();

	/* Not sure why this is here */
	cbContainer->cbEveryFrame.timeStep = dt / 1000;

	cbContainer->cbEveryFrame.gameTime += cbContainer->cbEveryFrame.timeStep;
	cbContainer->cbEveryFrame.windDirection = glm::vec2(1, 0);

	PlayerContainer player = *c.get<PlayerContainer>(Containers::PLAYER);

	float newEnergy = player.energyInfo.energyAmount;

	float energyDt = cbContainer->cbEveryFrame.energy - newEnergy;
	if (cbContainer->cbEveryFrame.padding.x < 0.5)
	{
		cbContainer->cbEveryFrame.padding.x = 0.5;
		cbContainer->cbEveryFrame.padding.y = 0;
	}
	else if (cbContainer->cbEveryFrame.padding.x > 1)
	{
		cbContainer->cbEveryFrame.padding.x = 1;
		cbContainer->cbEveryFrame.padding.y = 1;
	}

	if (cbContainer->cbEveryFrame.padding.y == 0)
	{
		//everyFrame.padding.x += dt / 1000;
		cbContainer->cbEveryFrame.padding.x += energyDt / 20;
		if (cbContainer->cbEveryFrame.padding.x >= 1)
		{
			cbContainer->cbEveryFrame.padding.y = 1;
		}
	}
	else
	{
		//everyFrame.padding.x -= dt / 1000;
		cbContainer->cbEveryFrame.padding.x -= energyDt / 20;
		if (cbContainer->cbEveryFrame.padding.x <= 0.5)
		{
			cbContainer->cbEveryFrame.padding.y = 0;
		}
	}
	cbContainer->cbEveryFrame.energy = newEnergy;

	graphics->updateCBuffer(cbEveryFrameIndex, (void*)&cbContainer->cbEveryFrame);
	/* [/]Not sure why the code above is here */

	/* Update aspects */
	//std::vector< InfoFunction* > functionList;

	std::map<std::string, AspectInterface*>::iterator asIt;
	for(asIt = activeAspects.begin(); asIt != activeAspects.end(); ++asIt)
	{
		if(asIt->second == nullptr)
			continue;

		asIt->second->run(dt);

		/*for(uint i(0); i < asIt->second->functions.size(); i++)
		functionList.push_back(&asIt->second->functions[i]);*/
	}

	SoundEngine::getInstance()->update(dt);

	/*for(InfoFunction *f : functionList)
	(*f)();*/

	graphics->presentFinalScene();

	debugger.draw();

	// Present swap chain
	graphics->presentSwapChain();

	/* Check for messages */
	while(AspectInterface::messages.messageCount("main"))
	{
		MessageSystem::Message m(AspectInterface::messages.getMessage("main"));
		if(m.info == "dead")
		{
			std::pair<uint, uint> p = m.receiveMessage< std::pair<uint, uint> >();
			//c.get<PlayerContainer>(Containers::PLAYER)->reset(); // could use this or aspect below...
			aspects[2]->reset(); // maybe should move some stuff from player aspect to container, so the above alternative can do more resets...
			// TODO: Check if player and restart instead of shutdown
			//PostQuitMessage(0);
		}
		else
			; // TODO: Log memory leak
	}

	/* Update Input */ 
	input->frame();
}

/* Game specific message pump */
LRESULT CALLBACK Window::WndProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
{
	switch(_msg)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
			// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_INPUT:
		{
			InternalState state;
			state = pInput->pollInput(_lparam);

			if(state == InternalState::EVENT_BUFFER_OVERFLOW)
				printf("Input buffer overflow!\n");
			return 0;
		}
		default:
		{
			return DefWindowProc(_hwnd, _msg, _wparam, _lparam);
		}
	}
}

void MainState::updateCamera()
{
	CameraContainer &cam(*c.get<CameraContainer>(Containers::CAMERA));

	CBViewData view;


	view.cameraDir = glm::vec4(cam.vLookDir, 1);
	view.cameraPos = cam.vEye;
	view.mView = cam.viewMatrix;
	view.mViewInv = glm::inverse(cam.viewMatrix);

	graphics->updateCBuffer(cbViewIndex, &view);
}
