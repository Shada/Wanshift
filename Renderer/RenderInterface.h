#pragma once
#include <windows.h>
#include <string>
#include <d3d11.h>
#include <glm\glm.hpp>
#include "Common.h"
#include "StructDef.h"

#include <vector>


/* Version numbering of DirectX */
enum class Version
{
	UNDEFINED,
	DX10,
	DX11
};

/* vertex buffers */
struct DefaultVertex
{
	glm::vec3 pos;			//position
	glm::vec3 normal;		//normal
	glm::vec2 texCoord;		//texture coordinates
};
struct NormallessVertex
{
	glm::vec3 pos;			//position
	glm::vec2 texCoord;		//texture coordinates
};
struct grassVertex
{
	glm::vec3 pos;
};
struct grassVertexPerInstance
{
	glm::vec3 pos;
};
/* constant buffers */
struct CBWorldData
{
	glm::mat4x4 mWorld;		//world matrix
	glm::mat4x4 mWorldInv;	//world inverse
};

struct CBViewData
{
	glm::vec4 cameraPos;	//camera position
	glm::vec4 cameraDir;	//camera direction
	glm::mat4x4 mView;		//view matrix
	glm::mat4x4 mViewInv;	//view inverse
};

struct CBOnceData
{
	glm::mat4x4 mProj;		//projection matrix
	glm::mat4x4 mProjInv;	//projection inverse
	glm::vec2 resolution;	//screen resolution
	UINT nSamples;	// 4
	float scaleDepth;		// 4
	float scaleOverDepth;	// 4
	glm::vec3 padding;			// 4
};

struct CBTextData
{
	glm::vec4 tintAlpha;	//rgb tint, a alpha
	glm::vec2 pos;			//world position
	glm::vec2 scale;		//world scale
	glm::vec2 imageSize;	//resolution of font image
	//float rotation;			//world rotation
	glm::vec2 padding;		//padding
};

struct CBLightInfoData
{
	int lightCount;
	glm::vec3 lightDir;
	glm::vec4 pos[25];
	glm::vec4 color[25];
	glm::vec4 reach[25];
	glm::mat4x4 lightView;
	glm::mat4x4 lightProj;
	glm::mat4x4 toShadowSpace;
	glm::mat4 CascadeViewProj[4];
};
struct CBSpotLight
{
	int lightCount;
	glm::vec3 reach_cosTheta_cosPhi;
	glm::vec4 pos[5];
	glm::vec4 color[5];
	glm::vec4 dir[5];

};
struct CBTerrainInfo
{
	glm::vec2 startPos;
	glm::vec2 localHeightPos;
	float pointStep;
	glm::vec3 padding;
	glm::vec4 tinyChunkPos[9];
};

struct CBEveryFrame
{
	float		timeStep;
	float		gameTime;
	glm::vec2	windDirection;
	float		timeofDay;
	float		energy;
	glm::vec2	padding;
	glm::vec4	dayBool;
};

struct CBParticleStart
{
	glm::vec3 pos;
	int effectType;
	glm::vec3 dir;
	float pad;
};

struct CBParticleEnergy
{
	glm::vec4 pos[20];
	glm::vec4 goal[20];
	float time;
	float lifeTime;
	float radie;
	float pad;
};

struct CBSandStorm
{
	glm::mat4x4	DepthView;
	glm::vec3 pos;
	float age;
};
struct CBWisp
{
	glm::vec4 pos;
	glm::vec4 color;
};
struct CBWorldAttributes
{
	/* Terrain */
	float innerRadius;		// 4
	float outerRadius;		// 4
	float reflectance;		// 4
	/* Sun */
	float brightness;	// 4	
};							// = 16

struct CBScatterAttributes
{
	/* Rayleigh */
	float kr4PI;	// 4
	float km4PI;	// 4
	float krEsun;	// 4	
	float kmEsun;	// 4
	
	float gM;		// 4	
	glm::vec3 wM;	// 12

	float exposure;	// 4
	glm::vec3 wR;	// 12	
	
};					// = 48

