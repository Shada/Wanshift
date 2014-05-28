#include "RenderDX11.h"

//////////////////////////////
//		Set Functions		//
//////////////////////////////

void RenderDX11::setVertexShaderResource(int _startSlot, int _index, int _amount)
{
	if (_index != -1)
		deviceContext->VSSetShaderResources(_startSlot, _amount, sharedContainer->getShaderResourceViews(_index));
	else
	{
		ID3D11ShaderResourceView* nullsrv = {NULL};
		deviceContext->VSSetShaderResources(_startSlot, 1, &nullsrv);
	}
}

void RenderDX11::setGeometryShaderResource(int _startSlot, int _index, int _amount)
{
	if (_index != -1)
		deviceContext->GSSetShaderResources(_startSlot, _amount, sharedContainer->getShaderResourceViews(_index));
	else
		deviceContext->GSSetShaderResources(_startSlot, 1, sharedContainer->getShaderResourceViews(blackImageIndex));
}

void RenderDX11::setHullShaderResource(int _startSlot, int _index, int _amount)
{
	if (_index != -1)
		deviceContext->HSSetShaderResources(_startSlot, _amount, sharedContainer->getShaderResourceViews(_index));
	else
		deviceContext->HSSetShaderResources(_startSlot, 1, sharedContainer->getShaderResourceViews(blackImageIndex));
}

void RenderDX11::setDomainShaderResource(int _startSlot, int _index, int _amount)
{
	if (_index != -1)
		deviceContext->DSSetShaderResources(_startSlot, _amount, sharedContainer->getShaderResourceViews(_index));
	else
		deviceContext->DSSetShaderResources(_startSlot, 1, sharedContainer->getShaderResourceViews(blackImageIndex));
}

void RenderDX11::setPixelShaderResource(int _startSlot, int _index, int _amount)
{

	if (_index != -1)
		deviceContext->PSSetShaderResources(_startSlot, _amount, sharedContainer->getShaderResourceViews(_index));
	else
	{
		ID3D11ShaderResourceView* nullsrv = {NULL};
		deviceContext->PSSetShaderResources(_startSlot, 1, &nullsrv);
	}
}

void RenderDX11::setVertexBuffers(int _startSlot, int _index, int _amount, UINT *_strides, UINT *_offsets)
{
	deviceContext->IASetVertexBuffers(_startSlot, _amount, sharedContainer->getVertexBuffers(_index), _strides, _offsets);
}

void RenderDX11::setIndexBuffer(int _index, UINT _offset)
{
	if (_index != -1)
		deviceContext->IASetIndexBuffer(*sharedContainer->getVertexBuffers(_index), DXGI_FORMAT_R32_UINT, _offset);
	else
		deviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
}

void RenderDX11::setSOTargets(int _nrOfTargets, int _targetIndex, UINT *_offset)
{
	deviceContext->SOSetTargets(_nrOfTargets, sharedContainer->getVertexBuffers(_targetIndex), _offset);
}
void RenderDX11::setCBuffers(UINT startSlot, UINT amount, UINT _index)
{
	deviceContext->VSSetConstantBuffers(startSlot, amount, sharedContainer->getCBuffer(_index));
	deviceContext->DSSetConstantBuffers(startSlot, amount, sharedContainer->getCBuffer(_index));
	deviceContext->GSSetConstantBuffers(startSlot, amount, sharedContainer->getCBuffer(_index));
	deviceContext->PSSetConstantBuffers(startSlot, amount, sharedContainer->getCBuffer(_index));
	deviceContext->HSSetConstantBuffers(startSlot, amount, sharedContainer->getCBuffer(_index));
	deviceContext->CSSetConstantBuffers(startSlot, amount, sharedContainer->getCBuffer(_index));
}
void RenderDX11::resetSOTargets()
{
	ID3D11Buffer* temp = NULL;
	UINT offset = { 0 };
	deviceContext->SOSetTargets(1, &temp, &offset);
}

void RenderDX11::setRenderTargetViews(int _rtvIndex, int _amount, int _depthIndex)
{
	ID3D11ShaderResourceView *srvNULL[4];
	for (int i = 0; i < _amount; i++)
	{
		srvNULL[i] = NULL;
	}
	deviceContext->PSSetShaderResources(0, _amount, &srvNULL[0]);

	if (_rtvIndex == -1 && _depthIndex == -1)
	{
		deviceContext->OMSetRenderTargets(NULL, 0, NULL);
	}
	else if (_rtvIndex == -1)
	{
		deviceContext->OMSetRenderTargets(0, 0, sharedContainer->getDepthStencilView(_depthIndex));
	}
	else if (_depthIndex == -1)
	{
		deviceContext->OMSetRenderTargets(_amount, sharedContainer->getRenderTargetViews(_rtvIndex), NULL);
	}
	else
	{
		deviceContext->OMSetRenderTargets(_amount, sharedContainer->getRenderTargetViews(_rtvIndex), sharedContainer->getDepthStencilView(_depthIndex));
	}
}

