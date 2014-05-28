#include "PlayerCamAspect.h"
#include <glm/gtc/matrix_transform.hpp>
#include <Common.h>
#include "MainState.h"
#include "Utility.h"

PlayerCamAspect::PlayerCamAspect()
: AspectInterface("PlayerCam", 1), c(std::make_pair(Containers::CAMERA, MODIFY)), terrain(c.get<TerrainContainer>(Containers::TERRAIN))
{

}

void PlayerCamAspect::init(MainState& _mainState)
{
	PlayerContainer &player(*c.get<PlayerContainer>(Containers::PLAYER));
	lookAtPos = &player.position;

	cam = c.get<CameraContainer>(Containers::CAMERA);
	terrain = c.get<TerrainContainer>(Containers::TERRAIN);
	cam->lookAtPos = &player.position;;

	rightMouseDown = false;

	deltaSum = 0;
	posSum = glm::vec4(0, 0, 0, 0);

	resolution = _mainState.graphics->getResolution();
	input = _mainState.input;
	initCamera();
	updateFrustum();

	deltas = std::vector<float>();
	playerPositions = std::vector<glm::vec4>();
}

void PlayerCamAspect::initCamera()
{
	cam->viewMatrix = glm::mat4();
	cam->projMatrix = glm::mat4();
	cam->vUp = glm::vec4(0, 1, 0, 0);
	cam->vRight = glm::vec4(1,0,0,0);
	cam->vLastPos = *lookAtPos;
	cam->vLookDir = cam->vLookDirReference = glm::normalize(*cam->lookAtPos - glm::vec3(cam->vEye));

	baseDistanceToPlayer = distanceToPlayer = glm::vec4(0, 7, 35, 0);
	desired = cam->vEye = glm::vec4(*lookAtPos, 1) - distanceToPlayer;

	cam->yawAngle = cam->pitchAngle = 0;

	cam->projMatrix	= glm::perspective(45.f, (float)resolution.x / (float)resolution.y, 1.f, 30000.f);
	//cam.projMap		= glm::perspective(45.f, (float)resolution.x / (float)resolution.y, 1.f, 30000.f);

	cam->aspectRatio = (float)resolution.x / (float)resolution.y;
	cam->FOV = 45.f;
	cam->viewMatrix = glm::lookAt(glm::vec3(cam->vEye), *cam->lookAtPos, glm::vec3(cam->vUp));
}

void PlayerCamAspect::updateFrustum()
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

void PlayerCamAspect::normalizePlane(glm::vec4 *plane)
{
	float magnitude = 1 / sqrt(plane->x * plane->x + plane->y * plane->y + plane->z * plane->z);
	plane->x *= magnitude;
	plane->y *= magnitude;
	plane->z *= magnitude;
	plane->w *= magnitude;
}

void PlayerCamAspect::runAspect(float _dt)
{
	// Convert from milliseconds into seconds
	_dt /= 1000;

	rightMouseDown = input->isContinuousMouseKeyPress(MouseKeyCode::RIGHT, KeyState::PRESSED);
	/*if(input->isNewMousePosition())
	{*/
		MOUSEPOSITION p = input->getMousePosition();
		rotate((int)p.crX, (int)p.crY);
	/*}*/

	updateCameraPosition(_dt);
	updateFrustum();
}

void PlayerCamAspect::rotate(int _deltaX, int _deltaY)
{
	cam->yawAngle += -(float)_deltaX *.2f;
	glm::mat4 rotMatrix = glm::rotate(glm::mat4(), cam->yawAngle, glm::vec3(0, 1, 0)); // yaw
	cam->vRight = rotMatrix * glm::vec4(1, 0, 0, 0);
	desired = rotMatrix * baseDistanceToPlayer;

	cam->pitchAngle += -(float)_deltaY *.2f;
	if(cam->pitchAngle > 60)
		cam->pitchAngle = 60;
	if(cam->pitchAngle < -60)
		cam->pitchAngle = -60;

	rotMatrix = glm::rotate(glm::mat4(), cam->pitchAngle, glm::vec3(cam->vRight)); // pitch
	cam->vUp = rotMatrix * glm::vec4(0, 1, 0, 0);
	
	desired = rotMatrix * desired;

	distanceToPlayer = -desired;
	desired += glm::vec4(*lookAtPos, 1);
}

void PlayerCamAspect::updateCameraPosition(float _dt)
{
	cam->vLookDir = glm::normalize(*cam->lookAtPos - glm::vec3(cam->vEye));
	if(!rightMouseDown)
		cam->vLookDirReference = cam->vLookDir;

	cam->vEye = desired = glm::vec4(*lookAtPos, 1) - distanceToPlayer;
	float y = Utility::mapToTerrain(glm::vec2(cam->vEye.x, cam->vEye.z), *terrain).y;
	if(cam->vEye.y < y + 5)
		cam->vEye.y = y + 5;

	cam->viewMatrix = glm::lookAt(glm::vec3(cam->vEye), *cam->lookAtPos, glm::vec3(cam->vUp));

	cam->vLastPos = *cam->lookAtPos;
}

PlayerCamAspect::~PlayerCamAspect()
{
}
