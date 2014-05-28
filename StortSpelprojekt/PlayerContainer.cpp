#include "PlayerContainer.h"
#include "SoundEngine.h"

void PlayerContainer::reset()
{
	energyInfo = EnergyInfo();
	position = glm::vec3(2480, 500, 2480); // will fetch spawnpoint fron the server.
}
void PlayerContainer::animateModel(std::pair< ull, ull > _id, AnimationState &_state, int &_frame, int &_prevFrame, float &_timer, float &_animInterpolation, float _idleFreq, float _runningFreq, float _extractionFreq)
{
	switch (_state)
	{
	case aIDLE:
		if (_timer >= _idleFreq)
		{
			_prevFrame = _frame;
			_frame + 1 < playerModel.getAnimationAt(aIDLE).nFrames ? _frame++ : _frame = 0;
			_timer = 0;
		}
		_animInterpolation = _timer / _idleFreq;

		break;
	case aRUNNING:
		if (_timer >= _runningFreq)
		{
			_prevFrame = _frame;
			_frame + 1 < playerModel.getAnimationAt(aRUNNING).nFrames ? _frame++ : _frame = 0;
			_timer = 0.f;
		}
		_animInterpolation = _timer / _idleFreq;

		if (_frame == 4 || _frame == 16)
		{
			if (_id == id)
			{
				SoundEngine::getInstance()->playPlayerSound(SoundEngine::Sound::PLAYER_WALKING);
			}
			else
			{
				SoundEngine::getInstance()->playOtherPlayerSound(SoundEngine::Sound::PLAYER_WALKING, _id);
			}
		}

		break;
	case aEXTRACTING:
		if (_timer >= _extractionFreq)
		{
			_prevFrame = _frame;
			_frame + 1 < playerModel.getAnimationAt(aEXTRACTING).nFrames ? _frame++ : _frame = 0;
			if (_frame == 0)
			{
				_state = aIDLE;
				_prevFrame = 0;
			}
			_timer = 0.f;
		}
		_animInterpolation = _timer / _idleFreq;

		break;
	case aNOANIMATION:
		_prevFrame = _frame;
		_frame = 0;
		_timer = 0;
		_animInterpolation = 0;
		break;
	}
}
