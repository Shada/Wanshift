#pragma once
#include "DebugDomain.h"

#include "AtmosphereContainer.h"

class GameSettings :
	public DebugDomain
{
public:
	GameSettings(void);
	~GameSettings(void);

	bool command(std::string& _command) override;

	void update(Console* _console)override;

	void draw(Console* _console) override;

protected:

	bool init(Console* _console, MainState& _mainState) override;

private:
	Containers* containersPtr;
	RenderInterface* graphicsPtr;
};

