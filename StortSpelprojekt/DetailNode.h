#pragma once
#include "CullingNode.h"
#include "ForestContainer.h"
class DetailNode :
	public CullingNode
{
private:
	std::vector< int >	*activeListLod0; // pointer to the index buffer to draw objects with level of detail 0.
	std::vector< int >	*activeListLod1; // pointer to the index buffer to draw objects with level of detail 1.
	std::vector< int >	*activeListLod2; // pointer to the index buffer to draw objects with level of detail 2.
	std::vector< ModelInstance *> objects; // list of pointers to objects that are inside this node.
public:
	//child nodes of this node.
	std::vector<DetailNode*> children;
	DetailNode(RenderInterface *_graphics, std::vector< int > *_activeListLod0, std::vector< int > *_activeListLod1, std::vector< int > *_activeListLod2);
	/* adds an object to the node or its children if it has children
	#_object is the object to be inserted into the node
	#returns true if the object was placed inside, and returs false if for example the object's coordinates are outside the node*/
	bool addObject( ModelInstance *_object );
	/* adds a child node to the node
	#_child is the new child node to be added.*/
	void addNode(DetailNode *_child);
	/* draws
	#_frustum is the camera frustum*/
	void draw(Frustum *_frustum);

	void buildTree(std::vector< ModelInstance *> *_objects);
	~DetailNode();
};
