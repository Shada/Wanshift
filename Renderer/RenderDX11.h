#pragma once

#include "RenderInterface.h"
#include <string>
#include "SharedContainer.h"

#define nrOfSamplers 4

struct soDec
{
	std::vector<D3D11_SO_DECLARATION_ENTRY> soDeclaration;
	std::vector<std::string> name;
	int nrOfElements;
	UINT stride;
};

/*	RenderInterface implementation for DX11 version */
class RenderDX11
	: public RenderInterface
{
private:
	int cbSandStormIndex;
public:
	RenderDX11();
	~RenderDX11();

/*	Initializes Directx 11 
	
	# _screenWidth the width of the game window.
	# _screenHeight the height of the game window.
	# _hwnd the handle to the game window.

	# returns a bool. true if initialization succeded, false if the initialization encountered a error.*/
	bool inititalizeDirectX(int _screenWidth, int _screenHeight,bool _windowed, HWND _hwnd);

/*	Call to reset the window and change the window size and mode.
	
	# _
	sets the window mode. True for windowed, false for fullscreen.
	# _screenWidth the width of the window.
	# _screenHeight the height of the window.
	# _hwnd the handle for the window.
	
	returns a bool. True if the reset is successful false if it failed*/
	bool resetWindowMode(bool _windowed, int _screenWidth, int _screenHeight);



//////////////////////////////////
//			Add Functions		//
//////////////////////////////////

	/**/
	void addHandle(std::string _handleName, ResourceType _handleType, int _handleIndex);

//////////////////////////////////
//		Create Functions		//
//////////////////////////////////

/* create a cubeTextures 
	# _filename: Name of the texture
	
	returns index to the shader resource view in the sahred container */
	int createCubeTexture(std::string _filename);

/*	Create a shader resource view from a file

	# _fileName the name of the file where the shade resource is

	returns an index to the shader resource view in the shared container*/
	int createShaderResourceViewFromFile(std::string _fileName);
	
/* Create a default buffer containing a triangle

	returns a index value to the buffer in the shared container*/
	int createDefaultVertexBuffer();

/* create an immutabel vertex buffer cannot be altered or read from CPU, can only be read by the GPU 
	# _bufferData is the inital data that shall remain in the buffer for all eternity.
	# _bufferSize is the vertex amount for the buffer
	# _vertexSize is the size of each vertex. (_bufferSize * _vertexSize = total bytewidth.)
		
	returns the index to the buffer in the d3dbuffer array in the shared container. */
	int createVBufferImmutable(void** _bufferData, UINT _bufferSize, UINT _vertexSize);

/* create a dynamic vertex buffer that can be altered from the CPU, but not read, it can be read by the GPU.
	# _bufferSize is the maximum vertex amount for the buffer
	# _vertexSize is the size of each vertex. (_bufferSize * _vertexSize = total bytewidth.)
		
	returns the index to the buffer in the d3dbuffer array in the shared container. */
	int createVBufferDynamic(UINT _bufferSize, UINT _vertexSize);

/* create Immutable index buffer
	# _bufferData is initial data
	# _bufferSize is the maximum index amount for the buffer

	returns the index to the buffer in the d3dbuffer array in the shared container. */
	int createIndexBufferImmutable(void** _bufferData, UINT _bufferSize );

/* create dynamic index buffer
	# _bufferSize is the maximum index amount for the buffer

	returns the index to the buffer in the d3dbuffer array in the shared container. */
	int createIndexBufferDynamic( UINT _bufferSize );

/* create a default vertex buffer that can be altered and read from the GPU.
	# _bufferSize is the maximum vertex amount for the buffer
	# _vertexSize is the size of each vertex. (_bufferSize * _vertexSize = total bytewidth.)
		
	returns the index to the buffer in the d3dbuffer array in the shared container. */
	int createVBufferStreamout(UINT _bufferSize, UINT _vertexSize);

/* create a dynamic vertex buffer that holds the initial data values for the streamout.
	# _bufferSize is the maximum vertex amount for the buffer
	# _vertexSize is the size of each vertex. (_bufferSize * _vertexSize = total bytewidth.)
		
	returns the index to the buffer in the d3dbuffer array in the shared container. */
	int createVBufferInit(void** _bufferData, UINT _bufferSize, UINT _vertexSize);

/*	Creates and stores a texture2d in the SharedContainer

	# _width the width of the texture in pixels 
	# _height the height of the texture in pixels 

	returns an index to the texture2d in the shared container*/
	int createTexture2D(int _width, int _height);

/*	Creates and stores a texture2d in the SharedContainer

	# _width the width of the texture in pixels 
	# _height the height of the texture in pixels 

	returns an index to the texture2d in the shared container*/
	int createTexture2D32(int _width, int _height);

/*	Creates and stores a B8G8R8A8 texture2d in the SharedContainer

	# _width the width of the texture in pixels 
	# _height the height of the texture in pixels 

	returns an index to the texture2d in the shared container*/
	int createTexture2DB8G8R8A8(int _width, int _height);

/*	Creates and stores a texture3d in the sharedContainer

	#_width: texture width in texels
	#_height: texture height in texels
	#_depth: texture depth in texels
	
	#returns
		if success: index
		if else: -1 */
	int createTexture3D(UINT _width, UINT _height, unsigned int _depth) override;

/*	Creates a render target view from a texture2d

	# _index the index to the texture2d in SharedContainer

	returns an index to the render target view in the shared container*/
	int createRenderTargetView(UINT _index);

	int createShaderResourceViewTest(int _index);
/*	Creates a render target view from a texture2d with 32bit each channel

	# _index the index to the texture2d in SharedContainer

	returns an index to the render target view in the shared container*/
	int createRenderTargetView32(int _index);
	

/*	Creates a backbuffer-like render target view from a texture2d

	# _index the index to the texture2d in SharedContainer

	returns an index to the render target view in the shared container*/
	int createRenderTargetViewB8G8R8A8(int _index);

/*	creates a shader resource view and binds it to a texture2d

	# _index the index to the texture2d in SharedContainer

	returns an index to the shader resource view in the shared container*/
	int createShaderResourceView(UINT _index);

/*	creates a shader resource view and binds it to a texture2d

	# _index the index to the texture2d in SharedContainer

	returns an index to the shader resource view in the shared container*/

	int createShaderResourceView32(UINT _index);

/*	creates a shader resource view and binds it to a texture2d

	# _index the index to the texture2d in SharedContainer

	returns an index to the shader resource view in the shared container*/
	int createShaderResourceView32(int _index);

/*	creates a 8B8G8R8A shader resource view and binds it to a texture2d

	# _index the index to the texture2d in SharedContainer

	returns an index to the shader resource view in the shared container*/
	int createShaderResourceViewB8G8R8A8(int _index);

/* Creates and stores a texture2d in the SharedContainer, with initdata

	# _width the width of the texture in pixels 
	# _height the height of the texture in pixels 
	# _initData: the array containing the texture.
	returns an index to the texture2d in the shared container*/
	int createTexture2D(int _width, int _height, void **_initData);

/* Creates and stores a texture2d in the SharedContainer, with initdata

	# _width the width of the texture in pixels
	# _height the height of the texture in pixels
	# _initData: the array containing the texture.
	returns an index to the texture2d in the shared container*/
	int createTexture2D32(int _width, int _height, void **_initData);

/* update the data in a texture 

	# _bufferID: which texture to update
	# _width the width of the texture in pixels 
	# _height the height of the texture in pixels 
	# _data: the array containing the texture.
	returns true if successful and false if not*/
	bool updateTexture2D(int _textureID, int _width, int _height, void **_data, UINT _byteSizePerPixel);

/*	Creates and stores a one-channel texture2d in the SharedContainer

	# _width the width of the texture in pixels 
	# _height the height of the texture in pixels 

	returns an index to the texture2d in the shared container*/
	int createDepthTexture2D(int _width, int _height);

/**/
	int createComputeShader(std::string _CSshaderName) override;

/*	Creates a one-channel render target view from a texture2d

	# _index the index to the texture2d in SharedContainer

	returns an index to the render target view in the shared container*/
	int createDepthRenderTargetView(int _index);

/*	Creates a render target view from a texture3d

	#_index: texture element index in sharedContainer
	#_format: texture data format 
	
	#returns
		if success: index to rendertarget element
		if else: -1 */
	int createRenderTargetView3D(int _index) override;

/*	creates a one-channel shader resource view and binds it to a texture2d

	# _index the index to the texture2d in SharedContainer

	returns an index to the shader resource view in the shared container*/
	int createDepthShaderResourceView(int _index);

/*	Creates a one-channel render target view from a texture2d

	# _index the index to the texture2d in SharedContainer

	returns an index to the render target view in the shared container*/
	int createRenderTargetView1ch(int _index);

/*	Creates a shader resource view and binds it to a texture3d

	#_index: texture3d element index in sharedContainer
	
	#returns
		if success: shader resource element index
		if else: -1 */
	int createShaderResourceView3D(int _index)override;

/*	Should be called when the program shall exit*/
	void release();

/*	Compiles a shader. 
		
	#_shader: Path to the shader
	#_shaderModel: which shader model is going to be used.
		accepted arguments are: vs_5_0, gs_5_0, hs_5_0, ds_5_0, ps_5_0
	if the compilation fails PostQuitMessage() will be called and -1 will be returned.
	#returns: index to the shader in the shared container*/
	int createShader(std::string _shader, std::string _shaderModel, int _layoutType);

/*	Compiles a GeometryShader with streamout
	
	#_shader: Path to the shader
	#_shaderModel: which shader model is going to be used.
		accepted arguments are: vs_5_0, gs_5_0, hs_5_0, ds_5_0, ps_5_0
	if the compilation fails PostQuitMessage() will be called and -1 will be returned.
	#_layoutType

	#returns: index to the shader in the shared container*/
	int createGeometryStreamOut(std::string _shader, std::string _shaderModel);

	/**/
	int createUABuffer(void** _bufferData, UINT _bufferSize, UINT _bufferStride) override;

	/**/
	int createDepthTexture2DArray(int _width, int _height, int _arraySize);

	/**/
	int createDepthRenderTargetViewArray(int _index, int _arraySize);

	/**/
	int createDepthShaderResourceViewArray(int _index, int _arraySize);

	/**/
	int createCustomShaderResourceView(UINT _numElements, UINT _bufferIndex) override;

	/**/
	int createUAV(UINT _bufferIndex, UINT _numElements) override;

	/**/
	int createUAVTexture2D(int _width, int _height);

	/**/
	int createUAVTextureView(int _textureIndex);

//////////////////////////////
//		Set Functions		//
//////////////////////////////

/*	Tell this shaderclass to set its shaders to the graphics pipeline.
	if any of the indices are -1, that shader will be set to NULL
	# *Index: index to the shaders in the shared container. */
	void setShaders(int _vertexIndex, int _geomatryIndex, int _hullIndex, int _domainIndex, int _pixelIndex);

/*	sets shaderresources to the vertexshader
	#_index: index to the shader resource in the shared container */
	void setVertexShaderResource(int _startSlot, int _index, int _amount);

/*	sets shaderresources to the geometryshader
	#_index: index to the shader resource in the shared container */
	void setGeometryShaderResource(int _startSlot, int _index, int _amount);

/*	sets shaderresources to the hullshader
	#_index: index to the shader resource in the shared container */
	void setHullShaderResource(int _startSlot, int _index, int _amount);

/*	sets shaderresources to the domainxshader
	#_index: index to the shader resource in the shared container */
	void setDomainShaderResource(int _startSlot, int _index, int _amount);

/*	sets shaderresources to the pixelshader
	#_index: index to the shader resource in the shared container */
	void setPixelShaderResource(int _startSlot, int _index, int _amount);

/*	sets vertexbuffers
	#_startSlot: which start slot to put the vertexbuffer in
	#_index: index to the vertexbuffers in the shared container
	#_amount: amount of vertexbuffers to set
	#_strides: strides to be used in the buffers, must be the same size as _amount
	#_offsets: offsets to be used in the buffers, must be the same size as _amount*/
	void setVertexBuffers(int _startSlot, int _index, int _amount, UINT *_strides, UINT *_offsets);

/*	Sets an indexbuffer that another vertex buffer will use as reference
	#_index: index to the vertexbuffers in the shared container
	#_offsets: offsets to be used in the buffers*/
	void setIndexBuffer(int _index, UINT _offset);

	/**/
	void setComputeShader(ID3D11ComputeShader* _CSshader) override;

/*	sets rendertargets
	#_rtvIndex: index to the rendertargets in the shared container
	#_amount: amount of rendertargets to set
	#_dethIndex: index to the depthstencil in the shared container*/
	void setRenderTargetViews(int _rtvIndex, int _amount, int _depthIndex);

	/**/
	void setNullRenderTargetView() override;

	/**/
	void CSsetUAVs(UINT _length, UINT _index) override;

	/**/
	void CSsetShaderResources(UINT _length, UINT _index) override;

	/**/
	void CSsetShader(UINT _index) override;

	/**/
	void CSexec(UINT _uavLength, UINT _uavStartIndex, UINT _srvLength, UINT _srvStartIndex, UINT _csIndex, UINT _tdX = 32, UINT _tdY = 32, UINT _tdZ = 1) override;

	/**/
	void CSexecSingle(UINT _uavIndex, UINT _srvIndex, UINT _csIndex, UINT _tDx, UINT _tDy, UINT _tDz = 1) override;

	/**/
	void CSexecDouble(UINT _uavIndex, UINT _srvIndex1, UINT _srvIndex2, UINT _csIndex, UINT _tDx, UINT _tDy, UINT _tDz);

	/**/
	void setNullVertexBuffers() override;

/*	sets depthstencilstate
	#_index: index to the depthstencilstate in the shared container*/
	void setDepthStencilState(int _index);

/*	Sets the topology of the input data

	#_index the index to the topology

	0 = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	1 = D3D_PRIMITIVE_TOPOLOGY_POINTLIST
	2 = D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST*/
	void setTopology(int _index);

/* Sets the rasterizer state

	#_index the index to the raster state
	
	0 = Back face culling, filled solid frame*/
	void setRasterState(int _index);

/*	Sets the blend state

	#_index the index to the blend state

	0 = blend false
	1 = alpha blend
	2 = additive
	3 = writemask enable all
	4 = some other shit I don't know*/
	void setBlendState(int _index);

/*	Sets the input layout to the device

	#_index the index to the layout

	0 = Default layout*/
	void setInputLayout(int _index);

/*	Sets the stremout target*/
	void setSOTargets(int _nrOfTargets,int _targetIndex,UINT *_offset);

/*	Sets the streamout target to NULL*/
	void resetSOTargets();

/*	update constant buffer
	#_index is the index in the buffer in the shared container class ( and hopefully, the same index as in the shader. )
	#_data is the filled struct that is going to update the cbuffer.*/
	void updateCBuffer(UINT _index, const void *_data);

	void setCBuffers(UINT startSlot, UINT amount, UINT _index);

/*	update the vertex buffer with new data (Must be dynamic)
	#_index is the index to the buffer in the array
	#_mapData is the new data to be mapped to the buffer.
	# _bufferSize is the maximum vertex amount for the buffer
	# _vertexSize is the size of each vertex. (_bufferSize * _vertexSize = total bytewidth.)*/
	void updateVBuffer(UINT _index, void** _mapData, UINT _bufferSize, UINT _vertexSize);

/*	update the index buffer with new data (Must be dynamic)
	#_index is the index to the buffer in the array
	#_mapData is the new data to be mapped to the buffer.
	# _bufferSize is the maximum vertex amount for the buffer*/
	void updateIBuffer(UINT _index, void** _mapData, UINT _bufferSize);

	/*	replaces a buffer with a new buffer
	#_index is the index to the buffer in the array
	#_mapData is the new data to be mapped to the buffer.
	# _bufferSize is the maximum vertex amount for the buffer
	# _vertexSize is the size of each vertex. (_bufferSize * _vertexSize = total bytewidth.)*/
	void replaceImmutableVBuffer(UINT _index, void** _mapData, UINT _bufferSize, UINT _vertexSize);

/*	replaces a buffer with a new buffer
	#_index is the index to the buffer in the array
	#_mapData is the new data to be mapped to the buffer.
	# _bufferSize is the maximum vertex amount for the buffer*/
	void replaceImmutableIBuffer(UINT _index, void** _mapData, UINT _bufferSize);

/*	replaces a texture with a new one. does not need to be the same size as last one
	#_index is the index to the texture array
	#_width is the number of pixels in x-axis
	#_height is the number of pixels in y-axis
	#_initData is the data that should be contained in the texture */
	void replaceTexture2D(UINT _index, int _width, int _height, void **_initData);

/*	replaces a texture with a new one(32 bit). does not need to be the same size as last one
	#_index is the index to the texture array
	#_width is the number of pixels in x-axis
	#_height is the number of pixels in y-axis
	#_initData is the data that should be contained in the texture */
	void replaceTexture2D32(UINT _index, int _width, int _height, void **_initData);

/*	replaces an existing shader resource view
	#_index is the to the array of shader resources
	#_textureID is the index to the texute that should be bound to the resource*/
	void replaceShaderResourceViewTest(UINT _index, UINT _textureID);

/*	replaces an existing shader resource view (32 bit)
	#_index is the to the array of shader resources
	#_textureID is the index to the texute that should be bound to the resource*/
	void replaceShaderResourceView32(UINT _index, UINT _textureID);

/*	Sets the viewport for the device
	# _screenWidth the width of the window
	# _screenHeight the height of the window*/
	void setViewport(int _screenWidth, int _screenHeight);
//////////////////////////////
//		Get Functions		//
//////////////////////////////

/*	Returns the depth resource view index*/
	int getDepthResourceView();

/*	Get the resolution */
	glm::vec2 getResolution()override;

/*	Returns the shadow resource view index*/
	int getShadowResourceView();

/*	Clears all render targets and all depth stencil views*/
	void clearRenderTargets(glm::vec4 _clearColor);

/*	Clear single rendertaget */
	void clearRenderTargetSingle(UINT _renderTargetIndex, glm::vec4 _clearColor = glm::vec4(0,0,0,1))override;

	/**/
	void clearDepthStencil(UINT _depthStencilIndex) override;

/*	Shows the scene on the screen*/
	void presentSwapChain();

/**/
	int getDedicatedVRAM()override;

/**/
	int getUsedVRAM()override;
	
/* see RenderInterface*/
	int getRandomSeedIndex();

/*	Swaps the data in two buffers*/
	void swapBuffers(int* _bufferOneIndex, int* _bufferTwoIndex);

/**/
	int getDefaultTexture() { return defaultImageIndex; }

/**/
	int getBlackTexture() { return blackImageIndex; }

/*	Returns the index value from the handles with name _handleName of type _handleType

	#_handleName the name of the variable to get the index from
	#_handleType the type of variable to get
	
	returns a index value to the value of type _handleType assigned to _handleName*/
	int getHandleIndex(std::string _handleName, ResourceType _handleType);

	/**/
	ID3D11DeviceContext*		getDeviceContext() override;
	ID3D11Device*				getDevice() override;
	ID3D11UnorderedAccessView*	getUAV(UINT _index) override;
	ID3D11ShaderResourceView*	getSRV(UINT _index) override;

//////////////////////
//		DRAW		//
//////////////////////

/* Draw a number of vertex points from the inputed vertex buffer

	#_vertexCount the number of vertexes to draw
	#_startVertexLocation the start location from where to start reading vertexes*/
	void draw(UINT _vertexCount, UINT _startVertexLocation);

/*	Draw vertexes from the vertex buffer
	only for use by points that is expanded by the geometry shader*/
	void drawAuto();

/*	Render using a smaller vertex buffer and indices to map vertices

	#_vertexCount: the number of vertices to draw
	#_startVertexLocation: the start location from where to start reading indices
	#_baseVertexLocation: the start location from where to start reading vertices */
	void drawIndexed(UINT _indexCount, UINT _startIndexLocation, UINT _baseVertexLocation);
	
	void drawInstanced(UINT _vertexCount, UINT _instanceCount, UINT _startVertexLocation, UINT _startInstanceLocation);

	/**/
	void presentFinalScene()override;

	float getTotalVRAM()override;
	float getMemBuffers()override;
	float getMemShaders()override;
	float getMemTextures()override;
	float getMemBackBuffer()override;
private:

/*	Finds the video card description that fits screen resolution and gets the amount of video card memmory available
	
	# _screenWidth the width of the window
	# _screenHeight the height of the window

	returns true if a description is found and valid*/
	bool initSetCorrectVideoCardDescription(int _screenWidth, int _screenHeight);

/*	Creates the swapchain, device and divicecontext that will be used by directx
	
	# _screenWidth the width of the window
	# _screenHeight the height of the window
	# _hwnd the handle to the window

	returns true if a description is found and valid*/
	bool initCreateSwapChainAndDevice(int _screenWidth, int _screenHeight, HWND _hwnd);

/*	Creates a rendertargetview that points to the back buffer
	
	# _renderTargetView a pointer to the rendertargetview to be set

	returns true if a rendertargetview is created*/
	bool initCreateBackBufferRenderTargetView(int _renderTargetIndex);

/*	creates a constant buffer
	#_bytewidth is the byte size of the struct for the buffer
	#_registerIndex is the index, the cbuffer has in the shader.

	returns a bool indicating success or failure.*/
	bool initCreateCBuffer(unsigned int _byteWidth, std::string _name);
/* creates a constant buffer with is gonna be used to send in 
	   light sources data. This buffer will only be bound to pixelshaders, this is
	   why it is special. Otherwize the same as initCreateCBuffer()*/
	bool createLightCBuffer(unsigned int _byteWidth, unsigned int _registerIndex);
/*	Creates the depth buffer for directx, this is where the depth data will be stored from the shader pass
	
	# _screenWidth the width of the window
	# _screenHeight the height of the window

	returns true if a depthbuffer is created*/
	bool initCreateDepthBuffer(int _screenWidth, int _screenHeight);

/*	Creates a default input layout POSITION, NORMAL, TEXCOORD

	#_vertexBlob holds the input signatures for the layout
	
	returns a index to the layout in the shared container*/
	int createInputLayout(ID3DBlob *_vertexBlob,int layoutType);

/*	Creates a so declaration from a vertex blob

	#_vertexBlob holds the input signatures for the declaration
	
	returns a index to the declaration in the shared container*/
	void createSODeclaration(ID3DBlob *_shaderBlob);

/*	Creates the samplers that is used by the shaders

	returns true if thay are created succesfully*/
	bool initCreateSamplers();
	
/*	Creates the topologys that is used in the shaders*/
	void initCreateTopologys();

/*	Creates the blend states that is used by the shaders*/
	bool initCreateBlendStates();

/*	Binds a rendertarget and a depth stencil to the graphics device

	# _numOfRenderTargets the amount of rendertargets that the view contains ranges from 1 - 4
	# _renderTargetView the pointer to the rendertarget/s 
	# _depthStencilView the pointer to the depthstencil that will contail the depth data*/
	void setRenderTargetAndDepthStencil(int _numOfRenderTargets,ID3D11RenderTargetView *_renderTargetView, ID3D11DepthStencilView *_depthStencilView);

/*	Sets the rasteriser state to filled solid and the culling to backface

	returns true if the raster state is created and the state is set*/
	bool initCreateRasterState();

/*	Sets the render target to output to the screen*/
	void setToScreenRender(int _depthStencilView);

/*	Sets all the samplers to the shaders*/
	void setSamplers();

/*	creates all the resources needed for the volume shading pass*/
	void initCreateShadowResources(int _screenWidth, int _screenHeight);

/**/
	bool initHandleTextures();

	soDec soDeclaration;

	int videoCardMemory;
	char videoCardDescription[128];
	bool windowed;

	IDXGISwapChain* swapChain;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	int renderTargetView;

	int depthStencilTexture;
	int depthStencilState;
	int depthStencilView;

	int shadowTexture;
	int shadowView;
	int shadowState;

	int depthShaderResource;

	unsigned int defaultImageIndex;
	unsigned int blackImageIndex;
	unsigned int randomSeedIndex;

	SharedContainer *sharedContainer;

	std::vector<ID3D11SamplerState*> samplers;
	
	std::vector<D3D_PRIMITIVE_TOPOLOGY> topologys;

	std::vector<ID3D11RasterizerState*> rasterStates;

	HWND hWnd;

	int screenWidth;
	int screenHeight;

	int backBufferRenderTarget;
	int finalSceneShaderPS, finalSceneShaderVS;

	float totalAllocatedVRAM()override;

	unsigned int numerator, denominator;
};

