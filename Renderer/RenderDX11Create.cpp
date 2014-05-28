#include "RenderDX11.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <fileapi.h>
#include <glm\gtc\matrix_transform.hpp>
#include <Windows.h>

//////////////////////////////////
//			Add Functions		//
//////////////////////////////////

void RenderDX11::addHandle(std::string _handleName, ResourceType _handleType, int _handleIndex)
{
	sharedContainer->addHandleIndex(_handleName,_handleType,_handleIndex);
}

//////////////////////////////////
//		Create Functions		//
//////////////////////////////////

int RenderDX11::createCubeTexture(std::string _filename)
{
	D3DX11_IMAGE_LOAD_INFO loadSMInfo;
	loadSMInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* SMTexture = 0;
	HRESULT hr = D3DX11CreateTextureFromFile(device, _filename.c_str(),
		&loadSMInfo, 0, (ID3D11Resource**)&SMTexture, 0);

	if (FAILED(hr))
	{
		std::cout << "Error: Could not find " + _filename + " Using default texture instead" << std::endl;
		return defaultImageIndex;
	}

	D3D11_TEXTURE2D_DESC SMTextureDesc;
	SMTexture->GetDesc(&SMTextureDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
	SMViewDesc.Format = SMTextureDesc.Format;
	SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SMViewDesc.TextureCube.MipLevels = SMTextureDesc.MipLevels;
	SMViewDesc.TextureCube.MostDetailedMip = 0;

	ID3D11ShaderResourceView* shaderResourceView = NULL;
	hr = device->CreateShaderResourceView(SMTexture, &SMViewDesc, &shaderResourceView);

 
	if (FAILED(hr))
	{
		std::cout << "Error: Could not create shader resource view from file " + _filename + ". Using default texture //instead." << std::endl;
		return defaultImageIndex;
	}

	return sharedContainer->addShaderResourceView(shaderResourceView);
}

int RenderDX11::createInputLayout(ID3DBlob *_vertexBlob, int _layoutType)
{
	HRESULT result = S_OK;
	ID3D11InputLayout *inputLayout = NULL;
	ID3D11ShaderReflection* vertexShaderReflection = NULL;
	result = D3DReflect(_vertexBlob->GetBufferPointer(), _vertexBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&vertexShaderReflection);
	if(FAILED(result))
		return -1;
	D3D11_SHADER_DESC shaderDesc;
	result= vertexShaderReflection->GetDesc(&shaderDesc);
	if(FAILED(result))
		return -1;

	
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;

	byte animSlot = 255;

	for (UINT32 i = 0; i < shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		result = vertexShaderReflection->GetInputParameterDesc(i, &paramDesc);
		if(FAILED(result))
			return -1;
		std::string semantic = paramDesc.SemanticName;
		if(animSlot == 255)
		{
			if(semantic == "PERINST")
				animSlot = 2;
			else if(semantic == "ANIM")
				animSlot = 1;
		}
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

		if(semantic == "PERINST")
		{
			if(animSlot != 1)
				elementDesc.InputSlot = 1;
			else
				elementDesc.InputSlot = 2;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			elementDesc.InstanceDataStepRate = 1;
		}
		else if(semantic == "ANIM")
		{
			elementDesc.InputSlot = animSlot;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;
		}
		else
		{
			elementDesc.InputSlot = 0;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;
		}

		
		
		

		// determine DXGI format
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		inputLayoutDesc.push_back(elementDesc);
	}

	result = device->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(), _vertexBlob->GetBufferPointer(), _vertexBlob->GetBufferSize(), &inputLayout);
	if (FAILED(result))
	{
		std::string text = "Failed to create inputLayout with index: " + _layoutType;
		MessageBox(NULL, text.c_str(), "Shader Error", MB_OK);
		PostQuitMessage(0);
		return -1;
	}

	vertexShaderReflection->Release();
	return sharedContainer->addInputLayout(inputLayout, _layoutType);
}