struct CBRainConstants
{
	glm::mat4x4	rViewProjMatrix;
	glm::vec3	viewDir;
	float		rScale;
	glm::vec4	ambientColor;
};

struct CBRainSimulationConstants
{
	glm::vec3	boundCenter;
	float		rDt;
	glm::vec3	boundHalfSize;
	float		windVariation;
	glm::vec2	windForce;
	float		verticalSpeed;
	float		heightMapHalfSize;
	float		stretch;
	glm::vec3	rscPadding3;
};

struct CBAnimationMatrices
{
	float interpolation[4];
	glm::mat4 mat1[15];
	glm::mat4 mat2[15];
};

struct FogSim
{
	glm::vec3	color;
	float		startDepth;
	glm::vec3	highlightColor;
	float		globalDensity;
	glm::vec3	sunDir;
	float		heightFalloff;
};

struct cbHeat
{
	float heatDistance;
	float vp_inv_width;
	float vp_inv_height;
	float offsetScale;
	float hDt;
	glm::vec3 hPadding;
};

struct CBBiomShift
{
	float interpVal;
	glm::vec3 padding;
};

struct cbCloud 
{
	glm::vec2	offset;
	UINT		size;
	float		cellSize;
	UINT		intervalStart;
	UINT		intervalEnd;
	glm::vec2	cloudPad;
};

/* Interface for render instance */
class RenderInterface
{
public:
	RenderInterface(Version _dxVersion = Version::UNDEFINED);
	virtual			~RenderInterface();

	/*	Initializes Directx 11

		# _screenWidth the width of the game window.
		# _screenHeight the height of the game window.
		# _hwnd the handle to the game window.

		# returns a bool. true if initialization succeded, false if the initialization encountered a error.*/
	virtual bool inititalizeDirectX(int _screenWidth, int _screenHeight, bool _windowed, HWND _hwnd) = 0;

	/*	Call to reset the window and change the window size and mode.

		# _windowed sets the window mode. True for windowed, false for fullscreen.
		# _screenWidth the width of the window.
		# _screenHeight the height of the window.
		# _hwnd the handle for the window.

		returns a bool. True if the reset is successful false if it failed*/
	virtual bool resetWindowMode(bool _windowed, int _screenWidth, int _screenHeight) = 0;

	/* Release all held data before either destroying or resetting the instance */
	virtual void	release() = 0;


	//////////////////////////////////
	//			Add Functions		//
	//////////////////////////////////

	/*Adds a new handle to the handleArray with a specific name and a index value

		#_handleName the name for the new handle
		#_handleType the type of the handle
		#_handleIndex the index value to a element of _handleType*/
	virtual void addHandle(std::string _handleName, ResourceType _handleType, int _handleIndex) = 0;

	//////////////////////////////////
	//		Create Functions		//
	//////////////////////////////////

	/* create a cubeTextures
		# _filename: Name of the texture

		returns index to the shader resource view in the sahred container */
	virtual int createCubeTexture(std::string _fileName) = 0;

	/**/
	virtual int createDepthTexture2DArray(int _width, int _height, int _arraySize) = 0;

	/**/
	virtual int createDepthRenderTargetViewArray(int _index, int _arraySize) = 0;

	/**/
	virtual int createDepthShaderResourceViewArray(int _index, int _arraySize) = 0;

	/*	Create a shader resource view from a file

		# _fileName the name of the file where the shade resource is

		returns an index to the shader resource view in the shared container*/
	virtual int createShaderResourceViewFromFile(std::string _fileName) = 0;

	/* Create a default buffer containing a triangle

		returns a index value to the buffer in the shared container*/
	virtual int createDefaultVertexBuffer() = 0;

	/* create an immutabel vertex buffer cannot be altered or read from CPU, can only be read by the GPU
		# _bufferData is the inital data that shall remain in the buffer for all eternity.
		# _bufferSize is the vertex amount for the buffer
		# _vertexSize is the size of each vertex. (_bufferSize * _vertexSize = total bytewidth.)

		returns the index to the buffer in the d3dbuffer array in the shared container. */
	virtual int createVBufferImmutable(void** _bufferData, UINT _bufferSize, UINT _vertexSize) = 0;

