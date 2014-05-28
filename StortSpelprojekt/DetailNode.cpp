#include "DetailNode.h"
#define MINIMUMDETAIL 50

DetailNode::DetailNode(RenderInterface *_graphics, std::vector< int > *_activeListLod0, std::vector< int > *_activeListLod1, std::vector< int > *_activeListLod2)
	:CullingNode(_graphics)
{
	activeListLod0 = _activeListLod0;
	activeListLod1 = _activeListLod1;
	activeListLod2 = _activeListLod2;
}
void DetailNode::addNode(DetailNode *_child)
{
	children.push_back(_child);
}

void DetailNode::draw(Frustum *_frustum)
{
	if(children.size() == 0)
	{
		//graphics->drawIndexed(chunk->iBufferAmount, chunk->iBufferStartID, 0);
	}
	else
		for(unsigned int i = 0; i < children.size(); i++)
		{
			if(children.at(i)->hitTestFrustum(_frustum))
				children.at(i)->draw(_frustum);
		}
}

bool DetailNode::addObject( ModelInstance *_object )
{
	if(children.size() > 0)
		for(UINT i = 0; i < children.size(); i++)
			addObject(_object);
	else if(hitTestPoint( _object->pos) )
		objects.push_back(_object);
	return true;
}

void DetailNode::buildTree(std::vector< ModelInstance *> *_objects)
{
	for(UINT i = 0; i < objects.size(); i++)
		if( hitTestPoint(_objects->at(i)->pos) )
		{
			objects.push_back( _objects->at(i) );
		}
	if(objects.size() > MINIMUMDETAIL)
	{
		for(UINT i = 0; i < 4; i++)
			children.push_back( new DetailNode( graphics, activeListLod0, activeListLod1, activeListLod2 ) );
		float xSize = ( max.x - min.x ) * 0.5f;
		float ySize = ( max.y - min.y ) * 0.5f;
		float zSize = ( max.z - min.z ) * 0.5f;
		children.at(0)->setDimensions(min,												glm::vec3(min.x + xSize, max.y, min.z + zSize) );
		children.at(1)->setDimensions(glm::vec3(min.x + xSize, max.y, min.z),			glm::vec3(max.x, max.y, min.z + zSize) );
		children.at(2)->setDimensions(glm::vec3(min.x, max.y, min.z + zSize),			glm::vec3(min.x + xSize, max.y, max.z) );
		children.at(3)->setDimensions(glm::vec3(min.x + xSize, max.y, min.z + zSize),	max );

		for(UINT i = 0; i < 4; i++)
			children.back()->buildTree(&objects);
	}
}

DetailNode::~DetailNode()
{
	for(UINT i = 0; i < children.size(); i++)
		SAFE_DELETE( children.at(i) );
	children.clear();
}