int RenderDX11::createRenderTargetView3D(int _index)
{
	HRESULT							result;
	D3D11_RENDER_TARGET_VIEW_DESC	desc;
	D3D11_TEXTURE3D_DESC			tDesc;
	ID3D11Texture3D*				texture = sharedContainer->getTexture3D(_index);;
	ID3D11RenderTargetView*			rTV = nullptr;

	if (texture == nullptr)
		return -1;

	ZeroMemory(&rTV, sizeof(rTV));
	texture->GetDesc(&tDesc);

	desc.Format = tDesc.Format;
	desc.Texture3D.FirstWSlice = 0;
	desc.Texture3D.MipSlice = 0;
	desc.Texture3D.WSize = tDesc.Depth;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;

	result = device->CreateRenderTargetView(texture, &desc, &rTV);

	if (FAILED(result))
	{
		return -1;
	}

	return sharedContainer->addRenderTargetView(rTV);
}

int RenderDX11::createDepthShaderResourceView(int _index)
{
	HRESULT result = S_OK;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;


	ID3D11ShaderResourceView *srv = NULL;

	result = device->CreateShaderResourceView(sharedContainer->getTexture2D(_index),
		&srvDesc,
		&srv);
	if (FAILED(result))
	{
		MessageBox(NULL, "Failed to create depth shader resource view.", "RenderDX11 Error", S_OK);
		return -1;
	}

	return sharedContainer->addShaderResourceView(srv);
}

int RenderDX11::createDepthShaderResourceViewArray(int _index, int _arraySize)
{
	HRESULT result = S_OK;


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.ArraySize = _arraySize;
	srvDesc.Texture2DArray.FirstArraySlice = 0;

	ID3D11ShaderResourceView *srv = NULL;

	result = device->CreateShaderResourceView(sharedContainer->getTexture2D(_index),
		&srvDesc,
		&srv);
	if (FAILED(result))
	{
		MessageBox(NULL, "Failed to create depth shader array resource view.", "RenderDX11 Error", S_OK);
		return -1;
	}

	return sharedContainer->addShaderResourceView(srv);
}

int RenderDX11::createRenderTargetView1ch(int _index)
{
	HRESULT result = S_OK;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = DXGI_FORMAT_R16_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	ID3D11RenderTargetView *renderTargetView = NULL;

	result = device->CreateRenderTargetView(sharedContainer->getTexture2D(_index), &rtvDesc, &renderTargetView);
	if (FAILED(result))
	{
		std::cout << "Failed to create render target view.\n";
		return -1;
	}
	return sharedContainer->addRenderTargetView(renderTargetView);
}

int RenderDX11::createShaderResourceView3D(int _index)
{
	HRESULT							result;
	D3D11_SHADER_RESOURCE_VIEW_DESC	desc;
	D3D11_TEXTURE3D_DESC			tDesc;
	ID3D11ShaderResourceView*		sRV = nullptr;
	ID3D11Texture3D*				texture = sharedContainer->getTexture3D(_index);

	if (texture == nullptr)
		return -1;

	texture->GetDesc(&tDesc);
	ZeroMemory(&desc, sizeof(desc));

	desc.Format = tDesc.Format;
	desc.Texture3D.MipLevels = tDesc.MipLevels;
	desc.Texture3D.MostDetailedMip = 0;
	desc.ViewDimension = D3D11_SRV_DIMENSION::D3D_SRV_DIMENSION_TEXTURE3D;

	result = device->CreateShaderResourceView(texture, &desc, &sRV);

	if (FAILED(result))
	{
		return -1;
	}

	return sharedContainer->addShaderResourceView(sRV);
}