	/* create a dynamic vertex buffer that can be altered from the CPU, but not read, it can be read by the GPU.
		# _bufferSize is the maximum vertex amount for the buffer
		# _vertexSize is the size of each vertex. (_bufferSize * _vertexSize = total bytewidth.)

		returns the index to the buffer in the d3dbuffer array in the shared container. */
	virtual int createVBufferDynamic(UINT _bufferSize, UINT _vertexSize) = 0;

	/* create Immutable index buffer
		# _bufferData is initial data
		# _bufferSize is the maximum index amount for the buffer

		returns the index to the buffer in the d3dbuffer array in the shared container. */
	virtual int createIndexBufferImmutable(void** _bufferData, UINT _bufferSize) = 0;

	/* create dynamic index buffer
		# _bufferSize is the maximum index amount for the buffer

		returns the index to the buffer in the d3dbuffer array in the shared container. */
	virtual int createIndexBufferDynamic(UINT _bufferSize) = 0;

	/* create a default vertex buffer that can be altered and read from the GPU.
		# _bufferSize is the maximum vertex amount for the buffer
		# _vertexSize is the size of each vertex. (_bufferSize * _vertexSize = total bytewidth.)

		returns the index to the buffer in the d3dbuffer array in the shared container. */
	virtual int createVBufferStreamout(UINT _bufferSize, UINT _vertexSize) = 0;

	/* create a dynamic vertex buffer that holds the initial data values for the streamout.
		# _bufferSize is the maximum vertex amount for the buffer
		# _vertexSize is the size of each vertex. (_bufferSize * _vertexSize = total bytewidth.)

		returns the index to the buffer in the d3dbuffer array in the shared container. */
	virtual int createVBufferInit(void** _bufferData, UINT _bufferSize, UINT _vertexSize) = 0;

	/*	Creates and stores a texture2d in the SharedContainer

		# _width the width of the texture in pixels
		# _height the height of the texture in pixels

		returns an index to the texture2d in the shared container*/
	virtual int createTexture2D(int _width, int _height) = 0;

	/*	Creates and stores a texture2d in the SharedContainer

	# _width the width of the texture in pixels
	# _height the height of the texture in pixels

	returns an index to the texture2d in the shared container*/
	virtual int createTexture2D32(int _width, int _height) = 0;

/*	Creates and stores a B8G8R8A8 texture2d in the SharedContainer

	# _width the width of the texture in pixels 
	# _height the height of the texture in pixels 

	returns an index to the texture2d in the shared container*/
	virtual int createTexture2DB8G8R8A8(int _width, int _height) = 0;

/*	Creates and stores a texture3d in the sharedContainer
		#_width: texture width in texels
		#_height: texture height in texels
		#_depth: texture depth in texels
		#returns
		if success: index
		if else: -1 */
	virtual int createTexture3D(UINT _width, UINT _height, UINT _depth) = 0;

	/*	Creates a render target view from a texture2d

		# _index the index to the texture2d in SharedContainer

		returns an index to the render target view in the shared container*/
	virtual int createRenderTargetView(UINT _index) = 0;

	virtual int createShaderResourceViewTest(int _index) = 0;
	
	/*	creates a shader resource view and binds it to a texture2d

		# _index the index to the texture2d in SharedContainer

		returns an index to the shader resource view in the shared container*/
	virtual int createShaderResourceView(UINT _index) = 0;

	/*	creates a shader resource view and binds it to a texture2d

	# _index the index to the texture2d in SharedContainer

	returns an index to the shader resource view in the shared container*/

	virtual int createShaderResourceView32(UINT _index) = 0;

/*	Creates a backbuffer-like render target view from a texture2d

	# _index the index to the texture2d in SharedContainer

	returns an index to the render target view in the shared container*/
	virtual int createRenderTargetViewB8G8R8A8(int _index) = 0;

/*	creates a 8B8G8R8A shader resource view and binds it to a texture2d

	# _index the index to the texture2d in SharedContainer

	returns an index to the shader resource view in the shared container*/
	virtual int createShaderResourceViewB8G8R8A8(int _index) = 0;
	
/* Creates and stores a texture2d in the SharedContainer, with initdata
	# _width the width of the texture in pixels
	# _height the height of the texture in pixels
	# _initData: the array containing the texture.
	returns an index to the texture2d in the shared container*/
	virtual int createTexture2D(int _width, int _height, void **_initData) = 0;

