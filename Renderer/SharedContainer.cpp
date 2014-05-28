#include "SharedContainer.h"
#include "Utility_Render.h"

SharedContainer::SharedContainer()
{
	inputLayouts = std::vector<ID3D11InputLayout*>((UINT)LayoutTypes::LAYOUT_AMOUNT);
	for(UINT i = 0; i < (UINT)LayoutTypes::LAYOUT_AMOUNT; i++)
		inputLayouts[i] = NULL;
}

SharedContainer::~SharedContainer()
{
	release();
}

UINT SharedContainer::addShaderResourceView(ID3D11ShaderResourceView *_shaderResourceView)
{
	UINT index = shaderResourceViews.size();
	shaderResourceViews.push_back(_shaderResourceView);
	return index;
}

ID3D11ShaderResourceView** SharedContainer::getShaderResourceViews(UINT _index)
{
	return &shaderResourceViews[_index];
}

UINT SharedContainer::addD3DBuffer(ID3D11Buffer *_buffer, UINT _maxSize)
{
	UINT index = d3DBuffers.size();
	printf("index:%i\n",index);

	d3DBuffers.push_back(_buffer);
	d3DBufferMaxSizes.push_back(_maxSize);

	float size = getBufferMemSize(_buffer);
	//allocBuffer += size;
	allocBuffer[index] = size;

	return index;
}

void SharedContainer::setD3DBufferAt(UINT _index, ID3D11Buffer *_buffer, UINT _maxSize)
{
	SAFE_RELEASE(d3DBuffers.at(_index));

	d3DBuffers.at(_index) = _buffer;
	d3DBufferMaxSizes.at(_index) = _maxSize;
}

UINT SharedContainer::addCBuffer(ID3D11Buffer *_cb)
{
	UINT index = cbuffers.size();
	cbuffers.push_back(_cb);
	return index;
}

ID3D11Buffer **SharedContainer::getVertexBuffers(UINT _index)
{
	return &d3DBuffers[_index];
}

ID3D11Buffer *SharedContainer::getVertexBuffer(UINT _index)
{
	return d3DBuffers[_index];
}

UINT SharedContainer::getVertexBufferMaxSize(UINT _index)
{
	return d3DBufferMaxSizes[_index];
}

ID3D11Buffer **SharedContainer::getCBuffer(UINT _index)
{
	return &cbuffers[_index];
}

ID3D11UnorderedAccessView*	SharedContainer::getUAV(UINT _index)
{
	return uavs[_index];
}

ID3D11UnorderedAccessView**	SharedContainer::getUAVs(UINT _index)
{
	return &uavs[_index];
}
//
//UINT SharedContainer::addModel(Model *_model)
//{
//	UINT index = models.size();
//	models.pushback(_model);
//	return index;
//}

//Model* SharedContainer::getModel(UINT _index)
//{
//	return models[_index];
//}

UINT SharedContainer::addRenderTargetView(ID3D11RenderTargetView *_renderTargetView)
{
	UINT index = renderTargetViews.size();
	renderTargetViews.push_back(_renderTargetView);

	if(_renderTargetView != NULL)
	{
		//float size = getRTVMemSize(_renderTargetView);
		//allocRTV[index] = size;
	}

	return index;
}

ID3D11RenderTargetView *SharedContainer::getRenderTargetView(UINT _index)
{
	return renderTargetViews[_index];
}

UINT SharedContainer::getRenderTargetSize()
{
	return renderTargetViews.size();
}

UINT SharedContainer::getDepthStensilViewSize()
{
	return depthStensilViews.size();
}

ID3D11RenderTargetView **SharedContainer::getRenderTargetViews(UINT _index)
{
	return &renderTargetViews[_index];
}

UINT SharedContainer::addTexture2D(ID3D11Texture2D *_texture2D)
{
	UINT index = texture2Ds.size();
	texture2Ds.push_back(_texture2D);

	//float size = getTextureMemSize(_texture2D);
	//allocTex2D[index] = size;

	return index;
}

void SharedContainer::replaceTexture2D(UINT _index, ID3D11Texture2D *_texture2D)
{
	texture2Ds.at(_index)->Release();
	texture2Ds.at(_index) = _texture2D;
}

void SharedContainer::replaceShaderResourceView(UINT _index, ID3D11ShaderResourceView *_shaderResourceView)
{
	SAFE_RELEASE(shaderResourceViews.at(_index));
	shaderResourceViews.at(_index) = _shaderResourceView;
}