int RenderDX11::createShader(std::string _shader, std::string _shaderModel, int _layoutType)
{
	HRESULT result = S_OK;
	ID3DBlob* shaderBlob = NULL;
	std::string path = "Shaders/";
	std::string file = path + _shader + ".cso";

	std::wstring wfile;
	wfile.assign(file.begin(), file.end());
	size_t shaderSize;

	result = D3DReadFileToBlob(wfile.c_str(), &shaderBlob);
	if (FAILED(result))
	{
		std::string text = "Failed to load precompiled shader " + _shader;
		MessageBox(NULL, text.c_str(), "Shader Error", MB_OK);
		PostQuitMessage(0);

		std::cout << text + "\n";
		return -1;
	}

	if (_shaderModel == "vs_5_0")
	{
		soDeclaration.name.clear();
		soDeclaration.soDeclaration.clear();
		ID3D11VertexShader *shader = NULL;
		result = device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &shader);
		if (FAILED(result))
		{
			std::string text = "Failed to create " + _shader;
			MessageBox(NULL, text.c_str(), "Shader Error", MB_OK);
			PostQuitMessage(0);

			std::cout << text + "\n";
			return -1;
		}
		createInputLayout(shaderBlob, _layoutType);
		createSODeclaration(shaderBlob);

		shaderSize = shaderBlob->GetBufferSize();
		shaderBlob->Release();
		return sharedContainer->addVertexShader(shader,shaderSize);
	}
	if (_shaderModel == "gs_5_0")
	{
		ID3D11GeometryShader *shader = NULL;
		result = device->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &shader);
		if (FAILED(result))
		{
			std::string text = "Failed to create " + _shader;
			MessageBox(NULL, text.c_str(), "Shader Error", MB_OK);
			PostQuitMessage(0);

			std::cout << text + "\n";
			return -1;
		}
		shaderSize = shaderBlob->GetBufferSize();
		shaderBlob->Release();
		return sharedContainer->addGeometryShader(shader,shaderSize);
	}
	if (_shaderModel == "hs_5_0")
	{
		ID3D11HullShader *shader = NULL;
		result = device->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &shader);
		if (FAILED(result))
		{
			std::string text = "Failed to create " + _shader;
			MessageBox(NULL, text.c_str(), "Shader Error", MB_OK);
			PostQuitMessage(0);

			std::cout << text + "\n";
			return -1;
		}
		shaderSize = shaderBlob->GetBufferSize();
		shaderBlob->Release();
		return sharedContainer->addHullShader(shader,shaderSize);
	}
	if (_shaderModel == "ds_5_0")
	{
		ID3D11DomainShader *shader = NULL;
		result = device->CreateDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &shader);
		if (FAILED(result))
		{
			std::string text = "Failed to create " + _shader;
			MessageBox(NULL, text.c_str(), "Shader Error", MB_OK);
			PostQuitMessage(0);

			std::cout << text + "\n";
			return -1;
		}
		shaderSize = shaderBlob->GetBufferSize();
		shaderBlob->Release();
		return sharedContainer->addDomainShader(shader,shaderSize);
	}
	if (_shaderModel == "ps_5_0")
	{
		ID3D11PixelShader *shader = NULL;
		result = device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &shader);
		if (FAILED(result))
		{
			std::string text = "Failed to create " + _shader;
			MessageBox(NULL, text.c_str(), "Shader Error", MB_OK);
			PostQuitMessage(0);

			std::cout << text + "\n";
			return -1;
		}
		shaderSize = shaderBlob->GetBufferSize();
		shaderBlob->Release();
		return sharedContainer->addPixelShader(shader,shaderSize);
	}
	std::string text = "Failed to create " + _shader + " because shader model: " + _shaderModel + " is not accepted.";
	MessageBox(NULL, text.c_str(), "Shader Error", MB_OK);
	PostQuitMessage(0);

	std::cout << text + "\n";
	return -1;
}

int RenderDX11::createGeometryStreamOut(std::string _shader, std::string _shaderModel)
{
	HRESULT result = S_OK;
	ID3DBlob* shaderBlob = NULL;
	std::string path = "Shaders/";
	std::string file = path + _shader + ".cso";

	std::wstring wfile;
	wfile.assign(file.begin(), file.end());

	result = D3DReadFileToBlob(wfile.c_str(), &shaderBlob);
	if (FAILED(result))
	{
		std::string text = "Failed to load precompiled shader " + _shader;
		MessageBox(NULL, text.c_str(), "Shader Error", MB_OK);
		PostQuitMessage(0);

		std::cout << text + "\n";
		return -1;
	}


	if (_shaderModel == "gs_5_0")
	{
		ID3D11GeometryShader *shader = NULL;

		result = device->CreateGeometryShaderWithStreamOutput(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &soDeclaration.soDeclaration[0], soDeclaration.nrOfElements, &soDeclaration.stride, 1, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &shader);
		if (FAILED(result))
		{
			std::string text = "Failed to create " + _shader;
			MessageBox(NULL, text.c_str(), "Shader Error", MB_OK);
			PostQuitMessage(0);

			std::cout << text + "\n";
			return -1;
		}
		size_t shaderSize = shaderBlob->GetBufferSize();
		shaderBlob->Release();
		return sharedContainer->addGeometryShader(shader,shaderSize);
	}
	return -1;
}

