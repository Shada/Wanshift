#pragma once

#include "AspectInterface.h"
#include "Container.h"
#include "WanInput/InputInterface.h"
#include "Utility.h"

class PlayerCamAspect : public AspectInterface
{
public:
	PlayerCamAspect();
	~PlayerCamAspect();

	void runAspect(float _dt);
	void init(MainState& _mainState)override;
private:
	Containers c;
	InputInterface *input;
	CameraContainer *cam;
	TerrainContainer *terrain;

	bool rightMouseDown;

	const glm::vec3 *lookAtPos;
	glm::vec2		resolution;
	glm::vec4		desired,
					distanceToPlayer,
					baseDistanceToPlayer;

	std::vector<float> deltas;
	std::vector<glm::vec4> playerPositions;
	glm::vec4 posSum;
	float deltaSum;

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
	/* Updates the camera and makes it move towards a desired position, which is at a constant distance to the player
	* #_dt: delta time
	* #_rightClick: the camera can be moved to not be directly behinde the player if the right mouse button is pushed down
	*/
	void updateCameraPosition(float _dt);
};