void RenderDX11::setNullRenderTargetView()
{
	ID3D11RenderTargetView* nullView = NULL;
	deviceContext->OMSetRenderTargets(1, &nullView, NULL);
}

void RenderDX11::setDepthStencilState(int _index)
{
	deviceContext->OMSetDepthStencilState(sharedContainer->getDepthStencilState(_index), 0);
}

void RenderDX11::setSamplers()
{
	deviceContext->VSSetSamplers(0, samplers.size(), &samplers[0]);
	deviceContext->GSSetSamplers(0, samplers.size(), &samplers[0]);
	deviceContext->PSSetSamplers(0, samplers.size(), &samplers[0]);
	deviceContext->HSSetSamplers(0, samplers.size(), &samplers[0]);
	deviceContext->DSSetSamplers(0, samplers.size(), &samplers[0]);
}

void RenderDX11::setTopology(int _index)
{
	deviceContext->IASetPrimitiveTopology(topologys.at(_index));
}

void RenderDX11::setRasterState(int _index)
{
	deviceContext->RSSetState(rasterStates.at(_index));
}

void RenderDX11::setToScreenRender(int _depthStencilView)
{
	ID3D11ShaderResourceView *srvNULL[8];
	for (int i = 0; i < 8; i++)
	{
		srvNULL[i] = NULL;
	}
	deviceContext->PSSetShaderResources(0, 8, &srvNULL[0]);

	// Sets the render target to the screen
	if (_depthStencilView != -1)
	{
		deviceContext->OMSetRenderTargets(1, sharedContainer->getRenderTargetViews(renderTargetView), sharedContainer->getDepthStencilView(_depthStencilView));
	}
	else
	{
		deviceContext->OMSetRenderTargets(1, sharedContainer->getRenderTargetViews(renderTargetView), NULL);
	}

}

void RenderDX11::setInputLayout(int _index)
{
	deviceContext->IASetInputLayout(sharedContainer->getInputLayout(_index));
}

void RenderDX11::setBlendState(int _index)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	deviceContext->OMSetBlendState(sharedContainer->getBlendState(_index), blendFactor, 0xffffffff);
}

void RenderDX11::setRenderTargetAndDepthStencil(int _numOfRenderTargets, ID3D11RenderTargetView *_renderTargetView, ID3D11DepthStencilView *_depthStencilView)
{
	// Bind the render target view and the depth stencil buffer to the output render pipeline
	deviceContext->OMSetRenderTargets(_numOfRenderTargets, &_renderTargetView, _depthStencilView);
}

void RenderDX11::setShaders(int _vertexIndex, int _geometryIndex, int _hullIndex, int _domainIndex, int _pixelIndex)
{
	if (_vertexIndex == -1)
		deviceContext->VSSetShader(NULL, NULL, 0);
	else
		deviceContext->VSSetShader(sharedContainer->getVertexShader(_vertexIndex), NULL, 0);

	if (_geometryIndex == -1)
		deviceContext->GSSetShader(NULL, NULL, 0);
	else
		deviceContext->GSSetShader(sharedContainer->getGeometryShader(_geometryIndex), NULL, 0);

	if (_hullIndex == -1)
		deviceContext->HSSetShader(NULL, NULL, 0);
	else
		deviceContext->HSSetShader(sharedContainer->getHullShader(_hullIndex), NULL, 0);

	if (_domainIndex == -1)
		deviceContext->DSSetShader(NULL, NULL, 0);
	else
		deviceContext->DSSetShader(sharedContainer->getDomainShader(_domainIndex), NULL, 0);

	if (_pixelIndex == -1)
		deviceContext->PSSetShader(NULL, NULL, 0);
	else
		deviceContext->PSSetShader(sharedContainer->getPixelShader(_pixelIndex), NULL, 0);
}

void RenderDX11::setViewport(int _screenWidth, int _screenHeight)
{
	D3D11_VIEWPORT viewport;

	// Setup the viewport for rendering.
	viewport.Width = (float)_screenWidth;
	viewport.Height = (float)_screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	deviceContext->RSSetViewports(1, &viewport);
}

void RenderDX11::setComputeShader(ID3D11ComputeShader* _CSshader)
{
	deviceContext->CSSetShader(_CSshader, NULL, 0);
}

