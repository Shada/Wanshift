#include "AnimationLoader.h"

#define	SAFE_DELETE(x)	if( x ) { delete(x);	(x) = NULL; }

AnimationLoader::AnimationLoader()
{
	fbxManager = nullptr;
	outWeights = nullptr;
}


AnimationLoader::~AnimationLoader()
{
	fbxManager->Destroy();
	fbxManager = nullptr;
	SAFE_DELETE(outWeights);
}

float *AnimationLoader::loadFBX(const char* _filepath, Model *_model)
{
	SAFE_DELETE(outWeights);
	if(_model->getVertexAmount() == 0)
		return nullptr;
	weights.clear();
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

	// load matrices
	loadAnimationData(fbxScene, _model);

	fbxScene->Destroy();

	if(weights.size() == 0)
		return nullptr; 
	
	outWeights = new float[weights.size() * _model->getVertexAmount()];
	std::memset(outWeights, 0, sizeof(float)* weights.size() * _model->getVertexAmount());
	int index = 0;
	for(int i = 0; i < _model->getVertexAmount(); i++)
		for(unsigned int j = 0; j < weights.size(); j++)
			outWeights[index++] = weights[j][i];

	for(unsigned int j = 0; j < weights.size(); j++)
	{
		delete[] weights[j];
		weights[j] = nullptr;
	}

	return outWeights;
}

void AnimationLoader::loadAnimationData(FbxScene *_fbxScene, Model *_model)
{

	FbxNode *lRootNode = _fbxScene->GetRootNode();

	if(!lRootNode) return;

	loadNodeWeights(lRootNode, _model);

	FbxArray<FbxString*> lAnimStackNameArray;
	_fbxScene->FillAnimStackNameArray(lAnimStackNameArray);
	const int lAnimStackCount = lAnimStackNameArray.GetCount();
	FbxTime lperiod;
	lperiod.SetTime(0, 0, 0, 1, 0, _fbxScene->GetGlobalSettings().GetTimeMode());
	for(int i = 0; i < lAnimStackCount; i++)
	{
		FbxAnimStack * lCurrentAnimationStack = _fbxScene->FindMember<FbxAnimStack>(lAnimStackNameArray[i]->Buffer());
		if(!lCurrentAnimationStack)
			continue;
		fbxManager->GetAnimationEvaluator()->SetContext(lCurrentAnimationStack);

		FbxTakeInfo *lCurrentTake = _fbxScene->GetTakeInfo(*(lAnimStackNameArray[i]));

		FbxTime lstart, lstop;
		if(lCurrentTake)
		{
			lstart = lCurrentTake->mLocalTimeSpan.GetStart();
			lstop = lCurrentTake->mLocalTimeSpan.GetStop();
		}
		else
		{
			FbxTimeSpan ltimespan;
			_fbxScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(ltimespan);

			lstart = ltimespan.GetStart();
			lstop = ltimespan.GetStop();
		}

		int numFrames = (int)((lstop - lstart).GetFrameCount(_fbxScene->GetGlobalSettings().GetTimeMode()) / 1);

		_model->getAnimations().back().nFrames = numFrames;

		// thread. Private: lcurrentTime
		for(int k = 0; k < numFrames; k++) // and also thread this here
		{
			// load animation data and apply to vertices in a cool, yet civilization III, way.
			FbxTime lcurrentTime = lstart + lperiod * k;
			FbxAMatrix lDummyGlobalPosition;
			loadNodeAnimation(lRootNode, lcurrentTime, lDummyGlobalPosition, NULL, k, _model);
		}
	}
	lRootNode->Destroy();
}

void AnimationLoader::loadNodeWeights(FbxNode *_fbxNode, Model *_model)
{
	if(!_fbxNode) return;

	FbxNodeAttribute *attribute = _fbxNode->GetNodeAttribute();

	if(attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		loadMeshWeights(_fbxNode, _model);
	}

	const int lChildCount = _fbxNode->GetChildCount();
	for(int i = 0; i < lChildCount; ++i)
	{
		loadNodeWeights(_fbxNode->GetChild(i), _model);
	}
}

