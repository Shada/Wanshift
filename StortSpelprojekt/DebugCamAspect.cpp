#include "DebugCamAspect.h"
#include <glm/gtc/matrix_transform.hpp>
#include "MainState.h"
DebugCamAspect::DebugCamAspect()
		: AspectInterface("DebugCam", 10), c(std::make_pair(Containers::CAMERA, MODIFY))
{
}

void DebugCamAspect::init(MainState& _mainState)
{
	forwardKeyDown = false;
	backwardKeyDown = false;
	leftKeyDown = false;
	rightKeyDown = false;
	shiftKeyDown = false;
	ctrlKeyDown = false;
	spaceKeyDown = false;

	lockCamera = false;

	input = _mainState.input;
	resolution = _mainState.graphics->getResolution();
	initCamera();
	updateFrustum();
}

void DebugCamAspect::initCamera()
{
	cam = c.get<CameraContainer>(Containers::CAMERA);


	cam->viewMatrix = glm::mat4();
	cam->projMatrix = glm::mat4();
	cam->vUp = glm::vec4(0, 1, 0, 0);
	cam->vRight = glm::vec4(1,0,0,0);

	PlayerContainer &player(*c.get<PlayerContainer>(Containers::PLAYER));

	cam->vEye = glm::vec4(player.position,1);//glm::vec4(500, 2000, 500, 1);

	cam->vLastPos = glm::vec3(0, 0, 0);
	cam->lookAtPos = new glm::vec3(0, 0, 0);
	cam->vLookDir = cam->vLookDirReference = glm::vec3(0, 0, 1);// glm::normalize(*cam->lookAtPos - glm::vec3(cam->vEye));

	cam->yawAngle = cam->pitchAngle = 0;

	cam->projMatrix = glm::perspective(45.f, (float)resolution.x / (float)resolution.y, 1.f, 300000.f);

	cam->aspectRatio = (float)resolution.x / (float)resolution.y;
	cam->FOV = 45.f;

	cam->viewMatrix = glm::lookAt(glm::vec3(cam->vEye), glm::vec3(0,0,1), glm::vec3(cam->vUp));
}

void DebugCamAspect::rotate(int _deltaX, int _deltaY)
{
	if(lockCamera)
		return;

	float deltaX = -(float)_deltaX * .1f;
	float deltaY = (float)_deltaY * .1f;

	CameraContainer &cam(*c.get<CameraContainer>(Containers::CAMERA));
	
	cam.yawAngle += deltaX;
	glm::mat4 rotMatrix = glm::rotate(glm::mat4(), cam.yawAngle, glm::vec3(0, 1, 0)); // yaw
	cam.vRight = rotMatrix * glm::vec4(1, 0, 0, 0);

	vLook = rotMatrix * glm::vec4(0, 0, 1, 0);

	cam.pitchAngle += deltaY;
	if(cam.pitchAngle + deltaY > 80)
		cam.pitchAngle = 80;
	if(cam.pitchAngle + deltaY < -80)
		cam.pitchAngle = -80;


	rotMatrix = glm::rotate(glm::mat4(), cam.pitchAngle, glm::vec3(cam.vRight)); // pitch
	cam.vUp = rotMatrix * glm::vec4(0, 1, 0, 0);

 	vLook = rotMatrix * vLook;

	cam.vLookDir = glm::vec3(vLook);
}

void DebugCamAspect::runAspect(float _dt)
{
	// Convert from milliseconds into seconds
	_dt /= 1000;

	if(!lockCamera)
	{
		forwardKeyDown	= input->isContinousKeyPress(KeyCode::W);
		leftKeyDown		= input->isContinousKeyPress(KeyCode::A);
		backwardKeyDown = input->isContinousKeyPress(KeyCode::S);
		rightKeyDown	= input->isContinousKeyPress(KeyCode::D);
		shiftKeyDown	= input->isContinousKeyPress(KeyCode::LEFT_SHIFT);
		ctrlKeyDown		= input->isContinousKeyPress(KeyCode::LEFT_CTRL);				
		spaceKeyDown	= input->isContinousKeyPress(KeyCode::SPACE);
			
		/*if(input->isNewMousePosition())
		{*/
			MOUSEPOSITION p = input->getMousePosition();
			rotate((int)p.crX, (int)p.crY);
		/*}*/
	}

	glm::vec4 rSpeed;
	float speed = _dt * 750;
	if(forwardKeyDown)		rSpeed += vLook;
	if(backwardKeyDown)		rSpeed -= vLook;
	if(leftKeyDown)			rSpeed += cam->vRight;
	if(rightKeyDown)		rSpeed -= cam->vRight;
	if(shiftKeyDown)		speed *= 10;
	if(ctrlKeyDown)			rSpeed -= glm::vec4(0,1,0,0);
	if(spaceKeyDown)		rSpeed += glm::vec4(0,1,0,0);

	if(glm::length(rSpeed) > 0)
		cam->vEye += glm::normalize(rSpeed) * speed;
	cam->viewMatrix = glm::lookAt(glm::vec3(cam->vEye), glm::vec3(cam->vEye + vLook), glm::vec3(cam->vUp));
	updateFrustum();
}

