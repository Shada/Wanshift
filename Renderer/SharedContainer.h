#pragma once
#include <map>

#include <vector>
#include "StructDef.h"
#include <iostream>
/////////////
// LINKING //
/////////////
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

//////////////
// INCLUDES //
//////////////
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>

#include "Common.h"
#include <map>

class SharedContainer
{
public:
	SharedContainer();
	~SharedContainer();

/*	Adds a shaderresourceview to the shaderresourceview array and get the index for it returned

	# _texture the shaderresourceview that is to be added to the array

	returns the index value for the shaderresourceview*/
	UINT addShaderResourceView(ID3D11ShaderResourceView *_shaderResourceView);

/*	Gets a pointer to the shaderresourceview array

	returns the shaderresourceview array starting at _index*/
	ID3D11ShaderResourceView **getShaderResourceViews(UINT _index);

/*	Adds a buffer to the buffer array and returns the index value for that buffer

	# _buffer the buffer that is to be added to the buffer array
	# _maxSize is the buffer's max amount of vertices

	returns the index value for the buffer that is added*/
	UINT addD3DBuffer(ID3D11Buffer *_buffer, UINT _maxSize);

	/*	Replaces an existing buffer with a new one

	# _index is the position in the buffer
	# _buffer the buffer that is to be added to the buffer array
	# _maxSize is the buffer's max amount of vertices

	returns the index value for the buffer that is added*/
	void setD3DBufferAt(UINT _index, ID3D11Buffer *_buffer, UINT _maxSize);

/*	Adds a cbuffer to the cbuffer array and returns the index value for that cbuffer

	# _cb the cbuffer that is to be added to the cbuffer array

	returns the index value for the cbuffer that is added*/
	UINT addCBuffer(ID3D11Buffer *_cb);

/*	Gets the vertex buffer at the index in the array
	# _index is the index in of the buffer in the array
	returns the vertex buffer array*/
	ID3D11Buffer *getVertexBuffer(UINT _index);


/*	Gets the maximum vertex amount of the buffer on the same index.
	# _index is the index in of the buffer in the array
	returns the maximum vertex amount of the selected buffer*/
	UINT getVertexBufferMaxSize(UINT _index);
	
	ID3D11Buffer **getVertexBuffers(UINT _index);

/*	Gets the constant buffer
	#_index is the index to the buffer in the array.
	returns the constant buffer at the index*/
	ID3D11Buffer** getCBuffer(UINT _index);

/*	Adds a rendertargetview to the rendertargetview array and get the index for it returned

	# _renderTargetView the rendertargetview that is to be added to the array

	returns the index value for the rendertargetview*/
	UINT addRenderTargetView(ID3D11RenderTargetView *_renderTargetView);

/*	Gets a pointer to a single rendertargetview at a index value from the rendertargetview array

	# _index the index value in the array from which to get the rendertargetview

	returns the rendertargetview at the index value*/
	ID3D11RenderTargetView *getRenderTargetView(UINT _index);

/*	Gets a referense to the rendertargetview array at with the index as start value

	# _index the place in the array from which the reference will start

	returns the a pointer to a specific point in the rendertargetview array*/
	ID3D11RenderTargetView** getRenderTargetViews(UINT _index);

/*	Returns the amount of render targets in the render target array*/
	UINT getRenderTargetSize();

/*	Returns the amount of depth stencil views in the depth stencil view array*/
	UINT getDepthStensilViewSize();

/*	Adds a texture2d to the texture2d array and get the index for it returned

	# _texture2D the texture2d that is to be added to the array

	returns the index value for the texture2d*/
	UINT addTexture2D(ID3D11Texture2D *_texture2D);

/*	Replaces an existing texture with a new one
	
	# _index is the index to the position in the vector containing textures
	# _texture2D the texture that will replace the old one
	*/
	void replaceTexture2D(UINT _index, ID3D11Texture2D *_texture2D);

	void replaceShaderResourceView(UINT _index, ID3D11ShaderResourceView *_shaderResourceView);

/*	Gets a pointer to a single texture2d at a index value from the texture2d array

	# _index the index value in the array from which to get the texture2d

	returns the texture2d at the index value*/
	ID3D11Texture2D *getTexture2D(UINT _index);

/*	Adds a texture3d to the texture3d array and returns its index

	#_texture3D: the element to be added to the array 
	
	#returns
		index for the element in the array */
	UINT addTexture3D(ID3D11Texture3D* _texture3D);

/*	Gets a pointer to a single texture3d at the specified index

	#_index: the index value for the element
	
	#returns
		if index is valid - ID3D11Texture3D* element
		if else - nullptr */
	ID3D11Texture3D* getTexture3D(UINT _index);

/*	Adds a vertex shader to the vertex array

	#_shader the shader to add to the array
	
	returns the index to the array position*/
	UINT	addVertexShader(ID3D11VertexShader *_shader, size_t _shaderSize);

/*	Adds a geometry shader to the geometry array

	#_shader the shader to add to the array
	
	returns the index to the array position*/
	UINT	addGeometryShader(ID3D11GeometryShader *_shader, size_t _shaderSize);

/*	Adds a hull shader to the hull array

	#_shader the shader to add to the array
	
	returns the index to the array position*/
	UINT	addHullShader(ID3D11HullShader *_shader, size_t _shaderSize);

/*	Adds a domain shader to the domain array

	#_shader the shader to add to the array
	
	returns the index to the array position*/
	UINT	addDomainShader(ID3D11DomainShader *_shader, size_t _shaderSize);

/*	Adds a pixel shader to the pixel array

	#_shader the shader to add to the array
	
	returns the index to the array position*/
	UINT	addPixelShader(ID3D11PixelShader *_shader, size_t _shaderSize);

/*	Returns a vertex shader from the vertex array

	#_index the index from which to get the shader
	
	returns a vertex shader*/
	ID3D11VertexShader		*getVertexShader(UINT _index);

/*	Returns a geometry shader from the geometry array

	#_index the index from which to get the shader
	
	returns a geometry shader*/
	ID3D11GeometryShader	*getGeometryShader(UINT _index);

/*	Returns a hull shader from the hull array

	#_index the index from which to get the shader
	
	returns a hull shader*/
	ID3D11HullShader		*getHullShader(UINT _index);

/*	Returns a domain shader from the domain array

	#_index the index from which to get the shader
	
	returns a domain shader*/
	ID3D11DomainShader		*getDomainShader(UINT _index);

/*	Returns a pixel shader from the pixel array

	#_index the index from which to get the shader
	
	returns a pixel shader*/
	ID3D11PixelShader		*getPixelShader(UINT _index);

