#include "CascadeShadow.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

CascadeShadow::CascadeShadow()
{
	Containers containers(std::make_pair(Containers::CONSTBUFFER, MODIFY));
	cbuffers = containers.get<ConstantBufferContainer>(Containers::CONSTBUFFER);

}


CascadeShadow::~CascadeShadow()
{
}

void CascadeShadow::init(RenderInterface *_graphics)
{
	graphics = _graphics;
	int w = (int)graphics->getResolution().x;
	int h = (int)graphics->getResolution().y;

	cbLightInfoIndex = graphics->getHandleIndex("cbLightInfo", ResourceType::ConstantBuffer);
	cbAnimationMatricesIndex = graphics->getHandleIndex("cbAnimationMatrices", ResourceType::ConstantBuffer);
	cbWorldIndex = graphics->getHandleIndex("cbWorld", ResourceType::ConstantBuffer);

	shadowmapIndex = new ShaderClass(		"shadowVS",
											"shadowGS",
											"",
											"",
											"",
											graphics,
											(int)LayoutTypes::LAYOUT_DEFAULT,
											false);

	shadowcharIndex = new ShaderClass(		"shadowCharVS",
											"shadowCharGS",
											"",
											"",
											"",
											graphics,
											(int)LayoutTypes::LAYOUT_DEFAULT,
											false);


	shadowmapCompileIndex = new ShaderClass("shadowCompileVS",
											"shadowCompileGS",
											"",
											"",
											"shadowCompilePS",
											graphics,
											(int)LayoutTypes::LAYOUT_DEFAULT,
											false);

	shadowTreeIndex = new ShaderClass(		"treeShadowVS",
											"treeShadowGS",
											"",
											"",
											"",
											graphics,
											(int)LayoutTypes::LAYOUT_DEFAULT,
											false);

	//-----------------------------------------------------------------------------
	// Create the compute shader resources
	computeIndex = graphics->createComputeShader("shadowComputeCS");

	computeTexture = graphics->createTexture2D((int)graphics->getResolution().x, (int)graphics->getResolution().y);
	computeUAV = graphics->createUAVTextureView(computeTexture);
	computeSRV = graphics->createShaderResourceView(computeTexture);
	graphics->addHandle("computetexture", ResourceType::ShaderResourceView, computeSRV);


	//----------------------------------------------------------------
	// set container
	shadowConstants.textureName = "shadowcompile";
	shadowConstants.gridWidth = 45;
	shadowConstants.gridHeight = 25;
	//--------------------------------------------------------------
	// Create depth texture for the shadowmap
	shadowTextureArray = graphics->createDepthTexture2DArray(SHADOWMAPSIZE, SHADOWMAPSIZE, NROFCASCADES);
	shadowDepthStencilViewArray = graphics->createDepthRenderTargetViewArray(shadowTextureArray, NROFCASCADES);
	shadowShaderResourceViewArray = graphics->createDepthShaderResourceViewArray(shadowTextureArray, NROFCASCADES);
	graphics->addHandle("shadowmapsrv", ResourceType::ShaderResourceViewArray, shadowShaderResourceViewArray);

	//--------------------------------------------------------------------------------------------------
	// Create the resources for the shadowCompile
	//shadowCompileTexture = graphics->createUABuffer((void**)&glm::vec4(), graphics->getResolution().x*graphics->getResolution().y, 4);
	shadowCompileTexture = graphics->createTexture2D((int)graphics->getResolution().x, (int)graphics->getResolution().y);
	shadowCompileRenderTarget = graphics->createRenderTargetView(shadowCompileTexture);
	shadowCompileResourceView = graphics->createShaderResourceView(shadowCompileTexture);
	graphics->addHandle(shadowConstants.textureName, ResourceType::ShaderResourceView, shadowCompileResourceView);


	//---------------------------------------------------------------------
	//Lights initiate kanske inte ska användas senare

	// TODO: Move to correct place, should not be here...
	


	glm::mat4 rotatesun = glm::rotate(glm::mat4(), 80.0f, glm::vec3(0, 0, 1));
	cbuffers->cbLight.pos[0] = cbuffers->cbLight.pos[0] * rotatesun;

	glm::vec4 sunDir(0, 1, 0, 0);
	sunDir = glm::normalize(sunDir);

	lightDir = sunDir;
	cbuffers->cbLight.lightDir = glm::vec3(lightDir);

//	glm::vec3 sunUp = glm::cross(glm::cross(glm::vec3(0, 1, 0), sunDir), sunDir);
	graphics->updateCBuffer(cbLightInfoIndex, &cbuffers->cbLight);

	
	//---------------------------------------------------------------
	//		Init frustum scales

	frustumScales[0] = 1;
	frustumScales[1] = 10;
	frustumScales[2] = 50;
	frustumScales[3] = 100;
	frustumScales[4] = 200;

	//----------------------------------------------------------------
	//		Nulling center and scale
	for (int i = 0; i < NROFCASCADES; i++)
	{
		cascadeSphereCenter[i] = glm::vec3(0.0f, 0.0f, 0.0f);
		cascadeSphereRadie[i] = 0.0f;
	}
}



