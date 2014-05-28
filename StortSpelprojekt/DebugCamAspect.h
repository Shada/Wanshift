#pragma once

#include "AspectInterface.h"
#include "Container.h"
#include "WanInput/InputInterface.h"

class DebugCamAspect : public AspectInterface
{
public:
	DebugCamAspect();
	~DebugCamAspect();

	void runAspect(float _dt) override;

	void init(MainState& _mainState)override;

private:
	Containers c;
	InputInterface *input;
	CameraContainer *cam;

	bool lockCamera;
	bool					forwardKeyDown,
		backwardKeyDown,
		leftKeyDown,
		rightKeyDown,
		jumpKeyDown,
		shiftKeyDown,
		ctrlKeyDown,
		spaceKeyDown;

	glm::vec2 resolution;
	glm::vec4 vLook;

	/* Sets value to all of the varibles in the camera container
	*/
	void initCamera();
	/*update the view frustum used for culling*/
	void updateFrustum();
	/*normalize the plane
	#_plane is the plane to be normalized*/
	void normalizePlane(glm::vec4 *_plane);
	/* Rotation using pitch and yaw
	* #_deltaX: the amount of pixels that the mouse has moved in x-axis
	* #_deltaY: the amount of pixels that the mouse has moved in y-axis
	*/
	void rotate(int _deltaX, int _deltaY);

	void checkKeyAndDirection(KeyState _state, bool &_keyDown);
};