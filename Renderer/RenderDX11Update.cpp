#include "RenderDX11.h"
#include <glm\gtc\matrix_transform.hpp>

bool RenderDX11::updateTexture2D(int _textureID, int _width, int _height, void **_data, UINT _byteSizePerPixel)
{
	D3D11_MAPPED_SUBRESOURCE updateData;
	ZeroMemory(&updateData, sizeof(updateData));

	HRESULT result = S_OK;
	result = deviceContext->Map(sharedContainer->getTexture2D(_textureID), 0, D3D11_MAP_WRITE_DISCARD, 0, &updateData);
	if(FAILED(result))
	{
		std::cout << "Failed to update Texture2D!";
		return false;
	}
	memcpy(updateData.pData, _data, _width * _height * _byteSizePerPixel);
	deviceContext->Unmap(sharedContainer->getTexture2D(_textureID), 0);
	return true;
}

void RenderDX11::updateVBuffer(UINT _index, void** _mapData, UINT _bufferSize, UINT _vertexSize)
{
	D3D11_MAPPED_SUBRESOURCE updateData;
	ZeroMemory(&updateData, sizeof(updateData));

	if(!FAILED(deviceContext->Map(sharedContainer->getVertexBuffer(_index), 0, D3D11_MAP_WRITE_DISCARD, 0, &updateData)))
		memcpy(updateData.pData, _mapData, _vertexSize* _bufferSize);

	deviceContext->Unmap(sharedContainer->getVertexBuffer(_index), 0);
}

void RenderDX11::updateIBuffer(UINT _index, void** _mapData, UINT _bufferSize)
{
	D3D11_MAPPED_SUBRESOURCE updateData;
	ZeroMemory(&updateData, sizeof(updateData));

	if(!FAILED(deviceContext->Map(sharedContainer->getVertexBuffer(_index), 0, D3D11_MAP_WRITE_DISCARD, 0, &updateData)))
		memcpy(updateData.pData, _mapData, sizeof(int)* _bufferSize);

	deviceContext->Unmap(sharedContainer->getVertexBuffer(_index), 0);
}

void RenderDX11::replaceImmutableVBuffer(UINT _index, void** _mapData, UINT _bufferSize, UINT _vertexSize)
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
	initData.pSysMem = _mapData;

	ID3D11Buffer *buffer = NULL;
	result = device->CreateBuffer(&bufferDesc, &initData, &buffer);
	if(FAILED(result))
	{
		std::cout << "Immutable Vertex buffer re-creation failed!";
		return;
	}
	sharedContainer->setD3DBufferAt(_index, buffer, _bufferSize);
}

void RenderDX11::replaceImmutableIBuffer(UINT _index, void** _mapData, UINT _bufferSize)
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
	initData.pSysMem = _mapData;

	ID3D11Buffer *buffer = NULL;
	result = device->CreateBuffer(&bufferDesc, &initData, &buffer);
	if(FAILED(result))
	{
		std::cout << "Immutable Index buffer re-creation failed!";
		return;
	}
	sharedContainer->setD3DBufferAt(_index, buffer, _bufferSize);
}

void RenderDX11::replaceTexture2D(UINT _index, int _width, int _height, void **_initData)
{
	HRESULT result = S_OK;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
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
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = _initData;
	data.SysMemPitch = _width * sizeof(float);
	result = device->CreateTexture2D(&desc, &data, &texture);
	if(FAILED(result))
	{
		std::cout << "Failed to re-create texture.\n";
		return;
	}
	sharedContainer->replaceTexture2D(_index, texture);
}

void RenderDX11::replaceTexture2D32(UINT _index, int _width, int _height, void **_initData)
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

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = _initData;
	data.SysMemPitch = _width * 4 * sizeof(float);
	result = device->CreateTexture2D(&desc, &data, &texture);
	if(FAILED(result))
	{
		std::cout << "Failed to create texture.\n";
		return;
	}
	sharedContainer->replaceTexture2D(_index, texture);
}

void RenderDX11::replaceShaderResourceViewTest(UINT _index, UINT _textureID)
{
	HRESULT result = S_OK;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	ID3D11ShaderResourceView *srv = NULL;

	result = device->CreateShaderResourceView(sharedContainer->getTexture2D(_textureID), &srvDesc, &srv);
	if(FAILED(result))
	{
		std::cout << "Failed to re-create shader resource view.\n";
		return;
	}

	sharedContainer->replaceShaderResourceView(_index, srv);
}

void RenderDX11::replaceShaderResourceView32(UINT _index, UINT _textureID)
{
	HRESULT result = S_OK;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	ID3D11ShaderResourceView *srv = NULL;

	result = device->CreateShaderResourceView(sharedContainer->getTexture2D(_textureID), &srvDesc, &srv);
	if(FAILED(result))
	{
		MessageBox(NULL, "Failed to re-create shader resource view.", "RenderDX11 Error", S_OK);
		return;
	}

	sharedContainer->replaceShaderResourceView(_index, srv);
}

void RenderDX11::updateCBuffer(UINT _index, const void *_data)
{
	deviceContext->UpdateSubresource(*sharedContainer->getCBuffer(_index), 0, NULL, _data, 0, 0);
}