void CascadeShadow::shadowing()
{
	//if (timeOfDay * 24 > 4 && timeOfDay < 22) // onödigt att skugga på natten som vi har det just nu
	{
	
		update();

		//******************************//
		//			models				//
		//******************************//
		graphics->setRenderTargetViews(-1, 0, shadowDepthStencilViewArray);
		//graphics->setViewportArray(SHADOWMAPSIZE, SHADOWMAPSIZE, NROFCASCADES);
		graphics->setViewport(SHADOWMAPSIZE, SHADOWMAPSIZE);

		draw();

		graphics->setViewport((int)graphics->getResolution().x, (int)graphics->getResolution().y);

	}
}

void CascadeShadow::updateLight(float _dt)
{
	timeOfDay = cbuffers->cbEveryFrame.timeofDay;
	glm::mat4 rotatesun;
	if (cbuffers->cbEveryFrame.dayBool.x >= 1.0f)
		rotatesun = glm::rotate(glm::mat4(), timeOfDay * 180, glm::vec3(0, 0, 1));
	else
		rotatesun = glm::rotate(glm::mat4(), 180 + ((1 - timeOfDay) * 180), glm::vec3(0, 0, 1));


	cbuffers->cbLight.lightDir = glm::vec3(lightDir * rotatesun);
	cbuffers->cbLight.lightDir = glm::normalize(cbuffers->cbLight.lightDir);

	//cbuffers->cbLight.pos[0] = glm::vec4(player->position,1) - lightDir * 10000;

	glm::vec3 vDirectionalDir = cbuffers->cbLight.lightDir;
	// Find the view matrix
	glm::vec3 vWorldCenter = glm::vec3(camera->vEye) + camera->vLookDir * frustumScales[4] * 0.5f;
	glm::vec3 vPos = vWorldCenter;
	glm::vec3 vLookAt = vWorldCenter + vDirectionalDir * 30.0f;
	glm::vec3 vUp;
	glm::vec3 vRight = glm::vec3(1.0f, 0.0f, 0.0f);
	vUp = glm::cross(vDirectionalDir, vRight);
	vUp = glm::normalize(vUp);
	glm::mat4 mShadowView;
	mShadowView = glm::lookAt(vPos, vLookAt, vUp);

	cbuffers->cbLight.lightView = camera->viewMatrix;

	cbuffers->cbLight.lightProj = camera->projMatrix;

	cbuffers->cbLight.CascadeViewProj[0] = cbuffers->cbLight.lightProj * cbuffers->cbLight.lightView;
	cbuffers->cbLight.CascadeViewProj[1] = cbuffers->cbLight.CascadeViewProj[0];
	cbuffers->cbLight.CascadeViewProj[2] = cbuffers->cbLight.CascadeViewProj[0];
	cbuffers->cbLight.CascadeViewProj[3] = cbuffers->cbLight.CascadeViewProj[0];

	graphics->updateCBuffer(cbLightInfoIndex, &cbuffers->cbLight);


}

