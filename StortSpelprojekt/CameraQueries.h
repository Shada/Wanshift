#pragma once

#include "DebugDomain.h"
enum class CameraState
{
	DEBUG_MODE,
	PLAYER_MODE,
};

class CameraQueries
	: public DebugDomain
{
public:
	CameraQueries(void);
	~CameraQueries(void);

	bool init(Console* _console, MainState& _mainState)override;
	bool command(std::string& _command)override;
	void update(Console* _console)override;
	void draw(Console* _console)override;

private:
	MainState*		mainState;
	unsigned int	textStart;
	unsigned int	numTexts;
	CameraState		cameraState;
};

