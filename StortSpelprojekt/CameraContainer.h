#pragma once

#include "Container.h"
#include "glm\glm.hpp"

struct CameraContainer : Container
{
	glm::mat4		viewMatrix,
	projMatrix;

	glm::vec4		vUp,
					vEye,
					vRight;

	glm::vec3		vLastPos,
		vLookDir,
		// Have two look at varibles to enable the player to rotate around the character without changing moving direction
		vLookDirReference;

	float			aspectRatio;
	float			FOV;

	const glm::vec3	*lookAtPos;

	/* Saved values to enable lock on pitch rotation. 360 degrees pitch rotation is not desired */
	float			yawAngle,
		pitchAngle;

	Frustum frustum;

	void addData(uint _index, uint _number) {}

	// See Container.h for documentation
	void removeData(uint _index, uint _number) {}
};