void CascadeShadow::draw()
{
	UINT stride;
	UINT offset;
	glm::mat4 matrix;
	CBWorldData world;


	shadowcharIndex->use();


	graphics->setDepthStencilState(0);
	graphics->setBlendState(0);

	stride = sizeof(DefaultVertex);
	offset = 0;

	graphics->setVertexBuffers(0, player->playerModel.getVertexBufferID(), 1, &stride, &offset);

	stride = sizeof(float)* 15;

	graphics->setVertexBuffers(1, player->playerModel.getAnimationAt(0).weightBufferID, 1, &stride, &offset);

	graphics->setInputLayout((int)LayoutTypes::LAYOUT_ANIMATION);

	graphics->setTopology(0);
	graphics->setRasterState(1);

	graphics->setCBuffers(5, 1, cbAnimationMatricesIndex);
	CBAnimationMatrices cbanim;

	for (uint i = 0; i < player->playerModel.getGroupAmount(); i++)
	{
		matrix = glm::translate(glm::mat4x4(), glm::vec3(player->position.x, player->position.y - 15.0f, player->position.z));
		matrix = glm::rotate(matrix, player->rot * 180 / 3.14f, glm::vec3(0.f, 1.f, 0.f));
		matrix = glm::scale(matrix, glm::vec3(1.5f, 1.5f, 1.5f));
		world.mWorld = matrix;
		world.mWorldInv = matrix;

		graphics->updateCBuffer(cbWorldIndex, &world);

		cbanim.interpolation[0] = player->animationInterpolation;
		for(int j = 0; j < 15; j++)
		{
			cbanim.mat1[j] = player->playerModel.getAnimationAt(player->animationState).frameData.at(player->prevAnimationFrame).matrices[j];
			cbanim.mat2[j] = player->playerModel.getAnimationAt(player->animationState).frameData.at(player->animationFrame).matrices[j];
		}
		
		graphics->updateCBuffer(cbAnimationMatricesIndex, &cbanim);

		int startIndex = player->playerModel.getGroupAt(i).startIndex;
		int amount = player->playerModel.getGroupAt(i).vertexAmount;
		graphics->draw(amount, startIndex);
	}


	//**********************************//
	//				terrain				//
	//**********************************//	

	//stride = sizeof(DefaultVertex);
	//offset = 0;

	//shadowmapIndex->use();
	//matrix = glm::mat4x4();

	//world.mWorld = matrix;
	//world.mWorldInv = matrix;

	//graphics->updateCBuffer(0, &world);

	//graphics->setInputLayout((int)LayoutTypes::LAYOUT_DEFAULT);


	//// Set the vertex buffers
	//graphics->setRasterState(0);

	////// Set the vertex buffers
	graphics->setVertexBuffers(0, terrain->bufferID, 1, &stride, &offset);
	graphics->setIndexBuffer(terrain->indexBufferID, offset);


	//// fix the shadow frustum culling later
	////ExtractFrustumPoints();

	//terrain->tesselationchunks.clear();
	//terrain->smallNodes.at(0).draw(&camera->frustum);


	////terrain->mediumNodes.at(0).draw(&camera->frustum);

	////terrain->largeNodes.at(0).draw(&camera->frustum);

	//for (uint i = 0; i < terrain->tesselationchunks.size(); i++)
	//	graphics->drawIndexed(terrain->nVerticesPerTiny / (TINYSPLITTING*TINYSPLITTING), terrain->tesselationchunks.at(i), 0);

	//------------------------------------------------------
	// Draw forest
	//shadowTreeIndex->use();
	//forest->drawShadow();

}

