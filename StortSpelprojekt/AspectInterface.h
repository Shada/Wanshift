#pragma once

#include <string>
#include <vector>
#include "InfoFunction.h"
#include "MessageSystem.h"

class MainState;

class AspectInterface
{
public:
	AspectInterface(std::string _name = "UNDEFINED", int _updateFrequency = 1000);

	virtual ~AspectInterface();

	/* aspect main run loop - calls runAspect when time exceeds updateFrequency */
	bool	run(float _dt);

	/* get name of derived class */
	std::string	getName()const;

	/* get update frequency for derived class */
	int			getUpdateFrequency()const;

	/* reset the aspect */
	virtual void reset() {};

	/*	initialize the aspect
		#_mainState: service provider, fetch the required data from here */
	virtual void init(MainState& _mainState) = 0;

	static MessageSystem messages;
	std::vector< InfoFunction > functions;

protected:
	/* all internal data processing for derived classes are to be defined here */
	virtual void runAspect(float _dt) = 0;

private:
	std::string		name;
	int				updateFrequency;
	float			updateThreshold,
					timeSinceLastUpdate;
};