	/* Creates and stores a texture2d in the SharedContainer, with initdata

	# _width the width of the texture in pixels
	# _height the height of the texture in pixels
	# _initData: the array containing the texture.
	returns an index to the texture2d in the shared container*/
	virtual int createTexture2D32(int _width, int _height, void **_initData) = 0;

	/*	Creates a computeShader	
	
		#_CSshaderName: name of the file
		
		#returns
			index number if successful creation
			-1 if else */
	virtual int createComputeShader(std::string _CSshaderName) = 0;

	/* update the data in a texture

		# _bufferID: which texture to update
		# _width the width of the texture in pixels
		# _height the height of the texture in pixels
		# _data: the array containing the texture.
		# _byteSizePerPixel: how big is one pixel in the texure gonna be
		returns true if successful and false if not*/
	virtual bool updateTexture2D(int _textureID, int _width, int _height, void **_data, UINT _byteSizePerPixel) = 0;

	/*	Creates and stores a one-channel texture2d in the SharedContainer

		# _width the width of the texture in pixels
		# _height the height of the texture in pixels

		returns an index to the texture2d in the shared container*/
	virtual int createDepthTexture2D(int _width, int _height) = 0;

	/*	Creates a one-channel render target view from a texture2d

		# _index the index to the texture2d in SharedContainer

		returns an index to the render target view in the shared container*/
	virtual int createDepthRenderTargetView(int _index) = 0;

	/*	Creates a render target view from a texture3d

		#_index: texture element index in sharedContainer
		#_format: texture data format

		#returns
		if success: index to rendertarget element
		if else: -1 */
	virtual int createRenderTargetView3D(int _index) = 0;

	/*	creates a one-channel shader resource view and binds it to a texture2d
		# _index the index to the texture2d in SharedContainer

		returns an index to the shader resource view in the shared container*/
	virtual int createDepthShaderResourceView(int _index) = 0;

	/*	Creates a render target view from a texture2d

		# _index the index to the texture2d in SharedContainer

		returns an index to the render target view in the shared container*/
	virtual int createRenderTargetView1ch(int _index) = 0;

	/*	Creates a shader resource view and binds it to a texture3d

		#_index: texture3d element index in sharedContainer

		#returns
		if success: shader resource element index
		if else: -1 */
	virtual int createShaderResourceView3D(int _index) = 0;

	/*	Compiles a shader.

		#_shader: Path to the shader
		#_shaderModel: which shader model is going to be used.
		accepted arguments are: vs_5_0, gs_5_0, hs_5_0, ds_5_0, ps_5_0
		if the compilation fails PostQuitMessage() will be called and -1 will be returned.
		#returns: index to the shader in the shared container*/
	virtual int	createShader(std::string _shader, std::string _shaderModel, int _layoutType) = 0;

	/*	Compiles a GeometryShader with streamout

		#_shader: Path to the shader
		#_shaderModel: which shader model is going to be used.
		accepted arguments are: vs_5_0, gs_5_0, hs_5_0, ds_5_0, ps_5_0
		if the compilation fails PostQuitMessage() will be called and -1 will be returned.
		#_layoutType

		#returns: index to the shader in the shared container*/
	virtual int createGeometryStreamOut(std::string _shader, std::string _shaderModel) = 0;

	/**/
	virtual int createUABuffer(void** _bufferData, UINT _bufferSize, UINT _bufferStride) = 0;

	/**/
	virtual int createCustomShaderResourceView(UINT _numElements, UINT _bufferIndex) = 0;
	/**/
	virtual int createUAV(UINT _bufferIndex, UINT _numElements) = 0;

	/**/
	virtual int createUAVTexture2D(int _width, int _height) = 0;

	/**/
	virtual int createUAVTextureView(int _textureIndex) = 0;