void CascadeShadow::ExtractFrustumPoints()
{

	float clip[16];

	clip[0] = globalShadowView[0].x * globalShadowProjection[0].x + globalShadowView[0].z * globalShadowProjection[2].x;
	clip[1] = globalShadowView[0].y * globalShadowProjection[1].y + globalShadowView[0].z * globalShadowProjection[2].y;
	clip[2] = globalShadowView[0].z * globalShadowProjection[2].z;
	clip[3] = globalShadowView[0].z * globalShadowProjection[2].w;

	clip[4] = globalShadowView[1].x * globalShadowProjection[0].x + globalShadowView[1].z * globalShadowProjection[2].x;
	clip[5] = globalShadowView[1].y * globalShadowProjection[1].y + globalShadowView[1].z * globalShadowProjection[2].y;
	clip[6] = globalShadowView[1].z * globalShadowProjection[2].z;
	clip[7] = globalShadowView[1].z * globalShadowProjection[2].w;

	clip[8] = globalShadowView[2].x * globalShadowProjection[0].x + globalShadowView[2].z * globalShadowProjection[2].x;
	clip[9] = globalShadowView[2].y * globalShadowProjection[1].y + globalShadowView[2].z * globalShadowProjection[2].y;
	clip[10] = globalShadowView[2].z * globalShadowProjection[2].z;
	clip[11] = globalShadowView[2].z * globalShadowProjection[2].w;

	clip[12] = globalShadowView[3].x * globalShadowProjection[0].x + globalShadowView[3].z * globalShadowProjection[2].x;
	clip[13] = globalShadowView[3].y * globalShadowProjection[1].y + globalShadowView[3].z * globalShadowProjection[2].y;
	clip[14] = globalShadowView[3].z * globalShadowProjection[2].z + globalShadowView[3].w * globalShadowProjection[3].z;
	clip[15] = globalShadowView[3].z * globalShadowProjection[2].w;

	// Left plane
	frustum.leftPlane.x = clip[3] + clip[0];
	frustum.leftPlane.y = clip[7] + clip[4];
	frustum.leftPlane.z = clip[11] + clip[8];
	frustum.leftPlane.w = clip[15] + clip[12];

	// Right plane
	frustum.rightPlane.x = clip[3] - clip[0];
	frustum.rightPlane.y = clip[7] - clip[4];
	frustum.rightPlane.z = clip[11] - clip[8];
	frustum.rightPlane.w = clip[15] - clip[12];

	// Bottom plane
	frustum.botPlane.x = clip[3] + clip[1];
	frustum.botPlane.y = clip[7] + clip[5];
	frustum.botPlane.z = clip[11] + clip[9];
	frustum.botPlane.w = clip[15] + clip[13];

	// Top plane
	frustum.topPlane.x = clip[3] - clip[1];
	frustum.topPlane.y = clip[7] - clip[5];
	frustum.topPlane.z = clip[11] - clip[9];
	frustum.topPlane.w = clip[15] - clip[13];

	// Near plane
	frustum.nearPlane.x = clip[3] + clip[2];
	frustum.nearPlane.y = clip[7] + clip[6];
	frustum.nearPlane.z = clip[11] + clip[10];
	frustum.nearPlane.w = clip[15] + clip[14];

	// Far plane
	frustum.farPlane.x = clip[3] - clip[2];
	frustum.farPlane.y = clip[7] - clip[6];
	frustum.farPlane.z = clip[11] - clip[10];
	frustum.farPlane.w = clip[15] - clip[14];




	//// Get the camera bases
	//const glm::vec3 camPos = glm::vec3(camera->vEye);
	//const glm::vec3 camUp = glm::vec3(camera->vUp);
	//const glm::vec3 camForward = camera->vLookDir;
	//const glm::vec3 camRight = glm::cross(glm::vec3(camUp), camForward);

	//// Calculate the tangent values (this can be cached
	//const float fTanFOVX = tanf(camera->aspectRatio * camera->FOV);
	//const float fTanFOVY = tanf(camera->aspectRatio);

	//// Calculate the points on the near plane
	//arrFrustumCorners[0] = camPos + (-camRight * fTanFOVX + camUp * fTanFOVY + camForward) * frustumScales[0];
	//arrFrustumCorners[1] = camPos + (camRight * fTanFOVX + camUp * fTanFOVY + camForward) * frustumScales[0];
	//arrFrustumCorners[2] = camPos + (camRight * fTanFOVX - camUp * fTanFOVY + camForward) * frustumScales[0];
	//arrFrustumCorners[3] = camPos + (-camRight * fTanFOVX - camUp * fTanFOVY + camForward) * frustumScales[0];

	//// Calculate the points on the far plane
	//arrFrustumCorners[4] = camPos + (-camRight * fTanFOVX + camUp * fTanFOVY + camForward) * frustumScales[1];
	//arrFrustumCorners[5] = camPos + (camRight * fTanFOVX + camUp * fTanFOVY + camForward) * frustumScales[1];
	//arrFrustumCorners[6] = camPos + (camRight * fTanFOVX - camUp * fTanFOVY + camForward) * frustumScales[1];
	//arrFrustumCorners[7] = camPos + (-camRight * fTanFOVX - camUp * fTanFOVY + camForward) * frustumScales[1];
}

