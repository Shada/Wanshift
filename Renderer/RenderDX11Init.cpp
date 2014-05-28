#include "RenderDX11.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <fileapi.h>
#include <glm\gtc\matrix_transform.hpp>
#include "ReaderINI.h"
#include "Utility_Render.h"

RenderDX11::RenderDX11()
	: RenderInterface(Version::DX11)
{
	sharedContainer = NULL;
	sharedContainer = new SharedContainer();
	swapChain = NULL;
	device = NULL;
	deviceContext = NULL;
}

RenderDX11::~RenderDX11()
{
	release();
}

void RenderDX11::release()
{
	SAFE_DELETE(sharedContainer);
	SAFE_RELEASE(device);
	SAFE_RELEASE(deviceContext);
	SAFE_RELEASE(swapChain);

	for(unsigned int i = 0; i < samplers.size(); i++)
		SAFE_RELEASE(samplers.at(i));

	for(unsigned int i = 0; i < rasterStates.size(); i++)
		SAFE_RELEASE(rasterStates.at(i));
}

bool RenderDX11::inititalizeDirectX(int _screenWidth, int _screenHeight, bool _windowed, HWND _hwnd)
{
	screenWidth		= _screenWidth;
	screenHeight	= _screenHeight;
	windowed		= _windowed;
	hWnd			= _hwnd;

	bool result = true;

	// Finds and sets the correct video card option
	result = initSetCorrectVideoCardDescription(screenWidth, screenHeight);
	if(!result)
		return false;

	// Creates the swapchain that will be used by the rendering
	result = initCreateSwapChainAndDevice(screenWidth, screenHeight, hWnd);
	if(!result)
		return false;

	// Creates the render target view to be used for the final scene
	// NO! NO! NO! this creates a fake buffer, everything is written to a texture
	// used by the render target tied to the actual backbuffer

	//ID3D11RenderTargetView *rtv = NULL;
	//renderTargetView = sharedContainer->addRenderTargetView(rtv);
	int fakeBuffer = createTexture2D((int)getResolution().x, (int)getResolution().y);

	renderTargetView = createRenderTargetView(fakeBuffer);
	int fbSRV = createShaderResourceView(fakeBuffer);
	sharedContainer->addHandleIndex("fakeBuffer", ResourceType::ShaderResourceView, fbSRV);
	sharedContainer->addHandleIndex("fakeBufferRenderTarget", ResourceType::RenderTargetView, renderTargetView);

	ID3D11RenderTargetView* theRealDeal = NULL;
	backBufferRenderTarget = sharedContainer->addRenderTargetView(theRealDeal);
	result = initCreateBackBufferRenderTargetView(backBufferRenderTarget);
	sharedContainer->addHandleIndex("backBufferRTV",ResourceType::RenderTargetView,backBufferRenderTarget);
	
	//renderTargetView = backBufferRenderTarget;
	if(!result)
		return false;

	// Creates the depth buffer/stencil/view
	result = initCreateDepthBuffer(screenWidth, screenHeight);
	if(!result)
		return false;

	// creates the views and textures used by the volume shading
	initCreateShadowResources(_screenWidth,_screenHeight);

	// Set the render target and the depth stencil
	setRenderTargetAndDepthStencil(1, sharedContainer->getRenderTargetView(renderTargetView), sharedContainer->getDepthStencilView(depthStencilView));


	// Sets the rasterizer state to solid and the culling to backface
	result = initCreateRasterState();
	if(!result)
		return false;

	// sets the devicecontexts viewport
	setViewport(screenWidth, screenHeight);


	CBWorldData					cbwd; //do
	CBViewData					cbvd; //not
	CBOnceData					cbod; //remove
	CBTextData					cbtd; //me.
	CBLightInfoData				cblid;
	CBTerrainInfo				cbti; //I
	CBEveryFrame				cbef; //am
	CBParticleStart				cbps; //required.
	CBParticleEnergy			cbpe;
	CBWorldAttributes			cbworldattributes; //I
	CBScatterAttributes			cbsa; //am
	CBSandStorm					cbss; //warning
	CBRainSimulationConstants	cbrsc;//you!
	CBRainConstants				cbrc;
	CBAnimationMatrices			cbam;
	CBWisp						cbw;
	FogSim						cdf;
	cbHeat						cbh;
	cbCloud						cbcl;
	CBBiomShift					cbbs;
	CBSpotLight					cbsl;

	//create the constant buffers
	if(!initCreateCBuffer(sizeof(cbworldattributes), "cbWorldAttribute"))			// atmosphere: world
		return false;
	if(!initCreateCBuffer(sizeof(cbwd), "cbWorld"))
		return false;
	if(!initCreateCBuffer(sizeof(cbvd), "cbView"))
		return false;
	if(!initCreateCBuffer(sizeof(cbod), "cbOnce"))
		return false;
	if(!initCreateCBuffer(sizeof(cbtd), "cbText"))
		return false;
	if(!initCreateCBuffer(sizeof(cblid), "cbLightInfo"))
		return false;
	if(!initCreateCBuffer(sizeof(cbti), "cbTerrain"))
		return false;
	if(!initCreateCBuffer(sizeof(cbef), "cbEveryframe"))
		return false;
	if(!initCreateCBuffer(sizeof(cbps), "cbParticleStart"))
		return false;
	if(!initCreateCBuffer(sizeof(cbpe), "cbParticleEnergy"))
		return false;
	if(!initCreateCBuffer(sizeof(cbsa), "cbScatterAttributes"))			// atmosphere: scatter
		return false;
	if(!initCreateCBuffer(sizeof(cbss), "cbSandStorm"))
		return false;
	if(!initCreateCBuffer(sizeof(cbrsc), "cbRainSimulationConstants"))	// rain : weather
		return false;
	if(!initCreateCBuffer(sizeof(cbrc), "cbRainConstants"))				// rain : misc
		return false;
	if(!initCreateCBuffer(sizeof(cdf), "cbFog"))			// rain : misc
		return false;
	if(!initCreateCBuffer(sizeof(cbh), "cbHeatHaze"))
		return false;
	if(!initCreateCBuffer(sizeof(cbcl), "cbCloud"))
		return false;
	if(!initCreateCBuffer(sizeof(cbbs), "cbShifting"))
		return false;
	if(!initCreateCBuffer(sizeof(cbam), "cbAnimationMatrices"))			// animations
		return false;
	if(!initCreateCBuffer(sizeof(cbw), "cbWisps"))
		return false;
	if(!initCreateCBuffer(sizeof(cbsl), "cbSpotLight"))
		return false;

	cbSandStormIndex = getHandleIndex("cbSandStorm",ResourceType::ConstantBuffer);

	initCreateTopologys();

	initCreateBlendStates();

	initCreateSamplers();

	defaultImageIndex = createShaderResourceViewFromFile("Textures/default.png");
	blackImageIndex = createShaderResourceViewFromFile("Textures/black.png");
	randomSeedIndex = createShaderResourceViewFromFile("Textures/seed.png");

	if (!initHandleTextures())
		return false;

	return true;
}

