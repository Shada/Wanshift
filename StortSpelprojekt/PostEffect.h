#pragma once

#include "ShaderClass.h"

struct RainArgData
{
	std::string weatherType;
	std::string startState;
	std::string endState;
};


class MainState;

class PostEffect
{
public:
	PostEffect(std::string _shaderName);
	virtual ~PostEffect(void);

	void init(MainState& _mainState);
	virtual void preProcess(float _dt);
	virtual void draw(float _dt, int _renderTarget);

	std::string getName()const;
	void toggleActive(bool _isActive);
	bool isActive();
	virtual void		setArgs(void* _argData){}

protected:
	RenderInterface*	graphics;
	ShaderClass*		shader;
	virtual void		initPostEffect(MainState& _mainState) = 0;
	bool				active;
	

private:
	std::string name;
	
};