void AnimationLoader::loadMeshWeights(FbxNode *_fbxNode, Model *_model)
{
	FbxMesh *fbxMesh = (FbxMesh*)_fbxNode->GetNodeAttribute();

	int lskinCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	if(!lskinCount) return;

	int controlpointCount = fbxMesh->GetControlPointsCount();

	FbxSkin* lskinDeformer = (FbxSkin*)fbxMesh->GetDeformer(0, FbxDeformer::eSkin); // assume only one skin exists
	int lclusterCount = lskinDeformer->GetClusterCount();

	weights.resize(lclusterCount);

	for(int lclusterindex = 0; lclusterindex < lclusterCount; ++lclusterindex)
	{
		FbxCluster* lcluster = lskinDeformer->GetCluster(lclusterindex);
		if(!lcluster->GetLink())
			continue;

		double *lWeights = lcluster->GetControlPointWeights();
		int *wIndices = lcluster->GetControlPointIndices(); // these are the same for every frame.. Only need one weight per vertex, good. 
		int weightCount = lcluster->GetControlPointIndicesCount();
		float *currentClusterWeights = new float[controlpointCount];
		memset(currentClusterWeights, 0, controlpointCount * sizeof(float));
		for(int i = 0; i < weightCount; i++)
			currentClusterWeights[*wIndices++] = (float)*lWeights++;
		weights[lclusterindex] = currentClusterWeights;
	}

	// for the moment, we assume everything is in the same group... This might need to change in the future....
	int vCount = fbxMesh->GetPolygonVertexCount();
	int *vIndices = fbxMesh->GetPolygonVertices();

	for(int j = 0; j < lclusterCount; j++)
	{
		float* mapped = new float[vCount];
		for(int i = 0; i < vCount; i++)
		{
			mapped[i] = weights.at(j)[vIndices[i]];
		}
		float *t = weights[j];
		weights[j] = mapped;
		delete[] t;
		t = nullptr;
	}
	Animation a;
	a.nClusters = lclusterCount;
	_model->addAnimation(a);
}

void AnimationLoader::loadNodeAnimation(FbxNode *_fbxNode, FbxTime &_fbxTime, FbxAMatrix &_fbxParentGlobalPosition,
										FbxPose *_fbxPose, const int _frame, Model *_model)
{
	// if the node is null, return..
	if(!_fbxNode) return;

	FbxAMatrix lGlobalPosition = GetGlobalPosition(_fbxNode, _fbxTime, _fbxPose, &_fbxParentGlobalPosition);
	FbxNodeAttribute *attribute = _fbxNode->GetNodeAttribute();

	if(attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		// these are not used atm.. will check in on this..
		// Geometry offset. 
		// it is not inherited by the children.
		FbxAMatrix lGeometryOffset = GetGeometry(_fbxNode);
		FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;

		loadMeshAnimation(_fbxNode, _fbxTime, lGlobalPosition, _fbxPose, _model);
	}

	const int lChildCount = _fbxNode->GetChildCount();
	for(int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		loadNodeAnimation(_fbxNode->GetChild(lChildIndex), _fbxTime, lGlobalPosition, _fbxPose, _frame, _model);
	}
}

void AnimationLoader::loadMeshAnimation(FbxNode *_fbxNode, FbxTime &_fbxTime, FbxAMatrix &_fbxGlobalPosition, FbxPose *_fbxPose, Model *_model)
{
	FbxMesh *lMesh = _fbxNode->GetMesh();
	const int lSkinCount = lMesh->GetDeformerCount(FbxDeformer::eSkin);

	if(lSkinCount)
	{
		int lClusterCount = 0;
		for(int lSkinIndex = 0; lSkinIndex < lSkinCount; ++lSkinIndex)
			lClusterCount += ((FbxSkin*)(lMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin)))->GetClusterCount();
		if(lClusterCount)
			computeSkinDeformation(_fbxGlobalPosition, lMesh, _fbxTime, _fbxPose, _model);
	}
}