void RenderDX11::createSODeclaration(ID3DBlob *_shaderBlob)
{
	HRESULT res = S_OK;
	ID3D11ShaderReflection* vertexShaderReflection = NULL;
	res = D3DReflect(_shaderBlob->GetBufferPointer(), _shaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&vertexShaderReflection);
	if(FAILED(res))
		throw "failed to create SO declaration";
	D3D11_SHADER_DESC shaderDesc;
	res = vertexShaderReflection->GetDesc(&shaderDesc);
	if(FAILED(res))
		throw "failed to create SO declaration";
	soDeclaration.stride = 0;

	for (UINT32 i = 0; i < shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		res = vertexShaderReflection->GetInputParameterDesc(i, &paramDesc);
		if(FAILED(res))
			throw "failed to create SO declaration";
		D3D11_SO_DECLARATION_ENTRY elementDesc;

		soDeclaration.name.push_back(paramDesc.SemanticName);

		//elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.OutputSlot = (BYTE)paramDesc.SemanticIndex;
		elementDesc.Stream = 0;
		elementDesc.StartComponent = 0;
		// determine DXGI format
		if (paramDesc.Mask == 1)
		{
			elementDesc.ComponentCount = 1;
		}
		else if (paramDesc.Mask <= 3)
		{
			elementDesc.ComponentCount = 2;
		}
		else if (paramDesc.Mask <= 7)
		{
			elementDesc.ComponentCount = 3;
		}
		else if (paramDesc.Mask <= 15)
		{
			elementDesc.ComponentCount = 4;
		}
		else
		{
			elementDesc.ComponentCount = 0;
		}

		soDeclaration.stride += elementDesc.ComponentCount * 4;

		soDeclaration.soDeclaration.push_back(elementDesc);
	}

	for (unsigned int i = 0; i < shaderDesc.InputParameters; i++)
	{
		soDeclaration.soDeclaration.at(i).SemanticName = soDeclaration.name.at(i).c_str();
	}

	soDeclaration.nrOfElements = shaderDesc.InputParameters;
	vertexShaderReflection->Release();
}

int RenderDX11::createShaderResourceViewFromFile(std::string _fileName)
{
	if(_fileName.empty())
		return -1;
	ID3D11ShaderResourceView*	shaderResourceView = NULL;

	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(device, _fileName.c_str(), NULL, NULL, &shaderResourceView, NULL);

	if (FAILED(hr))
	{
		std::cout << "Error: Could not find " + _fileName + ". Using default texture instead.\n";
		return defaultImageIndex;
	}
	return sharedContainer->addShaderResourceView(shaderResourceView);
}

int RenderDX11::createDefaultVertexBuffer()
{
	HRESULT result = S_OK;
	DefaultVertex vertex[3];

	vertex[0].pos = glm::vec3(0, 0, 0);
	vertex[0].normal = glm::vec3(0, 0, -1);
	vertex[0].texCoord = glm::vec2(0, 0);
	vertex[1].pos = glm::vec3(0.1, 0.2, 0);
	vertex[1].normal = glm::vec3(0, 0, -1);
	vertex[1].texCoord = glm::vec2(0.5, 1);
	vertex[2].pos = glm::vec3(0.2, 0, 0);
	vertex[2].normal = glm::vec3(0, 0, -1);
	vertex[2].texCoord = glm::vec2(1, 0);
	UINT maxSize = 3;
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(DefaultVertex)* maxSize;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = &vertex[0];

	ID3D11Buffer *buffer = NULL;
	result = device->CreateBuffer(&bufferDesc, &initData, &buffer);
	if (FAILED(result))
	{
		std::cout << "Default vertex buffer creation failed!";
		return -1;
	}

	return sharedContainer->addD3DBuffer(buffer, maxSize);

}

