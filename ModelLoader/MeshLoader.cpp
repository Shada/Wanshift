#include "MeshLoader.h"
#include <iostream>
#include <omp.h>
//#ifdef __DEBUG
//#include <gtest\gtest.h>
//#endif

typedef unsigned int uint;

MeshLoader::MeshLoader()
{
	fbxManager = nullptr;
}

MeshLoader::~MeshLoader()
{
	fbxManager->Destroy();
	fbxManager = nullptr;
}

/*TEST(MeshLoader, filePathTest)
{
	MeshLoader *ml = new MeshLoader(nullptr);
	Model m;
	bool result = ml->loadFBX("Models/Exported/DummyAnimation.fbx", m);
	EXPECT_EQ(true, result);
	result = ml->loadFBX("", m);
	EXPECT_EQ(false, result);

	SAFE_DELETE(ml);
}*/
float *MeshLoader::loadFBX(const char* _filepath, Model *_model)
{
	reset();
	if(std::strlen(_filepath) == 0)
		return nullptr;

	if(fbxManager == nullptr)
	{
		fbxManager = FbxManager::Create();
		FbxIOSettings *fbxIOSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
		fbxManager->SetIOSettings(fbxIOSettings);
	}

	FbxImporter *fbxImporter = FbxImporter::Create(fbxManager, "");

	// try to initialize FxbImporter. Throw exception if it fails.
	if(!fbxImporter->Initialize(_filepath, -1, fbxManager->GetIOSettings()))
		return nullptr;

	// create a new scene to store the imported file
	FbxScene *fbxScene = FbxScene::Create(fbxManager, _filepath);

	// import the contents of the file into the scene
	if(!fbxImporter->Import(fbxScene))
		return nullptr;

	// FbxImporter is no more needed since the file is imported, destroy it
	fbxImporter->Destroy();

	loadMeshData(fbxScene, _model);

	// destroy the scene, it's not used any more.
	fbxScene->Destroy();
	if(_model->getGroupAmount() <= 0)
		return nullptr;
	return &verts.data()->pos[0];
}

void MeshLoader::loadMeshData(FbxScene *_fbxScene, Model *_model)
{
	FbxNode *lRootNode = _fbxScene->GetRootNode();

	if(!lRootNode) return;

	// get the mesh of the scene recursively
	for(int i = 0; i < lRootNode->GetChildCount(); i++)
		fetchNodeMesh(lRootNode->GetChild(i), _model);

	lRootNode->Destroy();
}

void MeshLoader::fetchNodeMesh(FbxNode *_fbxNode, Model *_model)
{
	if(!_fbxNode) return; // if the node is null, return..

	// get the attribute type from the node.
	FbxNodeAttribute::EType lAttrType = _fbxNode->GetNodeAttribute()->GetAttributeType();

	if(!lAttrType) return; // if the node has no attribute, return

	if(lAttrType == FbxNodeAttribute::eMesh) // can only get the data if it's a mesh
	{
		// here we fetch the vertices.
		fetchVerticesFromMesh(_fbxNode, _model);

		// fetch material for the group
		fetchMaterials(_fbxNode, _model->getGroups().back().material);
	}
	
	// call this method recursively
	for(int i = 0; i < _fbxNode->GetChildCount(); i++)
		fetchNodeMesh(_fbxNode->GetChild(i), _model);
}