bool RenderDX11::initSetCorrectVideoCardDescription(int _screenWidth, int _screenHeight)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
	{
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card)
	result = factory->EnumAdapters(0, &adapter);
	if(FAILED(result))
	{
		return false;
	}

	// Enumerate the primary adapter output (monitor)
	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result))
	{
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor)
	result = adapterOutput->GetDisplayModeList(	DXGI_FORMAT_R8G8B8A8_UNORM, 
												DXGI_ENUM_MODES_INTERLACED,
												&numModes,
												NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(	DXGI_FORMAT_R8G8B8A8_UNORM,
												DXGI_ENUM_MODES_INTERLACED,
												&numModes,
												displayModeList);
	if(FAILED(result))
	{
		return false;
	}

	// Now go throug all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for(unsigned int i = 0; i < numModes; i++)
	{
		if(displayModeList[i].Width == (unsigned int)_screenWidth)
		{
			if(displayModeList[i].Height == (unsigned int)_screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description
	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0)
	{
		return false;
	}

	// Release the display mode list
	delete [] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	return true;
}

bool RenderDX11::initCreateSwapChainAndDevice(int _screenWidth, int _screenHeight, HWND _hwnd)
{
	HRESULT result = S_OK;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	
	// Initialize the swap chain description
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = _screenWidth;
	swapChainDesc.BufferDesc.Height = _screenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = _hwnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Start in windowed as standard
	swapChainDesc.Windowed = windowed;

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;
	
	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;

	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(	NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
											&featureLevel, 1, D3D11_SDK_VERSION,
											&swapChainDesc, &swapChain, &device,NULL, &deviceContext);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

bool RenderDX11::initCreateBackBufferRenderTargetView(int _renderTargetIndex)
{
	HRESULT result = S_OK;
	ID3D11Texture2D* backBufferPtr;

	// Get the pointer to the back buffer.
	result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	float tsize = getTextureMemSize(backBufferPtr);
	sharedContainer->logBackBufferSize(tsize);

	if(FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = device->CreateRenderTargetView(backBufferPtr, NULL, sharedContainer->getRenderTargetViews(_renderTargetIndex));
	if(FAILED(result))
	{
		return false;
	}
	sharedContainer->addHandleIndex("BackBufferRenderTargetView", ResourceType::RenderTargetView, _renderTargetIndex);

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;



	return true;
}

bool RenderDX11::initCreateDepthBuffer(int _screenWidth, int _screenHeight)
{
	HRESULT result = S_OK;

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStancilViewDesc;

	// Initilize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = _screenWidth;
	depthBufferDesc.Height = _screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	ID3D11Texture2D *tDepthStencilTexture = NULL;
	ID3D11DepthStencilState *tDepthStencilState = NULL;
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &tDepthStencilTexture);
	if(FAILED(result))
	{
		return false;
	}
	depthStencilTexture = sharedContainer->addTexture2D(tDepthStencilTexture);

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = device->CreateDepthStencilState(&depthStencilDesc, &tDepthStencilState);
	if(FAILED(result))
	{
		return false;
	}
	depthStencilState = sharedContainer->addDepthStencilState(tDepthStencilState);


	// Stencil state for shadowing
	depthStencilDesc.DepthEnable	= true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc		= D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable		= true;
	depthStencilDesc.StencilReadMask	= 0xFF;
	depthStencilDesc.StencilWriteMask	= 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_DECR;
	depthStencilDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp= D3D11_STENCIL_OP_INCR;
	depthStencilDesc.BackFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc		= D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	result = device->CreateDepthStencilState(&depthStencilDesc, &tDepthStencilState);
	if(FAILED(result))
	{
		return false;
	}
	depthStencilState = sharedContainer->addDepthStencilState(tDepthStencilState);

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable	= true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc		= D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable		= true;
	depthStencilDesc.StencilReadMask	= 0xFF;
	depthStencilDesc.StencilWriteMask	= 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp= D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc		= D3D11_COMPARISON_ALWAYS;

	result = device->CreateDepthStencilState(&depthStencilDesc, &tDepthStencilState);
	if(FAILED(result))
	{
		return false;
	}
	depthStencilState = sharedContainer->addDepthStencilState(tDepthStencilState);

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable	= false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc		= D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable		= false;
	depthStencilDesc.StencilReadMask	= 0xFF;
	depthStencilDesc.StencilWriteMask	= 0xFF;

	result = device->CreateDepthStencilState(&depthStencilDesc, &tDepthStencilState);
	if(FAILED(result))
	{
		return false;
	}
	depthStencilState = sharedContainer->addDepthStencilState(tDepthStencilState);

	// stencil enabled, but no depth tested
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = device->CreateDepthStencilState(&depthStencilDesc, &tDepthStencilState);
	if(FAILED(result))
	{
		return false;
	}
	depthStencilState = sharedContainer->addDepthStencilState(tDepthStencilState);


	// Initialize the depth stencil view.
	ZeroMemory(&depthStancilViewDesc, sizeof(depthStancilViewDesc));

	// Set up the depth stencil view description.
	depthStancilViewDesc.Format				= DXGI_FORMAT_D32_FLOAT;
	depthStancilViewDesc.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStancilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	ID3D11DepthStencilView *tDepthStencilView = NULL;
	result = device->CreateDepthStencilView(sharedContainer->getTexture2D(depthStencilTexture), &depthStancilViewDesc, &tDepthStencilView);
	if(FAILED(result))
	{
		return false;
	}
	depthStencilView = sharedContainer->addDepthStencilView(tDepthStencilView);

	sharedContainer->addHandleIndex("DepthStencilView", ResourceType::DepthStencilView, depthStencilView);


	ID3D11ShaderResourceView *srv = NULL;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc,sizeof(srvDesc));
    srvDesc.Format      = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension    = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels   = 1;
    result = device->CreateShaderResourceView(	sharedContainer->getTexture2D(depthStencilTexture),
												&srvDesc,
												&srv );
	 if( FAILED(result) )
	 {
	  MessageBox(NULL, "Error creating depth stencil shader resource!", "RenderDX11 Error", S_OK);
	  return false;
	 }

	 depthShaderResource = sharedContainer->addShaderResourceView(srv);
	
	 sharedContainer->addHandleIndex("depthshaderresource",ResourceType::ShaderResourceView,depthShaderResource);

	return true;
}

bool RenderDX11::initCreateRasterState()
{
	HRESULT result = S_OK;

	D3D11_RASTERIZER_DESC rasterDesc;
	ID3D11RasterizerState *rasterState = NULL;

	// Setup the raster description which will determinatie how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = device->CreateRasterizerState(&rasterDesc, &rasterState);
	if(FAILED(result))
	{
		return false;
	}

	rasterStates.push_back(rasterState);	// index: 0

	rasterDesc.FrontCounterClockwise = true;
	result = device->CreateRasterizerState(&rasterDesc, &rasterState);
	if(FAILED(result))
	{
		return false;
	}
	rasterStates.push_back(rasterState);	// index: 1
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	result = device->CreateRasterizerState(&rasterDesc, &rasterState);
	if(FAILED(result))
	{
		return false;
	}
	rasterStates.push_back(rasterState);	// index: 2
	// Now set the rasterizer state
	deviceContext->RSSetState(rasterState);

	//rasterDesc.CullMode = D3D11_CULL_FRONT;	// overwrite?
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterStates.push_back(rasterState);	// index: 3

	rasterDesc.DepthClipEnable = false;
	rasterStates.push_back(rasterState);	// index : 4

	
	return true;
}

bool RenderDX11::resetWindowMode(bool _windowed, int _screenWidth, int _screenHeight)
{
	bool result = true;
	windowed = _windowed;

	result = initSetCorrectVideoCardDescription(_screenWidth, _screenHeight);
	if(!result)
		return false;
	result = initCreateSwapChainAndDevice(_screenWidth, _screenHeight, hWnd);
	if(!result)
		return false;
	result = initCreateDepthBuffer(_screenWidth, _screenHeight);
	if(!result)
		return false;
	setRenderTargetAndDepthStencil(1,sharedContainer->getRenderTargetView(renderTargetView),sharedContainer->getDepthStencilView(depthStencilView));
	result = initCreateRasterState();
	if(!result)
		return false;
	setViewport(_screenWidth, _screenHeight);

	return true;
}

void RenderDX11::swapBuffers(int* _bufferOneIndex, int* _bufferTwoIndex)
{
	int temp = *_bufferOneIndex;
	*_bufferOneIndex = *_bufferTwoIndex;
	*_bufferTwoIndex = temp;
}

bool RenderDX11::initCreateSamplers()
{
	HRESULT result;
	ID3D11SamplerState *samplerState;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc));
	sampDesc.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU  = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV  = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW  = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD   = -D3D11_FLOAT32_MAX;
	sampDesc.MaxLOD   = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState( &sampDesc, &samplerState);
	if(FAILED(result))
		return false;

	samplers.push_back(samplerState);

	sampDesc.Filter  = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	result = device->CreateSamplerState( &sampDesc, &samplerState);
	if(FAILED(result))
		return false;

	samplers.push_back(samplerState);

	sampDesc.Filter   = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU  = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV  = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW  = D3D11_TEXTURE_ADDRESS_WRAP;

	result = device->CreateSamplerState( &sampDesc, &samplerState);
	if(FAILED(result))
		return false;

	samplers.push_back(samplerState);

	sampDesc.Filter   = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	result = device->CreateSamplerState( &sampDesc, &samplerState);
	if(FAILED(result))
		return false;

	samplers.push_back(samplerState);

	sampDesc.Filter		= D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU	= D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV	= D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	
	result = device->CreateSamplerState( &sampDesc, &samplerState);

	if( FAILED(result) )
		return false;

	samplers.push_back(samplerState);

	sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&sampDesc, &samplerState);

	if (FAILED(result))
		return false;

	samplers.push_back(samplerState);

	setSamplers();

	return true;
}