int RenderDX11::createVBufferImmutable(void** _bufferData, UINT _bufferSize, UINT _vertexSize)
{
	HRESULT result = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = _vertexSize * _bufferSize;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = _bufferData;

	ID3D11Buffer *buffer = NULL;
	result = device->CreateBuffer(&bufferDesc, &initData, &buffer);
	if (FAILED(result))
	{
		std::cout << "Immutable Vertex buffer creation failed!";
		return -1;
	}

	return sharedContainer->addD3DBuffer(buffer, _bufferSize);
}

int RenderDX11::createVBufferDynamic(UINT _bufferSize, UINT _vertexSize)
{
	HRESULT result = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = _vertexSize * _bufferSize;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	ID3D11Buffer *buffer = NULL;
	result = device->CreateBuffer(&bufferDesc, NULL, &buffer);
	if (FAILED(result))
	{
		MessageBox(NULL, "Error creating dynamic vertex buffer", "RenderDX11 Error", S_OK);
		return -1;
	}

	return sharedContainer->addD3DBuffer(buffer, _bufferSize);
}

int RenderDX11::createIndexBufferImmutable(void** _bufferData, UINT _bufferSize)
{
	HRESULT result = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(DWORD)* _bufferSize;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = _bufferData;

	ID3D11Buffer *buffer = NULL;
	result = device->CreateBuffer(&bufferDesc, &initData, &buffer);
	if (FAILED(result))
	{
		std::cout << "Immutable Index buffer creation failed!";
		return -1;
	}

	return sharedContainer->addD3DBuffer(buffer, _bufferSize);
}

int RenderDX11::createVBufferStreamout(UINT _bufferSize, UINT _vertexSize)
{
	HRESULT result = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = _vertexSize * _bufferSize;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.StructureByteStride = 0;

	ID3D11Buffer *buffer = NULL;
	result = device->CreateBuffer(&bufferDesc, NULL, &buffer);
	if (FAILED(result))
	{
		std::cout << "Dynamic Vertex buffer creation failed!";
		return -1;
	}

	return sharedContainer->addD3DBuffer(buffer, _bufferSize);
}

int RenderDX11::createIndexBufferDynamic(UINT _bufferSize)
{

	HRESULT result = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(DWORD)* _bufferSize;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	ID3D11Buffer *buffer = NULL;
	result = device->CreateBuffer(&bufferDesc, NULL, &buffer);
	if (FAILED(result))
	{
		std::cout << "Dynamic Index buffer creation failed!";
		return -1;
	}

	return sharedContainer->addD3DBuffer(buffer, _bufferSize);
}

int RenderDX11::createVBufferInit(void** _bufferData, UINT _bufferSize, UINT _vertexSize)
{
	HRESULT result = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = _vertexSize * _bufferSize;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = _bufferData;

	ID3D11Buffer *buffer = NULL;
	result = device->CreateBuffer(&bufferDesc, &initData, &buffer);
	if (FAILED(result))
	{
		std::cout << "Dynamic Vertex buffer creation failed!";
		return -1;
	}

	return sharedContainer->addD3DBuffer(buffer, _bufferSize);
}

int RenderDX11::createTexture2D(int _width, int _height)
{
	HRESULT result = S_OK;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = _width;
	texDesc.Height = _height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

	ID3D11Texture2D *texture;

	result = device->CreateTexture2D(&texDesc, NULL, &texture);
	if (FAILED(result))
	{
		std::cout << "Failed to create texture for render target view.\n";
		return -1;
	}

	return sharedContainer->addTexture2D(texture);
}

int RenderDX11::createTexture2D32(int _width, int _height, void **_initData)
{
	HRESULT result = S_OK;


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = _width;
	desc.Height = _height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.SampleDesc.Quality = 0;

	ID3D11Texture2D *texture = NULL;

	if(_initData != NULL)
	{
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = _initData;
		data.SysMemPitch = _width * 4 * sizeof(float);
		result = device->CreateTexture2D(&desc, &data, &texture);
	}
	else
	{
		result = device->CreateTexture2D(&desc, NULL, &texture);
	}
	if(FAILED(result))
	{
		std::cout << "Failed to create texture.\n";
		return -1;
	}

	return sharedContainer->addTexture2D(texture);
}