	/*	Tell this shaderclass to set its shaders to the graphics pipeline.
		if any of the indices are -1, that shader will be set to NULL.
		# *Index: index to the shaders in the shared container. */
	virtual void	setShaders(int _vertexIndex, int _geomatryIndex, int _hullIndex, int _domainIndex, int _pixelIndex) = 0;

	/*	update constant buffer
		#_index is the index in the buffer in the shared container class ( and hopefully, the same index as in the shader. )
		#_data is the filled struct that is going to update the cbuffer.*/
	virtual void	updateCBuffer(UINT _index, const void *_data) = 0;

	/* sets cbuffers to all types of shaders on specified register index*/
	virtual void	setCBuffers(UINT startSlot, UINT amount, UINT _index) = 0;


	/*	update the vertex buffer with new data (Must be dynamic)
		#_index is the index to the buffer in the array
		#_mapData is the new data to be mapped to the buffer.
		# _bufferSize is the maximum vertex amount for the buffer
		# _vertexSize is the size of each vertex. (_bufferSize * _vertexSize = total bytewidth.)*/
	virtual void	updateVBuffer(UINT _index, void** _mapData, UINT _bufferSize, UINT _vertexSize) = 0;

/*	update the index buffer with new data (Must be dynamic)
	#_index is the index to the buffer in the array
	#_mapData is the new data to be mapped to the buffer.
	# _bufferSize is the maximum vertex amount for the buffer*/
	virtual void updateIBuffer(UINT _index, void** _mapData, UINT _bufferSize) = 0;

	/*	replaces a buffer with a new buffer
	#_index is the index to the buffer in the array
	#_mapData is the new data to be mapped to the buffer.
	# _bufferSize is the maximum vertex amount for the buffer
	# _vertexSize is the size of each vertex. (_bufferSize * _vertexSize = total bytewidth.)*/
	virtual void replaceImmutableVBuffer(UINT _index, void** _mapData, UINT _bufferSize, UINT _vertexSize) = 0;

	/*	replaces a buffer with a new buffer
	#_index is the index to the buffer in the array
	#_mapData is the new data to be mapped to the buffer.
	# _bufferSize is the maximum vertex amount for the buffer*/
	virtual void replaceImmutableIBuffer(UINT _index, void** _mapData, UINT _bufferSize) = 0;

	/*	replaces a texture with a new one. does not need to be the same size as last one
	#_index is the index to the texture array
	#_width is the number of pixels in x-axis
	#_height is the number of pixels in y-axis
	#_initData is the data that should be contained in the texture */
	virtual void replaceTexture2D(UINT _index, int _width, int _height, void **_initData) = 0;

	/*	replaces a texture with a new one(32 bit). does not need to be the same size as last one
	#_index is the index to the texture array
	#_width is the number of pixels in x-axis
	#_height is the number of pixels in y-axis
	#_initData is the data that should be contained in the texture */
	virtual void replaceTexture2D32(UINT _index, int _width, int _height, void **_initData) = 0;

	/*	replaces an existing shader resource view
	#_index is the to the array of shader resources
	#_textureID is the index to the texute that should be bound to the resource*/
	virtual void replaceShaderResourceViewTest(UINT _index, UINT _textureID) = 0;

	/*	replaces an existing shader resource view
	#_index is the to the array of shader resources
	#_textureID is the index to the texute that should be bound to the resource*/
	virtual void replaceShaderResourceView32(UINT _index, UINT _textureID) = 0;

	//////////////////////////////
	//		Set Functions		//
	//////////////////////////////

	/*	sets shaderresources to the vertexshader
		#_index: index to the shader resource in the shared container */
	virtual void setVertexShaderResource(int _startSlot, int _index, int _amount) = 0;

	/*	sets shaderresources to the geometryshader
		#_index: index to the shader resource in the shared container */
	virtual void setGeometryShaderResource(int _startSlot, int _index, int _amount) = 0;

	/*	sets shaderresources to the hullshader
		#_index: index to the shader resource in the shared container */
	virtual void setHullShaderResource(int _startSlot, int _index, int _amount) = 0;