void DebugCamAspect::updateFrustum()
{
	float clip[16];

	clip[0] = cam->viewMatrix[0].x * cam->projMatrix[0].x + cam->viewMatrix[0].z * cam->projMatrix[2].x;
	clip[1] = cam->viewMatrix[0].y * cam->projMatrix[1].y + cam->viewMatrix[0].z * cam->projMatrix[2].y;
	clip[2] = cam->viewMatrix[0].z * cam->projMatrix[2].z;
	clip[3] = cam->viewMatrix[0].z * cam->projMatrix[2].w;

	clip[4] = cam->viewMatrix[1].x * cam->projMatrix[0].x + cam->viewMatrix[1].z * cam->projMatrix[2].x;
	clip[5] = cam->viewMatrix[1].y * cam->projMatrix[1].y + cam->viewMatrix[1].z * cam->projMatrix[2].y;
	clip[6] = cam->viewMatrix[1].z * cam->projMatrix[2].z;
	clip[7] = cam->viewMatrix[1].z * cam->projMatrix[2].w;

	clip[8] = cam->viewMatrix[2].x * cam->projMatrix[0].x + cam->viewMatrix[2].z * cam->projMatrix[2].x;
	clip[9] = cam->viewMatrix[2].y * cam->projMatrix[1].y + cam->viewMatrix[2].z * cam->projMatrix[2].y;
	clip[10] = cam->viewMatrix[2].z * cam->projMatrix[2].z;
	clip[11] = cam->viewMatrix[2].z * cam->projMatrix[2].w;

	clip[12] = cam->viewMatrix[3].x * cam->projMatrix[0].x + cam->viewMatrix[3].z * cam->projMatrix[2].x;
	clip[13] = cam->viewMatrix[3].y * cam->projMatrix[1].y + cam->viewMatrix[3].z * cam->projMatrix[2].y;
	clip[14] = cam->viewMatrix[3].z * cam->projMatrix[2].z + cam->viewMatrix[3].w * cam->projMatrix[3].z;
	clip[15] = cam->viewMatrix[3].z * cam->projMatrix[2].w;

	// Left plane
	cam->frustum.leftPlane.x = clip[3] + clip[0];
    cam->frustum.leftPlane.y = clip[7] + clip[4];
    cam->frustum.leftPlane.z = clip[11] + clip[8];
    cam->frustum.leftPlane.w = clip[15] + clip[12];

	// Right plane
    cam->frustum.rightPlane.x = clip[3] - clip[0];
    cam->frustum.rightPlane.y = clip[7] - clip[4];
    cam->frustum.rightPlane.z = clip[11] - clip[8];
    cam->frustum.rightPlane.w = clip[15] - clip[12];
 
    // Bottom plane
    cam->frustum.botPlane.x = clip[3] + clip[1];
    cam->frustum.botPlane.y = clip[7] + clip[5];
    cam->frustum.botPlane.z = clip[11] + clip[9];
    cam->frustum.botPlane.w = clip[15] + clip[13];

	 // Top plane
    cam->frustum.topPlane.x = clip[3] - clip[1];
    cam->frustum.topPlane.y = clip[7] - clip[5];
    cam->frustum.topPlane.z = clip[11] - clip[9];
    cam->frustum.topPlane.w = clip[15] - clip[13];
 
    // Near plane
    cam->frustum.nearPlane.x = clip[3] + clip[2];
    cam->frustum.nearPlane.y = clip[7] + clip[6];
    cam->frustum.nearPlane.z = clip[11] + clip[10];
    cam->frustum.nearPlane.w = clip[15] + clip[14];
 
    // Far plane
    cam->frustum.farPlane.x = clip[3] - clip[2];
    cam->frustum.farPlane.y = clip[7] - clip[6];
    cam->frustum.farPlane.z = clip[11] - clip[10];
    cam->frustum.farPlane.w = clip[15] - clip[14];
}

void DebugCamAspect::normalizePlane(glm::vec4 *_plane)
{
	float magnitude = sqrt( _plane->x * _plane->x + _plane->y * _plane->y + _plane->z * _plane->z );
	_plane->x /= magnitude;
	_plane->y /= magnitude;
	_plane->z /= magnitude;
	_plane->w /= magnitude;
}

void DebugCamAspect::checkKeyAndDirection(KeyState _state, bool &_keyDown)
{
	if(!_keyDown && _state == KeyState::PRESSED)
		_keyDown = true;
	else if(_keyDown && _state == KeyState::RELEASED)
		_keyDown = false;
}

DebugCamAspect::~DebugCamAspect()
{
}