void CascadeShadow::ExtractFrustumBoundSphere(float fNear, float fFar, glm::vec3& vBoundCenter, float& fBoundRadius)
{
	// Get the camera bases
	const glm::vec3 camPos = glm::vec3(camera->vEye);
	const glm::vec3 camUp = glm::vec3(camera->vUp);
	const glm::vec3 camForward = camera->vLookDir;
	const glm::vec3 camRight = glm::vec3(camera->vRight);

	// Calculate the tangent values (this can be cached as long as the FOV doesn't change)
	const float fTanFOVX = tanf(camera->aspectRatio * (camera->FOV/180 * 3.14f));
	const float fTanFOVY = tanf(camera->aspectRatio);

	// The center of the sphere is in the center of the frustum
	vBoundCenter = camPos + camForward * (fNear + 0.5f * (fNear + fFar));

	// Radius is the distance to one of the frustum far corners
	glm::vec3 temp1 = -camRight * fTanFOVX;
	glm::vec3 temp2 = camUp * fTanFOVY;
	glm::vec3 temp3 = (temp1 + temp2 + camForward);
	glm::vec3 temp4 = temp3 * fFar;

	glm::vec3 tempfar = camPos + (-camRight * fTanFOVX + camUp * fTanFOVY + camForward) * fFar;
	glm::vec3 tempnear = camPos + (-camRight * fTanFOVX + camUp * fTanFOVY + camForward) * fNear;

	glm::vec3 vBoundSpan = camPos + temp4 - vBoundCenter;

	fBoundRadius = glm::length(vBoundSpan);
}

bool CascadeShadow::cascadeNeedsUpdate(glm::mat4 mShadowView, int iCascadeIdx, glm::vec3* newCenter, glm::vec3* vOffset)
{
	// Find the offset between the new and old bound ceter
	glm::vec4 vOldCenterInCascade;
	vOldCenterInCascade = mShadowView * glm::vec4(cascadeSphereCenter[iCascadeIdx], 1); //----------------------------------------------------------------
	//vOldCenterInCascade = glm::vec4(cascadeSphereCenter[iCascadeIdx], 1) * mShadowView; 
	glm::vec3 vNewCenterInCascade;
	vNewCenterInCascade = glm::vec3(mShadowView * glm::vec4(newCenter->x, newCenter->y, newCenter->z, 1)); //------------------------------------------------------------------------
	//vNewCenterInCascade = glm::vec3( glm::vec4(newCenter, 1) * mShadowView); 


	glm::vec3 vCenterDiff = vNewCenterInCascade - glm::vec3(vOldCenterInCascade);

	// Find the pixel size based on the diameters and map pixel size
	float fPixelSize = (float)SHADOWMAPSIZE / (2.0f * cascadeSphereRadie[iCascadeIdx]);

	float fPixelOffX = vCenterDiff.x * fPixelSize;
	float fPixelOffY = vCenterDiff.y * fPixelSize;

	//Check if the center moved at least half a pixel unit
	bool bNeedUpdate = abs(fPixelOffX) > 0.5f || abs(fPixelOffY) > 0.5f;
	if (bNeedUpdate)
	{
		// Round to the 
		vOffset->x = floorf(0.5f + fPixelOffX) / fPixelSize;
		vOffset->y = floorf(0.5f + fPixelOffY) / fPixelSize;
		vOffset->z = vCenterDiff.z;
	}

	return bNeedUpdate;
}