int RenderDX11::createTexture2D(int _width, int _height, void **_initData)
{
	HRESULT result = S_OK;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc,sizeof(desc));
	desc.Width = _width;
	desc.Height = _height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.Format = DXGI_FORMAT_R32_FLOAT;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.SampleDesc.Quality = 0;

	ID3D11Texture2D *texture = NULL;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data,sizeof(data));
	data.pSysMem = _initData;
	data.SysMemPitch = _width * sizeof(float);
	result = device->CreateTexture2D( &desc, &data, &texture);
	if(FAILED(result))
	{
		std::cout << "Failed to create texture.\n" ;
		return -1;
	}

	return sharedContainer->addTexture2D(texture);
}


int RenderDX11::createTexture2D32(int _width, int _height)
{
	HRESULT result = S_OK;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = _width;
	texDesc.Height = _height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	ID3D11Texture2D *texture;

	result = device->CreateTexture2D(&texDesc, NULL, &texture);
	if (FAILED(result))
	{
		std::cout << "Failed to create texture for render target view.\n";
		return -1;
	}

	return sharedContainer->addTexture2D(texture);
}

int RenderDX11::createTexture2DB8G8R8A8(int _width, int _height)
{
	HRESULT result = S_OK;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = _width;
	texDesc.Height = _height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	ID3D11Texture2D *texture;

	result = device->CreateTexture2D(&texDesc, NULL, &texture);
	if (FAILED(result))
	{
		std::cout << "Failed to create texture for render target view.\n";
		return -1;
	}

	return sharedContainer->addTexture2D(texture);
}

int RenderDX11::createRenderTargetView(UINT _index)
{
	HRESULT result = S_OK;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	ID3D11RenderTargetView *renderTargetView = NULL;

	result = device->CreateRenderTargetView(sharedContainer->getTexture2D(_index), &rtvDesc, &renderTargetView);
	if (FAILED(result))
	{
		MessageBox(NULL, "Failed to create render target view.", "RenderDX11 Error", S_OK);
		return -1;
	}

	return sharedContainer->addRenderTargetView(renderTargetView);
}

int RenderDX11::createRenderTargetView32(int _index)
{
	HRESULT result = S_OK;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	ID3D11RenderTargetView *renderTargetView = NULL;

	result = device->CreateRenderTargetView(sharedContainer->getTexture2D(_index), &rtvDesc, &renderTargetView);
	if (FAILED(result))
	{
		MessageBox(NULL, "Failed to create render target view.", "RenderDX11 Error", S_OK);
		return -1;
	}

	return sharedContainer->addRenderTargetView(renderTargetView);
}

int RenderDX11::createRenderTargetViewB8G8R8A8(int _index)
{
	HRESULT result = S_OK;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	ID3D11RenderTargetView *renderTargetView = NULL;

	result = device->CreateRenderTargetView(sharedContainer->getTexture2D(_index), &rtvDesc, &renderTargetView);
	if (FAILED(result))
	{
		MessageBox(NULL, "Failed to create render target view.", "RenderDX11 Error", S_OK);
		return -1;
	}

	return sharedContainer->addRenderTargetView(renderTargetView);
}

int RenderDX11::createShaderResourceView(UINT _index)
{
	HRESULT result = S_OK;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;


	ID3D11ShaderResourceView *srv = NULL;

	result = device->CreateShaderResourceView(sharedContainer->getTexture2D(_index),
		&srvDesc,
		&srv);
	if (FAILED(result))
	{
		MessageBox(NULL, "Failed to create shader resource view.", "RenderDX11 Error", S_OK);
		return -1;
	}

	return sharedContainer->addShaderResourceView(srv);
}

int RenderDX11::createShaderResourceView32(int _index)
{
	HRESULT result = S_OK;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;


	ID3D11ShaderResourceView *srv = NULL;

	result = device->CreateShaderResourceView(sharedContainer->getTexture2D(_index),
		&srvDesc,
		&srv);
	if (FAILED(result))
	{
		MessageBox(NULL, "Failed to create shader resource view.", "RenderDX11 Error", S_OK);
		return -1;
	}

	return sharedContainer->addShaderResourceView(srv);
}