void AnimationLoader::computeSkinDeformation(FbxAMatrix &_fbxGlobalPosition, FbxMesh *_fbxMesh, FbxTime &_fbxTime, FbxPose *_fbxPose, Model *_model)
{
	FbxSkin * lSkinDeformer = (FbxSkin *)_fbxMesh->GetDeformer(0, FbxDeformer::eSkin);
	FbxSkin::EType lSkinningType = lSkinDeformer->GetSkinningType();

	if(lSkinningType == FbxSkin::eLinear || lSkinningType == FbxSkin::eRigid)
	{
		computeLinearDeformation(_fbxGlobalPosition, _fbxMesh, _fbxTime, _fbxPose, _model);
	}
}

void AnimationLoader::computeLinearDeformation(FbxAMatrix &_fbxGlobalPosition, FbxMesh* _fbxMesh, FbxTime& _fbxTime, FbxPose* _fbxPose, Model *_model)
{
	// get the cluster matrices and the weights
	std::vector<FbxAMatrix> allClusterMatrices;

	int lskinCount = _fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	if(!lskinCount) return;

	FbxSkin* lskinDeformer = (FbxSkin*)_fbxMesh->GetDeformer(0, FbxDeformer::eSkin); // assume only one skin exists

	int lclusterCount = lskinDeformer->GetClusterCount();
	allClusterMatrices.resize(lclusterCount);

	for(int lclusterindex = 0; lclusterindex < lclusterCount; ++lclusterindex)
	{
		FbxCluster* lcluster = lskinDeformer->GetCluster(lclusterindex);
		if(!lcluster->GetLink())
			continue;

		FbxAMatrix lvertexTransformMatrix;

		computeClusterDeformation(_fbxGlobalPosition, _fbxMesh, lcluster, lvertexTransformMatrix, _fbxTime, _fbxPose);
		allClusterMatrices[lclusterindex] = lvertexTransformMatrix;
	}

	FrameData f;
	glm::mat4 matrix;
	for(int i = 0; i < lclusterCount; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			matrix[j][0] = (float)allClusterMatrices[i].mData[j][0];
			matrix[j][1] = (float)allClusterMatrices[i].mData[j][1];
			matrix[j][2] = (float)allClusterMatrices[i].mData[j][2];
			matrix[j][3] = (float)allClusterMatrices[i].mData[j][3];
		}
		f.matrices.push_back(matrix);
	}
	_model->getAnimations().back().frameData.push_back(f);
}