	/*	sets shaderresources to the domainxshader
		#_index: index to the shader resource in the shared container */
	virtual void setDomainShaderResource(int _startSlot, int _index, int _amount) = 0;

	/*	sets shaderresources to the pixelshader
		#_index: index to the shader resource in the shared container */
	virtual void setPixelShaderResource(int _startSlot, int _index, int _amount) = 0;

	/*	sets vertexbuffers
		#_index: index to the vertexbuffers in the shared container
		#_amount: amount of vertexbuffers to set
		#_strides: strides to be used in the buffers, must be the same size as _amount
		#_offsets: offsets to be used in the buffers, must be the same size as _amount*/
	virtual void setVertexBuffers(int _startSlot, int _index, int _amount, UINT *_strides, UINT *_offsets) = 0;

	virtual void setIndexBuffer(int _index, UINT _offset) = 0;

	/*	sets computeshader
	
		#_CSshader: shader pointer */
	virtual void setComputeShader(ID3D11ComputeShader* _CSshader) = 0;

	/*	sets rendertargets
		#_rtvIndex: index to the rendertargets in the shared container
		#_amount: amount of rendertargets to set
		#_dethIndex: index to the depthstencil in the shared container*/
	virtual void setRenderTargetViews(int _rtvIndex, int _amount, int _depthIndex) = 0;

	/**/
	virtual void setNullRenderTargetView() = 0;
		
	/**/
	virtual void CSsetUAVs(UINT _length, UINT _index) = 0;

	/**/
	virtual void CSsetShaderResources(UINT _length, UINT _index) = 0;

	/**/
	virtual void CSsetShader(UINT _index) = 0;

	/**/
	virtual void CSexec(UINT _uavLength, UINT _uavStartIndex, UINT _srvLength, UINT _srvStartIndex, UINT _csIndex, UINT _tdX = 32, UINT _tdY = 32, UINT _tdZ = 1) = 0;

	/**/
	virtual void CSexecSingle(UINT _uavIndex, UINT _srvIndex, UINT _csIndex, UINT _tDx, UINT _tDy, UINT _tDz = 1) = 0;

	/**/
	virtual void CSexecDouble(UINT _uavIndex, UINT _srvIndex1, UINT _srvIndex2, UINT _csIndex, UINT _tDx, UINT _tDy, UINT _tDz) = 0;

	virtual void setNullVertexBuffers() = 0;

/*	sets depthstencilstate
	#_index: index to the depthstencilstate in the shared container
	
	0 = backface, depth enabled, depth write all, stencil enabled
	1 = shadow, depth enabled, depth write zero, stencil enabled
	2 = backface, depth enabled, depth write zero, stencil enabled
	3 = backface, depth disabled, depth write zero, stencil disabled
	4 = backface, depth disabled, depth write zero, stencil enabled*/
	virtual void setDepthStencilState(int _index) = 0;

	/*	Sets the topology of the input data

		#_index the index to the topology

		0 = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		1 = D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		2 = D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST*/
	virtual void setTopology(int _index) = 0;

	/* Sets the rasterizer state
		#_index the index to the raster state

		0 = Back face culling, filled solid frame
		1 = Back face culling, filled, front counter clock
		2 = No cull*/
	virtual void setRasterState(int _index) = 0;

	/*	Sets the viewport for the device

		# _screenWidth the width of the window
		# _screenHeight the height of the window*/
	virtual void setViewport(int _screenWidth, int _screenHeight) = 0;

	/*	Sets the stremout target*/
	virtual void setSOTargets(int _nrOfTargets, int _targetIndex, UINT *_offset) = 0;

	/*	Sets the streamout target to NULL*/
	virtual void resetSOTargets() = 0;

	/*	Sets the blend state

		#_index the index to the blend state

		0 = blend false
		1 = alpha blend
		2 = additive
		3 = writemask enable all
		4 = some other shit I don't know*/
	virtual void setBlendState(int _index) = 0;

	/*	Sets the render target to output to the screen*/
	virtual void setToScreenRender(int _depthStencilView) = 0;