ID3D11Texture2D *SharedContainer::getTexture2D(UINT _index)
{
	return texture2Ds[_index];
}

UINT SharedContainer::addTexture3D(ID3D11Texture3D* _texture3D)
{
	UINT index = texture3Ds.size();
	texture3Ds.push_back(_texture3D);

	//float size = getTextureMemSize(_texture3D);
	//allocTex3D[index] = size;

	return index;
}

ID3D11Texture3D* SharedContainer::getTexture3D(UINT _index)
{
	if(_index >= texture3Ds.size() )
		return nullptr;

	return texture3Ds[_index];
}

UINT SharedContainer::addVertexShader(ID3D11VertexShader *_shader, size_t _shaderSize)
{
	UINT index = vertexShaders.size();
	vertexShaders.push_back(_shader);

	allocShader[0][index] = (float)_shaderSize;
	//printf("Vertex shaderSize(MB):%f\n",(float)_shaderSize / 1024.0f / 1024.0f);
	return index;
}
UINT SharedContainer::addGeometryShader(ID3D11GeometryShader *_shader, size_t _shaderSize)
{
	UINT index = geometryShaders.size();
	geometryShaders.push_back(_shader);

	allocShader[0][index] = (float)_shaderSize;
	//printf("Geometry shaderSize(MB):%f\n",(float)_shaderSize / 1024.0f / 1024.0f);
	return index;
}
UINT SharedContainer::addHullShader(ID3D11HullShader *_shader, size_t _shaderSize)
{
	UINT index = hullShaders.size();
	hullShaders.push_back(_shader);

	allocShader[0][index] = (float)_shaderSize;
	//printf("Hull shaderSize(MB):%f\n",(float)_shaderSize / 1024.0f / 1024.0f);
	return index;
}
UINT SharedContainer::addDomainShader(ID3D11DomainShader *_shader, size_t _shaderSize)
{
	UINT index = domainShaders.size();
	domainShaders.push_back(_shader);

	allocShader[0][index] = (float)_shaderSize;
	//printf("Domain shaderSize(MB):%f\n",(float)_shaderSize / 1024.0f / 1024.0f);
	return index;
}
UINT SharedContainer::addPixelShader(ID3D11PixelShader *_shader, size_t _shaderSize)
{
	UINT index = pixelShaders.size();
	pixelShaders.push_back(_shader);

	allocShader[0][index] = (float)_shaderSize;
	//printf("Pixel shaderSize(MB):%f\n",(float)_shaderSize / 1024.0f / 1024.0f);
	return index;
}
ID3D11VertexShader *SharedContainer::getVertexShader(UINT _index)
{
	return vertexShaders[_index];
}
ID3D11GeometryShader *SharedContainer::getGeometryShader(UINT _index)
{
	return geometryShaders[_index];
}
ID3D11HullShader *SharedContainer::getHullShader(UINT _index)
{
	return hullShaders[_index];
}
ID3D11DomainShader *SharedContainer::getDomainShader(UINT _index)
{
	return domainShaders[_index];
}
ID3D11PixelShader *SharedContainer::getPixelShader(UINT _index)
{
	return pixelShaders[_index];
}


UINT SharedContainer::addDepthStencilState(ID3D11DepthStencilState* _depthStencilState)
{
	UINT index = depthStensilStates.size();
	depthStensilStates.push_back(_depthStencilState);

	return index;
}

ID3D11DepthStencilState* SharedContainer::getDepthStencilState(UINT _index)
{
	return depthStensilStates[_index];
}

UINT SharedContainer::addDepthStencilView(ID3D11DepthStencilView* _depthStencilView)
{
	UINT index = depthStensilViews.size();
	depthStensilViews.push_back(_depthStencilView);
	return index;
}

ID3D11DepthStencilView *SharedContainer::getDepthStencilView(UINT _index)
{
	return depthStensilViews[_index];
}


int SharedContainer::addInputLayout(ID3D11InputLayout* _inputLayout, int _layoutType)
{
	if(_layoutType < (int)LayoutTypes::LAYOUT_AMOUNT)
	{
		if(inputLayouts[_layoutType] == NULL )
			inputLayouts[_layoutType] = _inputLayout;
		return _layoutType;
	}
	else
		return -1;
	
}

ID3D11InputLayout *SharedContainer::getInputLayout(UINT _index)
{
	return inputLayouts[_index];
}


UINT SharedContainer::addBlendState(ID3D11BlendState* _blendState)
{
	UINT index = blendStates.size();
	blendStates.push_back(_blendState);
	return index;
}



