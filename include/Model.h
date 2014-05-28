// TODO: add support for skeleton and animations
#pragma once
//#ifdef __DEBUG
//#include <gtest\gtest.h>
//#endif
#include <vector>
#include <glm/glm.hpp>

/* Material is used to define the maps and coefficients used by a group*/
struct Material
{
	Material()
	{
		//transparancyFactor = shininess = reflectionFactor = 0.f;
		diffuseMap = specularMap = normalMap = emissiveMap = -1;
		diffuseMapName = specularMapName = normalMapName = emissiveMapName = "";
	}
	int diffuseMap;
	int specularMap;
	int normalMap;
	int emissiveMap;
	
	std::string diffuseMapName;
	std::string specularMapName;
	std::string normalMapName;
	std::string emissiveMapName;
};

/* Groups is a part of the model that has its own material and is grouped together in the vertexBuffer */
struct Group
{
	Group()
	{
		startIndex = vertexAmount = 0;
	}
	int startIndex;
	int	vertexAmount;
	Material material;
	
	void changeMaterial(Material _material) { material = _material; }
	void setStartIndex(int _startIndex) { startIndex = _startIndex; }
};

struct FrameData
{
	std::vector<glm::mat4> matrices;
};
struct Animation
{
	Animation()
	{
		nFrames = nClusters = 0;
		weightBufferID = -1;
	}
	int weightBufferID;
	int nFrames;
	int nClusters;
	std::vector<FrameData> frameData;
};

class Model
{
public:
	Model()
	{
		vertexBufferID = -1;
		vertexAmount = 0;
		groupAmount = 0;
	}
	~Model(){}

	/* This function returns the id of the vertexBuffer where the model is located */
	int getVertexBufferID() { return vertexBufferID; }
	/* This function returns the total number of vertices this model consists of */
	int getVertexAmount() { return vertexAmount; }
	/* This function returns the number of groups this model consists of */
	unsigned int getGroupAmount() { return groupAmount; }
	/* This functions returns the list of the groups in the model */
	std::vector<Group> &getGroups() { return groups; }
	/* this function returns a specific group
	*
	* #_index: the index of the group that is to be returned
	*
	* #return: the Group at the specified index in the groups vector
	*
	* #error: throws error if index is out of bounds
	*/
	Group &getGroupAt(int _index)
	{
		if((signed)groupAmount <= _index || _index < 0)
		{
			throw "FATAL: out of bounds";
		}
		return groups[_index];
	}
	/* this function adds a group to the model
	*
	* #_g: the group that is to be added to the model
	*/
	void addGroup(Group _g)
	{
		groups.push_back(_g);
		vertexAmount += groups.back().vertexAmount;
		groupAmount++;
	}
	/* this functions sets the id if the vertex buffer wherein the model is loaded.
	*
	* #_vertexBufferID: the id of the buffer where the model is loaded
	*/
	void setVertexBufferID(int _vertexBufferID) { vertexBufferID = _vertexBufferID; }

	Animation &getAnimationAt(int _index) { return animations[_index]; }
	std::vector<Animation> &getAnimations() { return animations; }
	void addAnimation(Animation _anim) { animations.push_back(_anim); }
	int getAnimationCount() { return animations.size(); }
	

private:
	int	vertexBufferID;
	unsigned int vertexAmount;
	unsigned int groupAmount;

	std::vector<Group> groups;

	std::vector<Animation> animations;
};