	/*	Sets the input layout to the device

		#_index the index to the layout

		0 = Default layout*/
	virtual void setInputLayout(int _index) = 0;

	////////////////////////////////////
	//			Get Functions		  //
	////////////////////////////////////

	/*  get DX version of number from instance
		note: this should not be subject to override */
	virtual Version	getVersion();

	/*	Get the resolution */
	virtual glm::vec2 getResolution() = 0;

	virtual float getTotalVRAM();

	virtual float getMemBuffers();

	virtual float getMemShaders();

	virtual float getMemTextures();

	virtual float getMemBackBuffer();

	float memConsumption(float _size, UINT _width, UINT _height);

	float memConsumption(float _size, UINT _amount);

	/*	The answer to life, the universe and everything.
		In practice this is a useless test method */
	virtual int		getAnswer();

	virtual int getDefaultTexture() = 0;
	/*	Query the GPU VRAM attribute
		- this is a static attribute, so no need to
		call multiple times

		#returns
		total amount of VRAM*/
	virtual int getDedicatedVRAM() = 0;

	/*	Query the GPU for the amount of used VRAM
		- this is a variable attribute, query to
		look for changes in VRAM usage

		#returns
		total amount of used VRAM*/
	virtual int getUsedVRAM() = 0;

	virtual float totalAllocatedVRAM() = 0;

	/* get the image index of the seed image loaded for shaders to use

		returns the image index of the random seed image.*/
	virtual int getRandomSeedIndex() = 0;

	/*	Returns the depth resource view index*/
	virtual int getDepthResourceView() = 0;

	/*	Returns the shadow resource view index*/
	virtual int getShadowResourceView() = 0;

	/*	Returns the index value from the handles with name _handleName of type _handleType

		#_handleName the name of the variable to get the index from
		#_handleType the type of variable to get

		returns a index value to the value of type _handleType assigned to _handleName*/
	virtual int getHandleIndex(std::string _handleName, ResourceType _handleType) = 0;

	//////////////////////
	//		DRAW		//
	//////////////////////

	/* Draw a number of vertex points from the inputed vertex buffer

		#_vertexCount the number of vertexes to draw
		#_startVertexLocation the start location from where to start reading vertexes*/
	virtual void draw(UINT _vertexCount, UINT _startVertexLocation) = 0;

	/*	Draw vertexes from the vertex buffer
		only for use by points that is expanded by the geometry shader*/
	virtual void drawAuto() = 0;

	virtual void drawIndexed(UINT _indexCount, UINT _startIndexLocation, UINT _baseVertexLocation) = 0;

	/**/
	virtual void presentFinalScene() = 0;

	/*	Clears all render targets and all depth stencil views*/
	virtual void clearRenderTargets(glm::vec4 _clearColor) = 0;

	/*	Clear single rendertaget */
	virtual void clearRenderTargetSingle(UINT _renderTargetIndex, glm::vec4 _clearColor = glm::vec4(0, 0, 0, 1)) = 0;

	/**/
	virtual void clearDepthStencil(UINT _depthStencilIndex) = 0;

	/*	Shows the scene on the screen*/
	virtual void presentSwapChain() = 0;

	/*	Swaps the data in two buffers*/
	virtual void swapBuffers(int* _bufferOneIndex, int* _bufferTwoIndex) = 0;

	virtual void drawInstanced(UINT _vertexCount, UINT _instanceCount, UINT _startVertexLocation, UINT _startInstanceLocation) = 0;

	/**/
	virtual ID3D11DeviceContext*		getDeviceContext() = 0;
	virtual ID3D11Device*				getDevice() = 0;
	virtual ID3D11UnorderedAccessView*	getUAV(UINT _index) = 0;
	virtual ID3D11ShaderResourceView*	getSRV(UINT _index) = 0;

protected:
	/*	creates a constant buffer
		#_bytewidth is the byte size of the struct for the buffer
		#_registerIndex is the index, the cbuffer has in the shader.

		returns a bool indicating success or failure.*/
	virtual bool initCreateCBuffer(unsigned int _byteWidth, std::string _name) = 0;

private:

	Version version;

};