ID3D11BlendState *SharedContainer::getBlendState(UINT _index)
{
	return blendStates[_index];
}

UINT SharedContainer::addComputeShader(ID3D11ComputeShader* _CSshader)
{
	UINT index = computeShaders.size();
	computeShaders.push_back(_CSshader);
	return index;
}

ID3D11ComputeShader* SharedContainer::getComputeShader(UINT _index)
{
	return computeShaders[_index];
}

UINT SharedContainer::addUAV(ID3D11UnorderedAccessView* _uav)
{
	UINT index = uavs.size();
	uavs.push_back(_uav);
	return index;
}

int SharedContainer::getHandleIndex(std::string _handleName, ResourceType _handleType)
{
	
	std::map<ResourceType, std::map<std::string, int> >::iterator it = handles.find(_handleType);

	if(it != handles.end())
	{
		std::map<std::string, int>::iterator inner = it->second.find(_handleName);

		if (inner != handles[_handleType].end())
		{
			return inner->second;
		}
	}
	std::cout << "Could not find the recource named " << _handleName.c_str() <<". \n" ;
	return -1;
}

void SharedContainer::addHandleIndex(std::string _handleName, ResourceType _handleType, int _handleIndex)
{
	handles[_handleType].insert(std::pair<std::string, int>(_handleName, _handleIndex));
}


void SharedContainer::release()
{
	for(UINT i = 0; i < shaderResourceViews.size(); i++)
		SAFE_RELEASE(shaderResourceViews[i]);

	for(UINT i = 0; i < d3DBuffers.size(); i++)
		SAFE_RELEASE(d3DBuffers[i]);

	for(UINT i = 0; i < cbuffers.size(); i++)
		SAFE_RELEASE(cbuffers[i]);

	for(UINT i = 0; i < renderTargetViews.size(); i++)
		SAFE_RELEASE(renderTargetViews[i]);

	for(UINT i = 0; i < texture2Ds.size(); i++)
		SAFE_RELEASE(texture2Ds[i]);

	// shaders
	for(UINT i = 0; i < vertexShaders.size(); i++)
		SAFE_RELEASE(vertexShaders[i]);
	for(UINT i = 0; i < geometryShaders.size(); i++)
		SAFE_RELEASE(geometryShaders[i]);
	for(UINT i = 0; i < hullShaders.size(); i++)
		SAFE_RELEASE(hullShaders[i]);
	for(UINT i = 0; i < domainShaders.size(); i++)
		SAFE_RELEASE(domainShaders[i]);
	for(UINT i = 0; i < pixelShaders.size(); i++)
		SAFE_RELEASE(pixelShaders[i]);

	for(UINT i = 0; i < depthStensilStates.size(); i++)
		SAFE_RELEASE(depthStensilStates[i]);

	for(UINT i = 0; i < depthStensilViews.size(); i++)
		SAFE_RELEASE(depthStensilViews[i]);

	for(UINT i = 0; i < inputLayouts.size(); i++)
		SAFE_RELEASE(inputLayouts[i]);

	for(UINT i = 0; i < blendStates.size(); i++)
		SAFE_RELEASE(blendStates[i]);
}

float SharedContainer::getAllocBuffer()
{
	float totSize = 0;

	for(auto a : d3DBuffers)
	{
		totSize += getBufferMemSize(a);
	}

	return totSize;
}

float SharedContainer::getAllocTexture2D()
{
	float totSize = 0;
		for(auto a : texture2Ds)
	{
		totSize += getTextureMemSize(a);
	}
	return totSize;
}

float SharedContainer::getAllocTexture3D()
{
	float totSize = 0;
		for(auto a : texture3Ds)
	{
		totSize += getTextureMemSize(a);
	}
	return totSize;
}

float SharedContainer::getAllocRTV()
{
	return addAll<float>( allocRTV );
}

float SharedContainer::getAllocShader()
{
	float totSize = 0;

	for(int i = 0; i < 5; i++)
		totSize += addAll<float>( allocShader[i] );

	return totSize;
}

void SharedContainer::logBackBufferSize(float _size)
{
	allocBackBuffer = _size;
}

float SharedContainer::getBackBufferSize()
{
	return allocBackBuffer;
}

float SharedContainer::getTotAlloc()
{
	float totSize =	allocBackBuffer + getAllocBuffer() + 
					getAllocTexture2D() + getAllocTexture3D() + 
					getAllocShader();

	return totSize;
}