int RenderDX11::createShaderResourceViewB8G8R8A8(int _index)
{
	HRESULT result = S_OK;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;


	ID3D11ShaderResourceView *srv = NULL;

	result = device->CreateShaderResourceView(sharedContainer->getTexture2D(_index),
		&srvDesc,
		&srv);
	if (FAILED(result))
	{
		MessageBox(NULL, "Failed to create shader resource view.", "RenderDX11 Error", S_OK);
		return -1;
	}

	return sharedContainer->addShaderResourceView(srv);
}

int RenderDX11::createShaderResourceView32(UINT _index)
{
	HRESULT result = S_OK;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;


	ID3D11ShaderResourceView *srv = NULL;

	result = device->CreateShaderResourceView(sharedContainer->getTexture2D(_index),
		&srvDesc,
		&srv);
	if(FAILED(result))
	{
		MessageBox(NULL, "Failed to create shader resource view.", "RenderDX11 Error", S_OK);
		return -1;
	}

	return sharedContainer->addShaderResourceView(srv);
}

int RenderDX11::createShaderResourceViewTest(int _index)
{
	HRESULT result = S_OK;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc,sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;


	ID3D11ShaderResourceView *srv = NULL;

	result = device->CreateShaderResourceView(	sharedContainer->getTexture2D(_index),
												&srvDesc,
												&srv);
	if(FAILED(result))
	{
		std::cout << "Failed to create shader resource view.\n";
		return -1;
	}

	return sharedContainer->addShaderResourceView(srv);
}

int RenderDX11::createDepthTexture2D(int _width, int _height)
{
	HRESULT result = S_OK;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = _width;
	texDesc.Height = _height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D *texture;

	result = device->CreateTexture2D(&texDesc, NULL, &texture);
	if (FAILED(result))
	{
		MessageBox(NULL, "Failed to create texture for render target view.", "RenderDX11 Error", S_OK);
		return -1;
	}

	return sharedContainer->addTexture2D(texture);
}

int RenderDX11::createTexture3D(unsigned int _width, unsigned int _height, unsigned int _depth)
{
	HRESULT					result;
	ID3D11Texture3D*		texture;
	D3D11_TEXTURE3D_DESC	desc;

	ZeroMemory(&desc, sizeof(desc));

	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	desc.Height = _height;
	desc.Depth = _depth;
	desc.Width = _width;

	result = device->CreateTexture3D(&desc, NULL, &texture);

	if (FAILED(result))
	{
		return -1;
	}

	return sharedContainer->addTexture3D(texture);
}

int RenderDX11::createDepthRenderTargetView(int _index)
{
	HRESULT result = S_OK;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	ID3D11DepthStencilView *tDepthStencilView = NULL;

	result = device->CreateDepthStencilView(sharedContainer->getTexture2D(_index), &dsvDesc, &tDepthStencilView);
	if (FAILED(result))
	{
		MessageBox(NULL, "Failed to create depth stencil view", "RenderDX11 Error", S_OK);
		return -1;
	}
	shadowState = sharedContainer->addDepthStencilView(tDepthStencilView);

	return shadowState;
}

int RenderDX11::createDepthTexture2DArray(int _width, int _height, int _arraySize)
{
	HRESULT result = S_OK;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = _width;
	texDesc.Height = _height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = _arraySize;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D *texture;

	result = device->CreateTexture2D(&texDesc, NULL, &texture);
	if (FAILED(result))
	{
		MessageBox(NULL, "Failed to create texture for render target view.", "RenderDX11 Error", S_OK);
		return -1;
	}

	return sharedContainer->addTexture2D(texture);
}


int RenderDX11::createDepthRenderTargetViewArray(int _index, int _arraySize)
{
	HRESULT result = S_OK;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Texture2DArray.FirstArraySlice = 0;
	dsvDesc.Texture2DArray.ArraySize = _arraySize;


	ID3D11DepthStencilView *tDepthStencilView = NULL;

	result = device->CreateDepthStencilView(sharedContainer->getTexture2D(_index), &dsvDesc, &tDepthStencilView);
	if (FAILED(result))
	{
		MessageBox(NULL, "Failed to create depth stencil view", "RenderDX11 Error", S_OK);
		return -1;
	}

	return sharedContainer->addDepthStencilView(tDepthStencilView);
}

