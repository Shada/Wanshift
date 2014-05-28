#pragma once
#include "glm\glm.hpp"
#include <iostream>
#include <vector>
#include <array>
#include "RenderInterface.h"

//the camera frustum generated in the camera. It contains 6 vec4 planes.
struct Frustum
{
	glm::vec4 leftPlane;
	glm::vec4 rightPlane;
	glm::vec4 topPlane;
	glm::vec4 botPlane;
	glm::vec4 nearPlane;
	glm::vec4 farPlane;
};

class CullingNode
{
protected:
	//the lower left corner of the node
	glm::vec3 min;
	//the upper right corner of the node
	glm::vec3 max;
	//pointer to the renderer
	RenderInterface *graphics;
public:
	/* constructor
	#_graphics is the pointer to the renderer*/
	CullingNode(RenderInterface *_graphics);
	/* the virtual draw function
	#__frustum is the frustum to cull with*/
	virtual void draw(Frustum *_frustum) = 0;
	/*frustum culling function
	#_Frustum is the frustum to cull with
	#returns true if the target is within the frustum*/
	bool hitTestFrustum(Frustum *_frustum);
	/*plane collision. checks if any of the points in the node are on the negative side of the plane.
	#_plane is the plane to perform the hitTest with.
	#returns true if all points are on the positive side of the plane, else false.*/
	bool hitTestplaneAABB(glm::vec4 *_plane);
	/* performs a dot product of a plane and position.
	#_plane is the plane
	#_pos is the position
	#returns the dot product of the two.*/
	float planeDotCoord(glm::vec4 *_plane, glm::vec3 *_pos);
	/* tests if a point is inside the AABB
	#_point is the point in question
	#returns true if _point is inside the AABB*/
	bool hitTestPoint(glm::vec3 _point);
	/* sets the dimensions of a node
	#_min is the point with the lowest x,y and z.
	#_max is the point with the highest x,y and z.*/
	void setDimensions(glm::vec3 _min, glm::vec3 _max);
	/*destructor*/
	~CullingNode();
};