void RenderDX11::initCreateTopologys()
{
	topologys.push_back(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	topologys.push_back(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	topologys.push_back(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	topologys.push_back(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	topologys.push_back(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
	topologys.push_back(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ);
}

bool RenderDX11::initCreateBlendStates()
{
	HRESULT result = S_OK;

	ID3D11BlendState *blendState;
	
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc,sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable      = FALSE;
	blendDesc.IndependentBlendEnable     = FALSE;
	blendDesc.RenderTarget[0].BlendEnable    = FALSE;
	blendDesc.RenderTarget[0].SrcBlend     = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlend     = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOp     = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha    = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha   = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha    = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask  = 0x0f;

	result = device->CreateBlendState(&blendDesc,&blendState);
	if(FAILED(result))
		return false;

	sharedContainer->addBlendState(blendState);	// index: 0

	blendDesc.RenderTarget[0].BlendEnable				= TRUE;
	blendDesc.RenderTarget[0].SrcBlend					= D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend					= D3D11_BLEND_INV_SRC_ALPHA;

	result = device->CreateBlendState(&blendDesc,&blendState);
	if(FAILED(result))
		return false;
	sharedContainer->addBlendState(blendState);	// index: 1


	//blendDesc.AlphaToCoverageEnable      = TRUE;
	blendDesc.RenderTarget[0].BlendEnable				= TRUE;
	blendDesc.RenderTarget[0].SrcBlend					= D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend					= D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp					= D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha				= D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlendAlpha			= D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha				= D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask		= 0x0f;

	result = device->CreateBlendState(&blendDesc,&blendState);
	if(FAILED(result))
		return false;

	sharedContainer->addBlendState(blendState);	// index: 2

	blendDesc.RenderTarget[0].BlendEnable				= TRUE;
	blendDesc.RenderTarget[0].SrcBlend					= D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend					= D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp					= D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha				= D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlendAlpha			= D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOpAlpha				= D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask		= D3D11_COLOR_WRITE_ENABLE_ALL;	

	result = device->CreateBlendState(&blendDesc,&blendState);	// index: 3
	if(FAILED(result))
		return false;

	sharedContainer->addBlendState(blendState);

	//blendDesc.AlphaToCoverageEnable      = TRUE;
	blendDesc.RenderTarget[0].BlendEnable				= TRUE;
	blendDesc.RenderTarget[0].SrcBlend					= D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend					= D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOp					= D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha				= D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlendAlpha			= D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha				= D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask		= 0x0f;

	result = device->CreateBlendState(&blendDesc,&blendState);
	if(FAILED(result))
		return false;

	sharedContainer->addBlendState(blendState);

	blendDesc.RenderTarget[0].BlendEnable				= TRUE;
	blendDesc.RenderTarget[0].SrcBlend					= D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend					= D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOp					= D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha				= D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlendAlpha			= D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha				= D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask		= 0x0f;

	result = device->CreateBlendState(&blendDesc,&blendState);
	if(FAILED(result))
		return false;

	sharedContainer->addBlendState(blendState);

	return true;
}

void RenderDX11::initCreateShadowResources(int _screenWidth, int _screenHeight)
{
	HRESULT result = S_OK;

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStancilViewDesc;

	// Initilize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = _screenWidth;
	depthBufferDesc.Height = _screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	ID3D11Texture2D *tDepthStencilTexture = NULL;
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &tDepthStencilTexture);
	if(FAILED(result))
	{
		MessageBox(NULL, "Failed to create shadow texture", "Shadow Error", S_OK);
	}
	shadowTexture = sharedContainer->addTexture2D(tDepthStencilTexture);


	// Initialize the depth stencil view.
	ZeroMemory(&depthStancilViewDesc, sizeof(depthStancilViewDesc));

	// Set up the depth stencil view description.
	depthStancilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStancilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStancilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	ID3D11DepthStencilView *tDepthStencilView = NULL;
	result = device->CreateDepthStencilView(sharedContainer->getTexture2D(depthStencilTexture), &depthStancilViewDesc, &tDepthStencilView);
	if(FAILED(result))
	{
		MessageBox(NULL, "Failed to create shadow stencil view", "Shadow Error", S_OK);
	}
	shadowState = sharedContainer->addDepthStencilView(tDepthStencilView);

	ID3D11ShaderResourceView *srv = NULL;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc,sizeof(srvDesc));
    srvDesc.Format      = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension    = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels   = 1;
    result = device->CreateShaderResourceView(	sharedContainer->getTexture2D(depthStencilTexture),
												&srvDesc,
												&srv );
	if( FAILED(result) )
	{
		MessageBox(NULL, "Error creating shadow stencil shader resource!", "GraphicsCore Error", S_OK);
	}

	shadowView = sharedContainer->addShaderResourceView(srv);

	 /**/
	finalSceneShaderVS = createShader("quadTextureVS", "vs_5_0", (int)LayoutTypes::LAYOUT_FINAL);
	finalSceneShaderPS = createShader("quadTexturePS", "ps_5_0", (int)LayoutTypes::LAYOUT_FINAL);

}

bool RenderDX11::initCreateCBuffer(unsigned int _byteWidth, std::string _name)
{
	HRESULT hr;
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = _byteWidth;
	ID3D11Buffer *buffer = NULL;
	hr = device->CreateBuffer(&bd, NULL, &buffer);
	if (FAILED(hr))
	{
		std::cout << "Failed to create constant buffer " + _name + "\n";
		return false;
	}
	/*deviceContext->VSSetConstantBuffers(_registerIndex, 1, &buffer);
	deviceContext->DSSetConstantBuffers(_registerIndex, 1, &buffer);
	deviceContext->GSSetConstantBuffers(_registerIndex, 1, &buffer);
	deviceContext->PSSetConstantBuffers(_registerIndex, 1, &buffer);
	deviceContext->HSSetConstantBuffers(_registerIndex, 1, &buffer);
	deviceContext->CSSetConstantBuffers(_registerIndex, 1, &buffer);*/
	UINT index = sharedContainer->addCBuffer(buffer);
	addHandle(_name,ResourceType::ConstantBuffer,index);
	return true;
}

bool RenderDX11::initHandleTextures()
{
	ReaderINI reader;
	bool valid = true;
	valid = reader.generateIniData();
	if (!valid)
	{
		return false;
	}


	RootINI root;
	root = reader.getInitData();

	std::string Header = "Textures";

	Attributes attributes;

	valid = root.getAllNodeAttributes(Header, attributes);
	if (!valid)
	{
		return false;
	}
	
	Attributes::iterator it = attributes.begin();

	int index;

	while (it != attributes.end())
	{
		index = createShaderResourceViewFromFile(it->second);

		sharedContainer->addHandleIndex(it->first,ResourceType::ShaderResourceView,index);

		it++;
	}
	return true;
}