int RenderDX11::createComputeShader(std::string _CSshaderName)
{
	HRESULT hr;
	ID3DBlob* shaderBlob = NULL;
	ID3DBlob* errorBlob = NULL;

	std::string fullpath = "Shaders/" + _CSshaderName + ".cso";
	std::wstring wpath;
	wpath.assign(fullpath.begin(),fullpath.end());

	hr = D3DReadFileToBlob(wpath.c_str(), &shaderBlob);
	/*hr = D3DX11CompileFromFile( fullpath.c_str(), NULL, NULL, "main", "cs_5_0", 
								dwShaderFlags, 0, NULL, &shaderBlob, &errorBlob, NULL );*/

	if( FAILED(hr) )
		return -1;

	ID3D11ComputeShader* CSshader = NULL;

	hr = device->CreateComputeShader( shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &CSshader );

	if(FAILED(hr))
	{
		MessageBox(NULL, "Failed to create Compute shader", "RenderDX11 Error!", S_OK);
		return -1;
	}

	shaderBlob->Release();

	if(errorBlob != NULL)
		errorBlob->Release();

	return sharedContainer->addComputeShader(CSshader);
}

int RenderDX11::createUABuffer(void** _bufferData, UINT _bufferSize, UINT _bufferStride)
{
	HRESULT hr;
	D3D11_BUFFER_DESC uaDesc;
	ZeroMemory( &uaDesc, sizeof(uaDesc) );
	ID3D11Buffer* buffer = NULL;

	uaDesc.BindFlags			= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	uaDesc.ByteWidth			= _bufferSize;
	uaDesc.MiscFlags			= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	uaDesc.StructureByteStride	= _bufferStride;
	uaDesc.Usage				= D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = _bufferData;

	hr = device->CreateBuffer(&uaDesc, &initData,  &buffer);

	if (FAILED(hr))
	{
		std::cout << "Failed to create UA buffer.\n";
		return -1;
	}

	return sharedContainer->addD3DBuffer(buffer, _bufferSize);
}

int RenderDX11::createCustomShaderResourceView(UINT _numElements, UINT _bufferIndex)
{
	HRESULT hr;
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	desc.ViewDimension			= D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement	= 0;
	desc.Format					= DXGI_FORMAT_UNKNOWN;
	desc.BufferEx.NumElements	= _numElements;

	ID3D11ShaderResourceView* srvTemp	= nullptr;

	hr = device->CreateShaderResourceView( sharedContainer->getVertexBuffer(_bufferIndex), &desc, &srvTemp );

	if( FAILED(hr) )
		return -1;

	return sharedContainer->addShaderResourceView(srvTemp);
}

int RenderDX11::createUAV(UINT _bufferIndex, UINT _numElements)
{
	HRESULT hr;

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory( &desc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = _numElements;
	
	ID3D11UnorderedAccessView* uav = NULL;

	hr = device->CreateUnorderedAccessView( sharedContainer->getVertexBuffer(_bufferIndex), &desc, &uav );

	if( FAILED(hr) )
		return -1;

	return sharedContainer->addUAV(uav);
}

int RenderDX11::createUAVTexture2D(int _width, int _height)
{
	HRESULT result = S_OK;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = _width;
	texDesc.Height = _height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	ID3D11Texture2D *texture;

	result = device->CreateTexture2D(&texDesc, NULL, &texture);
	if (FAILED(result))
	{
		std::cout << "Failed to create texture for render target view.\n";
		return -1;
	}

	return sharedContainer->addTexture2D(texture);
}

int RenderDX11::createUAVTextureView(int _textureIndex)
{
	HRESULT result = S_OK;

	ID3D11UnorderedAccessView*  temp = NULL;
	result = device->CreateUnorderedAccessView(sharedContainer->getTexture2D(_textureIndex), NULL, &temp);

	if (FAILED(result))
	{
		std::cout << "Failed to create UAV texture for the texture.\n";
		return -1;
	}

	return sharedContainer->addUAV(temp);
}
