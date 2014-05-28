#include "PostEffect.h"

class CloudPostEffect
	: public PostEffect
{
public:
	CloudPostEffect();
	~CloudPostEffect();

	void preProcess(float _dt)override;
	void draw(float _dt, int _renderTarget)override;
	void initPostEffect(MainState& _mainState)override;

private:
	int				cloudDynamicVB;
	ShaderClass*	shader;
};