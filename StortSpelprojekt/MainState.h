#pragma once

// Comment this line out to navigate the player across the landscape
#define debugCamera

#include "AspectInterface.h"

#include <array>
#include "Window.h"
#include "WinTimer.h"
#include "WanInput\InputFactory.h"
#include "RenderInterface.h"

#include <vector>
#include "ReaderINI.h"

static const int ALL_ASPECTS_NUM = 20;

#include "Debugger.h"

class MainState
	: public Window
{
public:

	
	float biomDiff;

	MainState(void);
	~MainState(void);

	/*	Initialize mainState members 
		#returns
			true if all member inits succeeds
			false if any member init fails*/
	bool init();
	/* Game state internal update */
	void run();
	void shutDown();
	void updateCamera();

	InputInterface	*input;
	TimerInterface	*timer;
	RenderInterface	*graphics;
	Containers		c;
	Debugger		debugger;
	ReaderINI		initializer;

	float timeOfDay; /* the time of the day. 0~1. */


	/* push aspect to active state
	
		#_aspect: new Aspect instance
		
		#returns:
			true if aspect is unique
			false if aspect is duplicate */
	bool addAspect(const std::string& _aspectName, bool _reInit = false);

	/*	remove aspect from active state
	
		#_aspect: Aspect instance
		
		#returns
			true if _aspect successfully deleted
			false if _aspect already nullptr */
	bool removeAspect(const std::string& _aspectName);

	/*	get the aspect that you want

		#returns the wanted aspect if the name is valid, else nullptr
	*/
	template < class T >
	T *getAspect(std::string _aspectName)
	{
		for(uint i = 0; i < aspects.size(); i++)
		{
			if(aspects[i]->getName() == _aspectName)
				return dynamic_cast<T*>(aspects[i]);
		}

		return nullptr;
	}
	ConstantBufferContainer *cbContainer;

private:
	bool isActive;	/* kill me */
	bool isRunning;	/* let me rest */
	
	int cbOnceIndex;
	int cbEveryFrameIndex;
	int cbViewIndex;

	std::array<AspectInterface*, ALL_ASPECTS_NUM>	aspects;		/* inactive pool of aspects */
	std::map<std::string, AspectInterface*>			activeAspects;	/* aspects flagged active */



	// Note! - This is a temporary solution
	int screenHeight, screenWidth;
	bool windowed;



};
