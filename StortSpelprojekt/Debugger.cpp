#include "Debugger.h"
#include "SystemQueries.h"
#include "CameraQueries.h"
#include "RenderDebug.h"
#include "TextUtility.h"
#include "MainState.h"
#include "SoundDebug.h"
#include "GameSettings.h"

//#ifdef __DEBUG
//#include <gtest/gtest.h>
//TEST(DebugDomainTest, DebuggerTest)
//{
//	DebugDomain* d = new SystemQueries();
//	bool res = d->init(nullptr);
//
//	ASSERT_FALSE(res);
//}
//#endif
static const float updateFrequency = 10;


Debugger::Debugger() : isVisible(false), activeDomain(nullptr), backLog(std::array<std::string, 10>()), backLogIndex(0)
{
	
}


Debugger::~Debugger()
{
}

bool Debugger::init(MainState& _mainState)
{
	if(_mainState.graphics == nullptr ||
	   _mainState.input == nullptr)
	   return false;

	input = _mainState.input;
	bool res = console.init(_mainState.graphics);

	if(!res)
		return false;

	domains.insert(std::make_pair("system", new SystemQueries()));
	domains.insert(std::make_pair("srvdebug", new SRVDebug()));
	domains.insert( std::make_pair("camera", new CameraQueries()));
	domains.insert( std::make_pair("gfx", new RenderDebug()));
	//domains.insert(std::make_pair("sound", new SoundDebug()));
	//domains.insert(std::make_pair("gameset", new GameSettings()));

	for(DDomain::iterator dIt = domains.begin(); dIt != domains.end(); ++dIt)
	{
		dIt->second->initialize(&console, _mainState);
	}

	if(domains.size() > 0)
	{
		activeDomain = domains.find("gfx")->second;
	}
	TextContainer tc = *_mainState.c.get<TextContainer>(Containers::TEXT);


	commandTextIndex = 0;	// I can't wrap my head around how I'm supposed to find the correct buffer. Some values work, some don't.
	console.createText(30,commandTextIndex);

	return res;
}

bool Debugger::onDomainSwitch(std::string& _text)
{
	int sep = text.find_last_of(' ');
	std::string domainName = text.substr(sep + 1, text.size() - 1);
	DDomain::iterator dIt = domains.find(domainName);
	if(dIt != domains.end())
	{
		activeDomain = dIt->second;
		text.clear();
		return true;
	}

	printf("LOL no [%s] domain!", domainName.c_str());
	text.clear();
	return false;
}

void Debugger::onDomainCommand(std::string& _text)
{
	for(DDomain::iterator dIt = domains.begin(); dIt != domains.end(); ++dIt)
	{
		bool validCommand = dIt->second->command(text);
		if(validCommand)
		{
			text.clear();
			break;
		}
	}
	text.clear();
}

/* set if debugger should draw or not */
void toggleVisibility(bool& _isVisible)
{
	if(!_isVisible)
		_isVisible = true;
	else
		_isVisible = false;
}

void Debugger::delegateCommand(std::string& _text)
{
	if(!isEmpty(_text))
	{
		addToBackLog(_text);
		backLogIndex = 0;

		if(text.find("switch") != std::string::npos)
		{
			onDomainSwitch(text);
		}
		else
		{
			onDomainCommand(text);
		}
	}
}

void Debugger::onKeyRelease(const KeyCode& _kCode)
{
	switch(_kCode)
	{
		case KeyCode::TILDE:
			text.clear();	// clear text input field
			toggleVisibility(isVisible);
			break;
		case KeyCode::RETURN:
			if(isVisible)
				delegateCommand(text);
			break;
		case KeyCode::BACKSPACE:
			if(isVisible)
				popLastChar(text);
			break;
		default:
			if(isVisible)
				if(text.length() < 20)
					addChar(_kCode, text);
			if(isVisible && text.size() < 30)
				addChar(_kCode, text);
			break;
	}
}
//
//void Debugger::onState(const KeyEvent& _key)
//{
//	switch(_key.state)
//	{
//	case KeyState::RELEASED:
//		onKeyRelease(_key.key);
//		break;
//	case KeyState::PRESSED:
//
//		break;
//	default:
//
//		break;
//	}
//}

bool Debugger::update(float _dt)
{
	activeDomain->update(&console);

	//updateThreshold -= _dt;

	//if((int)updateThreshold > 0)
	//	return false;

	//updateThreshold += updateFrequency;


	std::string chars;

	if(input->isNewKeyPress(KeyCode::TILDE))
	{
		text.clear();
		toggleVisibility(isVisible);
	}
	else if (input->isNewKeyPress(KeyCode::F11))
	{
		if (backLogIndex >=0 && backLogIndex <= 9)
		{			
			text = backLog[backLogIndex];
			backLogIndex++;
		}
	}
	else if (input->isNewKeyPress(KeyCode::F12))
	{
		if (backLogIndex >= 0 && backLogIndex <= 9)
		{			
			text = backLog[backLogIndex];
			backLogIndex--;
		}
	}
	else if (input->hasFrameKeys(chars))	// check if keyboard has been pressed
	{
		if (isVisible)
		{
			if (input->isContinousKeyPress(KeyCode::BACKSPACE))	// pop latest char from text
			{
				popLastChar(text);
			}
			else if(input->isNewKeyPress(KeyCode::RETURN))
			{
				delegateCommand(text);
			}
			else	// fetch pressed buttons as string
			{
				text += chars;
			}
		}
	}
	
	console.loadText(text, commandTextIndex, 0);	

	return true;
}

void Debugger::draw()
{
	if(!isVisible)
		return;

	console.drawOverlay();

	if(activeDomain != nullptr)
	{
		activeDomain->draw(&console);
	}

	console.draw(commandTextIndex, glm::vec2(300, 5), glm::vec4(1, 0, 0, 1));
}

void Debugger::addToBackLog(std::string _cmd)
{
	for (int i = 8; i >= 0; i--)
	{
		backLog[i + 1] = backLog[i];
	}

	backLog[0] = _cmd;
}