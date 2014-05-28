#include "CullingNode.h"


CullingNode::CullingNode(RenderInterface *_graphics)
{
	graphics = _graphics;
}

bool CullingNode::hitTestFrustum(Frustum *_frustum)
{
	if( !hitTestplaneAABB( &_frustum->nearPlane ) )
		return false;
	if( !hitTestplaneAABB( &_frustum->farPlane ) )
		return false;
	if( !hitTestplaneAABB( &_frustum->topPlane ) )
		return false;
	if( !hitTestplaneAABB( &_frustum->botPlane ) )
		return false;
	if( !hitTestplaneAABB( &_frustum->rightPlane ) )
		return false;
	if( !hitTestplaneAABB( &_frustum->leftPlane ) )
		return false;
	return true;
}

float CullingNode::planeDotCoord(glm::vec4 *_plane, glm::vec3 *_pos)
{
	return _plane->x * _pos->x + _plane->y * _pos->y + _plane->z * _pos->z + _plane->w;
}
bool CullingNode::hitTestplaneAABB(glm::vec4 *_plane)
{
	//assert(min != max);
	if( planeDotCoord(_plane, &max) < 0 && planeDotCoord(_plane, &min) < 0)
	{
		glm::vec3 aCorner = glm::vec3(min.x, min.y, max.z);
		glm::vec3 bCorner = glm::vec3(max.x, max.y, min.z);
		if( planeDotCoord(_plane, &aCorner) < 0 && planeDotCoord(_plane, &bCorner) < 0 )
		{
			aCorner = glm::vec3(max.x, min.y, max.z);
			bCorner = glm::vec3(min.x, max.y, min.z);
			if( planeDotCoord(_plane, &aCorner) < 0 && planeDotCoord(_plane, &bCorner) < 0 )
			{
				aCorner = glm::vec3(max.x, min.y, min.z);
				bCorner = glm::vec3(min.x, max.y, max.z);
				if( planeDotCoord(_plane, &aCorner) < 0 && planeDotCoord(_plane, &bCorner) < 0 )
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool CullingNode::hitTestPoint(glm::vec3 _point)
{
	return (_point.x >= min.x && _point.x < max.x &&
			_point.y >= min.y && _point.y < max.y &&
			_point.z >= min.z && _point.z < max.z);
}

void CullingNode::setDimensions(glm::vec3 _min, glm::vec3 _max)
{
	min = _min;
	max = _max;
}

CullingNode::~CullingNode()
{
}
