#pragma once
#include "AspectInterface.h"
#include "Model.h"
class SkysphereAspect :
	public AspectInterface
{
public:
	SkysphereAspect();
	~SkysphereAspect();
	void init(MainState& _mainState)override;
	/* returns the vertices of the mesh.

	NOTE: if you insert the vertices together with all the other vertices for the other models,
	remember to use setStartIndex function to give the sphere model the correct start value
	*/
	std::vector<DefaultVertex> *getVertices() { return &vertices; }
	std::vector<int> *getIndices() { return &indices; }
	unsigned int getIndexAmount() { return amount; }

	void setVertexBufferID(int _index) { vertexBufferID = _index; }
	void setIndexBufferID(int _index) { indexBufferID = _index; }

	int getVertexBufferID() { return vertexBufferID; }
	int getIndexBufferID() { return indexBufferID; }

	/* when the and vertices are loaded into a vertexbuffer or indexbuffer, they can be cleared out. */
	void clearVertices() { vertices.clear(); }
	void clearIndices() { indices.clear(); }

	void runAspect(float _dt) override;

private:
	int vertexBufferID;
	int indexBufferID;
	unsigned int amount;

	/* this is the list of all vertices of the sphere. */
	std::vector<DefaultVertex> vertices;

	/* this is the list of indices for the sphere */
	std::vector<int> indices;

	/* this function generates a sphere to use with for the sky */
	void generateSphere();
};