	/**/
	ID3D11UnorderedAccessView*	getUAV(UINT _index);

	/**/
	ID3D11UnorderedAccessView**	getUAVs(UINT _index);

/*	Adds a depth stensil state to the depth stencil state array

	#_depthStencilState the depth stencil state to add the the array
	
	returns the index to the array position*/
	UINT addDepthStencilState(ID3D11DepthStencilState* _depthStencilState);

/*	Gets the depth stencil state from a index value

	#_index the index in the array to return the depth stencil state from
	
	returns the depth stencil state at the index value*/
	ID3D11DepthStencilState *getDepthStencilState(UINT _index);

/*	Adds a depth stensil view to the depth stencil view array

	#_depthStencilView the depth stencil view to add the the array
	
	returns the index to the array position*/
	UINT addDepthStencilView(ID3D11DepthStencilView* _depthStencilView);

/*	Gets the depth stencil view from a index value

	#_index the index in the array to return the depth stencil view from
	
	returns the depth stencil view at the index value*/
	ID3D11DepthStencilView *getDepthStencilView(UINT _index);

/*	Adds a input layout to the input layout array

	#_inputLayout the input layout to add the the array
	
	returns the index to the array position*/
	int addInputLayout(ID3D11InputLayout *_inputLayout, int _layoutType);

/*	Gets the input layout from a index value

	#_index the index in the array to return the input layout from
	
	returns the input layout at the index value*/
	ID3D11InputLayout *getInputLayout(UINT _index);


/*	Adds a blend state to the blend state array

	#_blendState the blend state to add the the array
	
	returns the index to the array position*/
	UINT addBlendState(ID3D11BlendState *_blendState);

/*	Gets the blend state from a index value

	#_index the index in the array to return the blend state from
	
	returns the blend state at the index value*/
	ID3D11BlendState *getBlendState(UINT _index);

	/**/
	UINT addComputeShader(ID3D11ComputeShader* _CSshader);

	UINT addUAV(ID3D11UnorderedAccessView* _uav);

	/**/
	ID3D11ComputeShader* getComputeShader(UINT _index);

/**/
	int getHandleIndex(std::string _handleName, ResourceType _handleType);

/**/
	void addHandleIndex(std::string _handleName, ResourceType _handleType, int _handleIndex);

	float getAllocBuffer();
	float getAllocTexture2D();
	float getAllocTexture3D();
	float getAllocRTV();
	float getAllocShader();
	void  logBackBufferSize(float _size);
	float getBackBufferSize();
	float getTotAlloc();

private:

	void release();

	std::map<ResourceType, std::map<std::string, int> > handles;

	std::vector<ID3D11ShaderResourceView*> shaderResourceViews;
	std::vector<ID3D11Buffer*> d3DBuffers;
	std::vector<UINT> d3DBufferMaxSizes;
	std::vector<ID3D11Buffer*> cbuffers;

	std::vector<ID3D11RenderTargetView*> renderTargetViews;
	std::vector<ID3D11Texture2D*> texture2Ds;
	std::vector<ID3D11Texture3D*> texture3Ds;

	std::vector<ID3D11VertexShader*> vertexShaders;
	std::vector<ID3D11GeometryShader*> geometryShaders;
	std::vector<ID3D11HullShader*> hullShaders;
	std::vector<ID3D11DomainShader*> domainShaders;
	std::vector<ID3D11PixelShader*> pixelShaders;

	std::vector<ID3D11DepthStencilState*> depthStensilStates;
	std::vector<ID3D11DepthStencilView*> depthStensilViews;

	std::vector<ID3D11InputLayout*> inputLayouts;

	std::vector<ID3D11BlendState*> blendStates;

	std::vector<ID3D11ComputeShader*> computeShaders;
	std::vector<ID3D11UnorderedAccessView*> uavs;

	std::map<int,float> allocTex2D, allocTex3D, allocBuffer, allocRTV, allocDSV;
	float allocBackBuffer;
	std::map<int, std::map<int,float> > allocShader;
};