void MeshLoader::fetchVerticesFromMesh(FbxNode *_fbxNode, Model *_model)
{
	//------------------ Step 1 --------------------//
	// mesh is fetched from the node				//
	FbxMesh *lMesh = (FbxMesh*)_fbxNode->GetNodeAttribute();

	//------------------ Step 2 --------------------//
	// define and declare function variables		//
	int vertCount = lMesh->GetPolygonVertexCount();
	int *pIndices = nullptr;
	FbxVector4 *pVertices = nullptr;
	FbxArray<FbxVector4> pNormals;
	FbxArray<FbxVector2> pUV;
	bool normalExist = false;
	bool uvExist = false;
	Group g;

	//------------------ Step 3 --------------------//
	// vertices, normals, texcoords and indices		//
	// are fetched from the mesh					//
	pVertices = lMesh->GetControlPoints();

	pIndices = lMesh->GetPolygonVertices();

	normalExist = lMesh->GetPolygonVertexNormals(pNormals);

	FbxStringList uvnamelist;
	lMesh->GetUVSetNames(uvnamelist);
	if(uvnamelist.GetCount())
		uvExist = lMesh->GetPolygonVertexUVs(uvnamelist.GetItemAt(0)->mString, pUV);

	//------------------ Step 4 --------------------//
	// copy vertexdata to vert vector				//

	g.startIndex = verts.size();
	verts.resize(verts.size() + vertCount);

	for(uint i = g.startIndex; i < verts.size(); i++)
	{
		FbxVector4 vertex = pVertices[*pIndices++];
		verts.at(i).pos[0] = (float)vertex.mData[0];
		verts.at(i).pos[1] = (float)vertex.mData[1];
		verts.at(i).pos[2] = (float)vertex.mData[2];
	}
	uint j = 0;
	if(normalExist)
		for(uint i = g.startIndex; i < verts.size(); i++)
		{
			verts.at(i).normal[0] = (float)pNormals[j].mData[0];
			verts.at(i).normal[1] = (float)pNormals[j].mData[1];
			verts.at(i).normal[2] = (float)pNormals[j++].mData[2];
		}
	if(uvExist)
	{
		j = 0;
		for(uint i = g.startIndex; i < verts.size(); i++)
		{
			verts.at(i).texCoord[0] = (float)pUV[j].mData[0];
			verts.at(i).texCoord[1] = 1 - (float)pUV[j++].mData[1];
		}
	}

	//------------------ Step 5 --------------------//
	// copy vertexdata to vert vector				//
	g.vertexAmount = vertCount;
	_model->addGroup(g);
}

void MeshLoader::fetchMaterials(FbxNode *_fbxNode, Material &_material)
{
	int nMaterials = _fbxNode->GetMaterialCount(); // get the number of materials in this node.

	if(nMaterials <= 0) return; // no materials ? return.

	FbxSurfaceMaterial *lMaterial = _fbxNode->GetMaterial(0); // we only support one material per group

	readMaterial(lMaterial, _material); // read the material
}

