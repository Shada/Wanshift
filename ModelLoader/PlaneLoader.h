#pragma once
#include <vector>
#include <list>
#include "ModelLoaderInterface.h"
#include <fbxsdk.h>

struct PlaneVertex
{
	float pos[3];
	float texCoord[2];
};

class PlaneLoader :
	public ModelLoaderInterface
{
public:
	PlaneLoader();
	~PlaneLoader() override;

	/* this function loads a .fbx object
	*
	* #_filepath: the relative path of the fbx file to be loaded
	* #_model: the model where the mesh data (groups, materials &c.) will be stored
	*
	* #return: false whn the loading did not succeeded.
	*/
	float *loadFBX(const char* _filepath, Model *_model);


private:
	std::vector<PlaneVertex> verts;
	FbxManager		*fbxManager;

	/* reset function. If you want another group of models in a separate buffer, do reset before loading them. */
	void reset();


	/* fetch the mesh of the scene and store appropriate data in the model
	*
	* #_model: The output model.  The result of the loading will be stored here
	* #_fbxsdkScene: the scene wherefrom to import the scene */
	void loadMeshData(FbxScene *_fbxScene, Model *_model);

	/*fetch the mesh of a specified node.
	*
	* #_fbxsdkNode: The node that will be 
	* #_model: The model where the mesh data will be stored
	*/
	void fetchNodeMesh(FbxNode *_fbxNode, Model *_model);

	/* fetch all vertices for a group mesh
	*
	* #_fbsdkNode: The node were the mesh is stored
	* #_group: The group data will be stored here. */
	void fetchVerticesFromMesh(FbxNode *_fbxNode, Model *_model);

	/* fetch the material from a group mesh
	*
	* #_fbxsdkNode: The node in where the materials can be found
	* #_material: The material data will be stored here */
	void fetchMaterials(FbxNode *_fbxNode, Material &_material);

	/* read the surface material specified
	*
	* #_fbxsdkSurfaceMaterial: The surface material where material data can be found
	* #_material: The material data will be stored here. */
	void readMaterial(FbxSurfaceMaterial *_fbxSurfaceMaterial, Material &_material);

	/* here the textures of a material will be found and loaded
	*
	* #_fbxsdkProperty: The property where texture data can be found.
	* #_material; The texture data will be stored here. */
	void findMaterialTexture(FbxProperty *_fbxProperty, Material &_material);

};