void CascadeShadow::update()
{
	glm::vec3 vDirectionalDir = cbuffers->cbLight.lightDir;
	// Find the view matrix
	glm::vec3 vWorldCenter = glm::vec3(camera->vEye) + camera->vLookDir * (frustumScales[4] * 0.4f);
	glm::vec3 vPos = player->position;
	glm::vec3 vLookAt = player->position + vDirectionalDir*30.0f;
	glm::vec3 vUp = glm::vec3(0,1,0);
	glm::vec3 vRight = glm::vec3(camera->vRight);
	vUp = glm::cross(vDirectionalDir, vUp);
	vUp = glm::normalize(vRight);
	glm::mat4 mShadowView;
	mShadowView = glm::lookAt(vPos, vLookAt, vUp);
	
	//mShadowView = camera->viewMatrix;

	//mShadowView = lightTest.lightView;
	cbuffers->cbLight.lightView = mShadowView;
	globalShadowView = mShadowView;
	// Get the bounds for the shadow space
	float radie;
	ExtractFrustumBoundSphere(frustumScales[0], frustumScales[4], shadowSphereCenter, radie);
	shadowSphereRadie = glm::max(shadowSphereRadie, radie); // Expend the radius to compensate for numerical errors
	// Find the projection matrix
	glm::mat4 mShadowProj;
	//mShadowProj = glm::ortho(-shadowSphereRadie, shadowSphereRadie, -shadowSphereRadie, shadowSphereRadie, -shadowSphereRadie, shadowSphereRadie);
	mShadowProj = orthoCalc(shadowSphereRadie, shadowSphereRadie, -shadowSphereRadie, shadowSphereRadie);
	globalShadowProjection = mShadowProj;
	// The combined transformation from world to shadow space
	worldToShadow = mShadowProj * mShadowView;

	glm::mat4 shadowViewInv;
	shadowViewInv = glm::inverse(mShadowView);

	for (int iCascadeIdx = 0; iCascadeIdx < NROFCASCADES; iCascadeIdx++)
	{
		glm::mat4 cascadeTrans;
		glm::mat4 cascadeScale;

		// To avoid anti flickering we need to make the transformation invariant to camera rotation and translation
		// By encapsulating the cascade frustum with a sphere we achive the rotation invariance
		glm::vec3 vNewCenter;
		ExtractFrustumBoundSphere(frustumScales[iCascadeIdx], frustumScales[iCascadeIdx + 1], vNewCenter, radie);
		cascadeSphereRadie[iCascadeIdx] = glm::max(cascadeSphereRadie[iCascadeIdx], radie); // Expend the radius to compensate for numerical errors
		
		// Only update the cascade bounds if it moved at least a full pixel unit
		// This makes the transformation invariant to translation
		glm::vec3 vOffset;
		if (cascadeNeedsUpdate(mShadowView, iCascadeIdx, &vNewCenter, &vOffset))
		{
			localShadowProjection[iCascadeIdx] = orthoCalc(cascadeSphereRadie[iCascadeIdx], cascadeSphereRadie[iCascadeIdx], -cascadeSphereRadie[iCascadeIdx], cascadeSphereRadie[iCascadeIdx]);

		}

	}

	cbuffers->cbLight.CascadeViewProj[0] = localShadowProjection[0] * mShadowView;
	cbuffers->cbLight.CascadeViewProj[1] = localShadowProjection[1] * mShadowView;
	cbuffers->cbLight.CascadeViewProj[2] = localShadowProjection[2] * mShadowView;
	cbuffers->cbLight.CascadeViewProj[3] = localShadowProjection[3] * mShadowView; 
	
	graphics->updateCBuffer(cbLightInfoIndex, &cbuffers->cbLight);

}

glm::mat4 CascadeShadow::orthoCalc(float w, float h, float zf, float zn)
{
	glm::mat4 out;

	out[0][0] = 2 / w;
	out[1][1] = 2 / h;
	out[2][2] = 1 / (zf - zn);
	out[3][2] = zn / (zn - zf);
	out[3][3] = 1;

	return out;
}

void CascadeShadow::compileShadow()
{
	shadowmapCompileIndex->use();

	
	glm::mat4 matrix;
	CBWorldData world;
	graphics->updateCBuffer(cbWorldIndex, &world);

	//this is now the third time this is updated, is this necessary?
	graphics->updateCBuffer(cbLightInfoIndex, &cbuffers->cbLight);

	graphics->setPixelShaderResource(1, graphics->getDepthResourceView(), 1);
	graphics->setPixelShaderResource(2, graphics->getHandleIndex("shadowmapsrv", ResourceType::ShaderResourceViewArray), 1);

	graphics->setInputLayout((int)LayoutTypes::LAYOUT_DEFAULT);

	graphics->setTopology(1);

	graphics->setRasterState(0);

	graphics->setRenderTargetViews(shadowCompileRenderTarget,1,-1);

	graphics->draw(1, 0);

	graphics->setRenderTargetViews(-1, 1, -1);

	blurShadow();
}

void CascadeShadow::blurShadow()
{

	graphics->CSexecDouble(computeUAV, shadowCompileResourceView, graphics->getHandleIndex("normal", ResourceType::ShaderResourceView), computeIndex, shadowConstants.gridWidth, shadowConstants.gridHeight, 1);
	//graphics->CSexecSingle(computeUAV, graphics->getHandleIndex("rainnoise",ResourceType::ShaderResourceView), computeIndex, shadowConstants.gridWidth, shadowConstants.gridHeight, 1);
}