void MeshLoader::readMaterial(FbxSurfaceMaterial *_fbxSurfaceMaterial, Material &_material)
{
	FbxProperty lProperty;

	if(!_fbxSurfaceMaterial) return;

	// go through possible textures
	int texIndex;
	FBXSDK_FOR_EACH_TEXTURE(texIndex)
	{
		lProperty = _fbxSurfaceMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[texIndex]);
		findMaterialTexture(&lProperty, _material);
	}
	FbxPropertyT<FbxDouble3> fbxDouble3;
	FbxPropertyT<FbxDouble> fbxDouble1;
	if(_fbxSurfaceMaterial->GetClassId().Is(FbxSurfacePhong::ClassId)) // Phong Material
	{
		printf("Loading Phong Material.\n");

		//// ambient, diffuse, specular, emissive, transparacy, shininess and reflectivity can be found here
		//fbxDouble3 = ((FbxSurfacePhong*)_fbxSurfaceMaterial)->Ambient;
		//_material.ka = glm::vec3((float)fbxDouble3.Get()[0], (float)fbxDouble3.Get()[1],
		//						 (float)fbxDouble3.Get()[2]);
		//fbxDouble3 = ((FbxSurfacePhong*)_fbxSurfaceMaterial)->Diffuse;
		//_material.kd = glm::vec3((float)fbxDouble3.Get()[0], (float)fbxDouble3.Get()[1],
		//						 (float)fbxDouble3.Get()[2]);
		//fbxDouble3 = ((FbxSurfacePhong*)_fbxSurfaceMaterial)->Emissive;
		//_material.ke = glm::vec3((float)fbxDouble3.Get()[0], (float)fbxDouble3.Get()[1],
		//						 (float)fbxDouble3.Get()[2]);
		//fbxDouble3 = ((FbxSurfacePhong*)_fbxSurfaceMaterial)->Specular;
		//_material.ks = glm::vec3((float)fbxDouble3.Get()[0], (float)fbxDouble3.Get()[1],
		//						 (float)fbxDouble3.Get()[2]);
		//fbxDouble1 = ((FbxSurfacePhong*)_fbxSurfaceMaterial)->TransparencyFactor;
		//_material.transparancyFactor = (float)fbxDouble1.Get();
		//fbxDouble1 = ((FbxSurfacePhong*)_fbxSurfaceMaterial)->Shininess;
		//_material.shininess = (float)fbxDouble1.Get();
		//fbxDouble1 = ((FbxSurfacePhong*)_fbxSurfaceMaterial)->ReflectionFactor;
		//_material.reflectionFactor = (float)fbxDouble1.Get();
	}
	else if(_fbxSurfaceMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId)) // Lambert Material
	{
		printf("Loading Lambert Material.\n");

		//// ambient, diffuse, emissive and opacity can be found here
		//fbxDouble3 = ((FbxSurfaceLambert*)_fbxSurfaceMaterial)->Ambient;
		//_material.ka = glm::vec3((float)fbxDouble3.Get()[0], (float)fbxDouble3.Get()[1],
		//						 (float)fbxDouble3.Get()[2]);
		//fbxDouble3 = ((FbxSurfaceLambert*)_fbxSurfaceMaterial)->Diffuse;
		//_material.kd = glm::vec3((float)fbxDouble3.Get()[0], (float)fbxDouble3.Get()[1],
		//						 (float)fbxDouble3.Get()[2]);
		//fbxDouble3 = ((FbxSurfaceLambert*)_fbxSurfaceMaterial)->Emissive;
		//_material.ke = glm::vec3((float)fbxDouble3.Get()[0], (float)fbxDouble3.Get()[1],
		//						 (float)fbxDouble3.Get()[2]);
		//fbxDouble1 = ((FbxSurfaceLambert*)_fbxSurfaceMaterial)->TransparencyFactor;
		//_material.transparancyFactor = (float)fbxDouble1.Get();
	}
	else
		printf("Unknown material.\n");
}

void MeshLoader::findMaterialTexture(FbxProperty *_fbxProperty, Material &_material)
{
	if(!_fbxProperty->IsValid()) return;

	// how many textures are included in the material?
	int texCount = _fbxProperty->GetSrcObjectCount<FbxTexture>();
	for(int j = 0; j < texCount; ++j)
	{
		// it's not a layered texture. get on with you business.
		FbxTexture *fbxTexture = _fbxProperty->GetSrcObject<FbxTexture>(j);
		if(!fbxTexture) return;
		// fetch the texture, find out what kind of texture it is and set it to the material. 

		FbxFileTexture *fbxFileTexture = FbxCast<FbxFileTexture>(fbxTexture);

		if(fbxFileTexture)
		{
			FbxString fbxstring = _fbxProperty->GetName();
			FBXSDK_printf("property name: %s\n", fbxstring.Buffer());
			
			std::string filePath = fbxFileTexture->GetFileName();
		
			if(fbxstring == FbxSurfaceMaterial::sDiffuse)
				_material.diffuseMapName = filePath;
			else if(fbxstring == FbxSurfaceMaterial::sSpecular)
				_material.specularMapName = filePath;
			else if(fbxstring == FbxSurfaceMaterial::sNormalMap)
				_material.normalMapName = filePath;
			else if(fbxstring == FbxSurfaceMaterial::sEmissive)
				_material.emissiveMapName = filePath;
		}
	}
}

void MeshLoader::reset()
{
	verts.clear();
}