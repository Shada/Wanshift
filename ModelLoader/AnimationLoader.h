#pragma once
#include <vector>
#include <list>
#include "ModelLoaderInterface.h"
#include <fbxsdk.h>

class AnimationLoader :
	public ModelLoaderInterface
{
public:
	AnimationLoader();
	~AnimationLoader() override;

	float *loadFBX(const char* _filePath, Model *_model);

private:

	std::vector<float*> weights;
	float *outWeights;
	FbxManager		*fbxManager;
	
	void loadAnimationData(FbxScene *_fbxScene, Model *_model);

	void loadNodeWeights(FbxNode *_fbxNode, Model *_model);

	void loadMeshWeights(FbxNode *_fbxNode, Model *_model);

	void loadNodeAnimation(FbxNode *_fbxNode, FbxTime &_fbxTime, FbxAMatrix &_fbxParentGlobalPosition,
						   FbxPose *_fbxPose, const int _frame, Model *_model);

	void loadMeshAnimation(FbxNode *_fbxNode, FbxTime &_fbxTime, FbxAMatrix &_fbxGlobalPosition,
						   FbxPose *_fbxPose, Model *_model);

	void computeClusterDeformation(FbxAMatrix &_fbxGlobalPosition, FbxMesh *_fbxsdkMesh, FbxCluster *_fbxCluster,
								   FbxAMatrix &_fbxVertexTransformMatrix, FbxTime &_fbxTime, FbxPose *_fbxPose);

	void computeLinearDeformation(FbxAMatrix &_fbxGlobalPosition, FbxMesh *_fbxMesh, FbxTime &_fbxTime, FbxPose *_fbxPose, Model *_model);

	void computeSkinDeformation(FbxAMatrix &_fbxGlobalPosition, FbxMesh *_fbxMesh, FbxTime &_fbxTime, FbxPose *_fbxPose, Model *_model);

	FbxAMatrix GetGlobalPosition(FbxNode *_fbxNode, const FbxTime &_fbxTime, FbxPose *_fbxPose = NULL,
								 FbxAMatrix *_fbxParentGlobalPosition = NULL);

	FbxAMatrix GetPoseMatrix(FbxPose *_fbxPose, int _nodeIndex);

	FbxAMatrix GetGeometry(FbxNode *_fbxNode);
};