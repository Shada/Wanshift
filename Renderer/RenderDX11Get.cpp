#include "RenderDX11.h"
#include "Utility_Render.h"

glm::vec2 RenderDX11::getResolution()
{
	return glm::vec2(screenWidth, screenHeight);
}

int RenderDX11::getDepthResourceView()
{
	return depthShaderResource;
}

int RenderDX11::getShadowResourceView()
{
	return shadowView;
}

int RenderDX11::getDedicatedVRAM()
{
	return videoCardMemory;
}

int RenderDX11::getUsedVRAM()
{
	return getAnswer();
}

int RenderDX11::getRandomSeedIndex()
{
	return randomSeedIndex;
}

int RenderDX11::getHandleIndex(std::string _handleName, ResourceType _handleType)
{
	return sharedContainer->getHandleIndex(_handleName, _handleType);
}

ID3D11DeviceContext* RenderDX11::getDeviceContext()
{
	return deviceContext;
}

ID3D11Device* RenderDX11::getDevice()
{
	return device;
}

ID3D11UnorderedAccessView* RenderDX11::getUAV(UINT _index)
{
	return sharedContainer->getUAV(_index);
}

ID3D11ShaderResourceView* RenderDX11::getSRV(UINT _index)
{
	return sharedContainer->getShaderResourceViews(_index)[0];
}

float RenderDX11::totalAllocatedVRAM()
{
	//float textureSize2D, textureSize3D, bufferSize, shaderSize, rtvSize;

	//printf("\n---Texture2D---\n");
	//textureSize2D	= sharedContainer->getAllocTexture2D() / 1024.0f / 1024.0f;
	//printf("\n---Texture3D---\n");
	//textureSize3D	= sharedContainer->getAllocTexture3D() / 1024.0f / 1024.0f;
	//printf("\n---Buffer---\n");
	//bufferSize		= sharedContainer->getAllocBuffer() / 1024.0f / 1024.0f;

	//printf("\n--- Total Texture2D Memory ---\n");
	//printf("total size(MB):%f\n", textureSize2D);

	//printf("\n--- Total Texture3D Memory ---\n");
	//printf("total size(MB):%f\n", textureSize3D);
	//
	//printf("\n--- Total Buffer Memory ---\n");
	//printf("total size(MB):%f\n", bufferSize);

	//printf("\n--- Total Shader Memory ---\n");
	//shaderSize = 0;
	//for(int i = 0; i < 5; i++)
	//{
	//	float typeShaderSize = sharedContainer->getAllocShader() / 1024.0f / 1024.0f;
	//	shaderSize += typeShaderSize;
	//	printf("type:%i size(MB):%f\n", i, typeShaderSize);
	//}
	//printf("total shader size(MB):%f\n", shaderSize);

	//printf("\n--- Total BackBuffer Memory ---\n");
	//float bbsize = sharedContainer->getBackBufferSize() / 1024.0f / 1024.0f;

	//printf("total size(MB):%f\n",bbsize);
	return sharedContainer->getTotAlloc();
	//return textureSize2D + textureSize3D + bufferSize + shaderSize;
}

float RenderDX11::getTotalVRAM()
{
	return sharedContainer->getTotAlloc();
}

float RenderDX11::getMemBuffers()
{
	return sharedContainer->getAllocBuffer();
}

float RenderDX11::getMemShaders()
{
	return sharedContainer->getAllocShader();
}

float RenderDX11::getMemTextures()
{
	return sharedContainer->getAllocTexture2D() + sharedContainer->getAllocTexture3D();
}

float RenderDX11::getMemBackBuffer()
{
	return sharedContainer->getBackBufferSize();
}