void RenderDX11::CSsetUAVs(UINT _length, UINT _index)
{
	//ID3D11UnorderedAccessView* t = sharedContainer->getUAV(_index);
	//ID3D11UnorderedAccessView* arrUAV[1] = { t };
	//deviceContext->CSSetUnorderedAccessViews(0,1,arrUAV,NULL);
	deviceContext->CSSetUnorderedAccessViews(0,_length,sharedContainer->getUAVs(_index),NULL);
}

void RenderDX11::CSsetShaderResources(UINT _length, UINT _index)
{
	deviceContext->CSSetShaderResources(0, _length, sharedContainer->getShaderResourceViews(_index));
}

void RenderDX11::CSsetShader(UINT _index)
{
	ID3D11ComputeShader* shader = sharedContainer->getComputeShader(_index);
	deviceContext->CSSetShader(shader, NULL, 0);
}

/**/
void RenderDX11::CSexec(UINT _uavLength, UINT _uavStartIndex, UINT _srvLength, UINT _srvStartIndex, UINT _csIndex, UINT _tdX, UINT _tdY, UINT _tdZ)
{
	// write to
	if(_uavLength > 0)
		CSsetUAVs(_uavLength, _uavStartIndex);

	// read from
	if(_srvLength > 0)
		CSsetShaderResources(_srvLength, _srvStartIndex);

	// shader
	CSsetShader(_csIndex);

	// execute
	deviceContext->Dispatch(_tdX, _tdY, _tdZ);

	// clean up
	deviceContext->CSSetShader(NULL,NULL,0);

	/* TODO: theres more cleaning that should be done (input/output)*/

	//deviceContext->CSSetShaderResources(0, _srvLength, NULL);
	//deviceContext->CSSetUnorderedAccessViews( 0, _uavStartIndex, NULL, NULL);
}

void RenderDX11::CSexecSingle(UINT _uavIndex, UINT _srvIndex, UINT _csIndex, UINT _tDx, UINT _tDy, UINT _tDz)
{
	// write
	ID3D11UnorderedAccessView* uavT			= sharedContainer->getUAV(_uavIndex);
	ID3D11UnorderedAccessView* arrUAVs[1]	= { uavT };
	deviceContext->CSSetUnorderedAccessViews(1, 1, arrUAVs, NULL);

	// read
	ID3D11ShaderResourceView* srvT			= getSRV(_srvIndex);
	ID3D11ShaderResourceView* arrViews[1]	= { srvT };
	deviceContext->CSSetShaderResources(1, 1, arrViews);

	ID3D11ComputeShader* shader = sharedContainer->getComputeShader(_csIndex);
	deviceContext->CSSetShader(shader, NULL, 0);

	deviceContext->Dispatch(_tDx, _tDy, _tDz);

	deviceContext->CSSetShader( NULL, NULL, 0 );
	arrViews[0] = NULL;
	deviceContext->CSSetShaderResources(1,1,arrViews);

	ID3D11UnorderedAccessView* nullUAV[1] = { NULL };
	deviceContext->CSSetUnorderedAccessViews(1,1,nullUAV,NULL);
}

void RenderDX11::CSexecDouble(UINT _uavIndex, UINT _srvIndex1, UINT _srvIndex2, UINT _csIndex, UINT _tDx, UINT _tDy, UINT _tDz)
{
	// write
	ID3D11UnorderedAccessView* uavT = sharedContainer->getUAV(_uavIndex);
	ID3D11UnorderedAccessView* arrUAVs[1] = { uavT };
	deviceContext->CSSetUnorderedAccessViews(1, 1, arrUAVs, NULL);

	// read
	ID3D11ShaderResourceView* srvT1 = getSRV(_srvIndex1);
	ID3D11ShaderResourceView* srvT2 = getSRV(_srvIndex2);
	ID3D11ShaderResourceView* arrViews[2];
	arrViews[0] = srvT1;
	arrViews[1] = srvT2;

	deviceContext->CSSetShaderResources(1, 2, arrViews);

	ID3D11ComputeShader* shader = sharedContainer->getComputeShader(_csIndex);
	deviceContext->CSSetShader(shader, NULL, 0);

	deviceContext->Dispatch(_tDx, _tDy, _tDz);

	deviceContext->CSSetShader(NULL, NULL, 0);
	arrViews[0] = NULL;
	arrViews[1] = NULL;
	deviceContext->CSSetShaderResources(1,2, arrViews);

	ID3D11UnorderedAccessView* nullUAV[1] = { NULL };
	deviceContext->CSSetUnorderedAccessViews(1, 1, nullUAV, NULL);
}

void RenderDX11::setNullVertexBuffers()
{
	deviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
}