void AnimationLoader::computeClusterDeformation(FbxAMatrix &_fbxGlobalPosition, FbxMesh *_fbxMesh, FbxCluster *_fbxCluster,
										   FbxAMatrix &_fbxVertexTransformMatrix, FbxTime &_fbxTime, FbxPose *_fbxPose)
{
	FbxCluster::ELinkMode lClusterMode = _fbxCluster->GetLinkMode();

	FbxAMatrix lReferenceGlobalInitPosition;
	FbxAMatrix lReferenceGlobalCurrentPosition;
	FbxAMatrix lAssociateGlobalInitPosition;
	FbxAMatrix lAssociateGlobalCurrentPosition;
	FbxAMatrix lClusterGlobalInitPosition;
	FbxAMatrix lClusterGlobalCurrentPosition;

	FbxAMatrix lReferenceGeometry;
	FbxAMatrix lAssociateGeometry;
	FbxAMatrix lClusterGeometry;

	FbxAMatrix lClusterRelativeInitPosition;
	FbxAMatrix lClusterRelativeCurrentPositionInverse;

	if(lClusterMode == FbxCluster::eAdditive && _fbxCluster->GetAssociateModel())
	{
		_fbxCluster->GetTransformAssociateModelMatrix(lAssociateGlobalInitPosition);
		// Geometric transform of the model
		lAssociateGeometry = GetGeometry(_fbxCluster->GetAssociateModel());
		lAssociateGlobalInitPosition *= lAssociateGeometry;
		lAssociateGlobalCurrentPosition = GetGlobalPosition(_fbxCluster->GetAssociateModel(), _fbxTime, _fbxPose);

		_fbxCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(_fbxMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;
		lReferenceGlobalCurrentPosition = _fbxGlobalPosition;

		// Get the link initial global position and the link current global position.
		_fbxCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		// Multiply lClusterGlobalInitPosition by Geometric Transformation
		lClusterGeometry = GetGeometry(_fbxCluster->GetLink());
		lClusterGlobalInitPosition *= lClusterGeometry;
		lClusterGlobalCurrentPosition = GetGlobalPosition(_fbxCluster->GetLink(), _fbxTime, _fbxPose);

		// Compute the shift of the link relative to the reference.
		//ModelM-1 * AssoM * AssoGX-1 * LinkGX * LinkM-1*ModelM
		_fbxVertexTransformMatrix = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse() *
			lClusterGlobalCurrentPosition * lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
	}
	else
	{
		_fbxCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		lReferenceGlobalCurrentPosition = _fbxGlobalPosition;
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(_fbxMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;

		// Get the link initial global position and the link current global position.
		_fbxCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		lClusterGlobalCurrentPosition = GetGlobalPosition(_fbxCluster->GetLink(), _fbxTime, _fbxPose);

		// Compute the initial position of the link relative to the reference.
		lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

		// Compute the current position of the link relative to the reference.
		lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

		// Compute the shift of the link relative to the reference.
		_fbxVertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
	}
}
FbxAMatrix AnimationLoader::GetGlobalPosition(FbxNode *_fbxNode, const FbxTime &_fbxTime, FbxPose *_fbxPose, FbxAMatrix *_fbxParentGlobalPosition)
{
	FbxAMatrix lGlobalPosition;
	bool        lPositionFound = false;

	if(_fbxPose)
	{
		int lNodeIndex = _fbxPose->Find(_fbxNode);

		if(lNodeIndex > -1)
		{
			// The bind pose is always a global matrix.
			// If we have a rest pose, we need to check if it is
			// stored in global or local space.
			if(_fbxPose->IsBindPose() || !_fbxPose->IsLocalMatrix(lNodeIndex))
			{
				lGlobalPosition = GetPoseMatrix(_fbxPose, lNodeIndex);
			}
			else
			{
				// We have a local matrix, we need to convert it to
				// a global space matrix.
				FbxAMatrix lParentGlobalPosition;

				if(_fbxParentGlobalPosition)
				{
					lParentGlobalPosition = *_fbxParentGlobalPosition;
				}
				else
				{
					if(_fbxNode->GetParent())
					{
						lParentGlobalPosition = GetGlobalPosition(_fbxNode->GetParent(), _fbxTime, _fbxPose);
					}
				}

				FbxAMatrix lLocalPosition = GetPoseMatrix(_fbxPose, lNodeIndex);
				lGlobalPosition = lParentGlobalPosition * lLocalPosition;
			}

			lPositionFound = true;
		}
	}

	if(!lPositionFound)
	{
		// There is no pose entry for that node, get the current global position instead.

		// Ideally this would use parent global position and local position to compute the global position.
		// Unfortunately the equation 
		//    lGlobalPosition = pParentGlobalPosition * lLocalPosition
		// does not hold when inheritance type is other than "Parent" (RSrs).
		// To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
		lGlobalPosition = _fbxNode->EvaluateGlobalTransform(_fbxTime);
	}

	return lGlobalPosition;
}

FbxAMatrix AnimationLoader::GetPoseMatrix(FbxPose *_fbxPose, int _nodeIndex)
{
	FbxAMatrix lPoseMatrix;
	FbxMatrix lMatrix = _fbxPose->GetMatrix(_nodeIndex);

	memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

	return lPoseMatrix;
}

// Get the geometry offset to a node. It is never inherited by the children.
FbxAMatrix AnimationLoader::GetGeometry(FbxNode *_fbxNode)
{
	const FbxVector4 lT = _fbxNode->GetGeometricTranslation(FbxNode::EPivotSet::eSourcePivot);
	const FbxVector4 lR = _fbxNode->GetGeometricRotation(FbxNode::EPivotSet::eSourcePivot);
	const FbxVector4 lS = _fbxNode->GetGeometricScaling(FbxNode::EPivotSet::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}