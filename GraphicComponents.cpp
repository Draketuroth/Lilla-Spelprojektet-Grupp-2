
#include "GraphicComponents.h"

GraphicComponents::GraphicComponents() {

	gSwapChain = nullptr;
	gDevice = nullptr;	
	gDeviceContext = nullptr;	
	gBackbufferRTV = nullptr;	

	gVertexLayout = nullptr;
	gVertexShader = nullptr;
	gPixelShader = nullptr;
	gGeometryShader = nullptr;

	gIceEnemyVertexLayout = nullptr;
	gIceEnemyVertexShader = nullptr;
	gIceEnemyPixelShader = nullptr;

	gLavaVertexLayout = nullptr; 
	gLavaVertexShader = nullptr; 
	gLavaPixelShader = nullptr; 
	gLavaGeometryShader = nullptr; 

	gProjectileVertexLayout = nullptr;
	gProjectileVertexShader = nullptr;
	gProjectilePixelShader = nullptr;
}

GraphicComponents::~GraphicComponents() {

}

void GraphicComponents::ReleaseAll() {

	SAFE_RELEASE(gSwapChain);
	SAFE_RELEASE(gDevice);
	SAFE_RELEASE(gDeviceContext);
	SAFE_RELEASE(gBackbufferRTV);

	SAFE_RELEASE(gVertexLayout);
	SAFE_RELEASE(gVertexShader);
	SAFE_RELEASE(gPixelShader);
	SAFE_RELEASE(gGeometryShader);

	SAFE_RELEASE(gPlatformLayout);
	SAFE_RELEASE(gPlatformVertexShader);
	SAFE_RELEASE(gPlatformPixelShader);
	SAFE_RELEASE(gPlatformGeometryShader);

	SAFE_RELEASE(gFortressLayout);
	SAFE_RELEASE(gFortressVertexShader);
	SAFE_RELEASE(gFortressPixelShader);

	SAFE_RELEASE(depthStencil);
	SAFE_RELEASE(depthView);
	SAFE_RELEASE(depthState);

	SAFE_RELEASE(gLavaVertexLayout); 
	SAFE_RELEASE(gLavaVertexShader); 
	SAFE_RELEASE(gLavaPixelShader); 
	SAFE_RELEASE(gLavaGeometryShader); 

	SAFE_RELEASE(gMenuLayout);
	SAFE_RELEASE(gMenuVertex);
	SAFE_RELEASE(gMenuPixel);

	SAFE_RELEASE(gIceEnemyPixelShader);
	SAFE_RELEASE(gIceEnemyVertexLayout);
	SAFE_RELEASE(gIceEnemyVertexShader);

	SAFE_RELEASE(gLavaEnemyPixelShader);
	SAFE_RELEASE(gLavaEnemyVertexLayout);
	SAFE_RELEASE(gLavaEnemyVertexShader);

	SAFE_RELEASE(gDebugVertexLayout);
	SAFE_RELEASE(gDebugPixelShader);
	SAFE_RELEASE(gDebugVertexShader);

	SAFE_RELEASE(gShadowVertexLayout);
	SAFE_RELEASE(gShadowVertexLayout);
	SAFE_RELEASE(gHUDVertexShader);
	SAFE_RELEASE(gHUDVertexLayout);
	SAFE_RELEASE(gHUDPixelShader);


}

bool GraphicComponents::InitalizeDirect3DContext(HWND &windowHandle) {

	if (!CreateSwapChainAndDevice(windowHandle)) 
	{
		return false;
	}
	if (!CreateDepthStencil())
	{
		return false;
	}
	if (!CreateRenderTargetView()) 
	{
		return false;
	}

	SetViewport();

	if (!CreateStandardShaders()) 
	{
		return false;
	}
	if (!CreatePlatformShaders()) 
	{
		return false;
	}

	if (!CreateFortressShader()) {

		return false;
	}

	if (!CreateMenuShaders())
	{
		return false;
	}

	if (!CreateIceEnemyShaders())
	{
		return false;
	}

	if (!CreateLavaEnemyShaders())
	{
		return false;
	}

	if (!CreateLavaShaders())
	{
		return false; 
	}

	if (!CreateDebugShaders())
	{
		return false;
	}
	if (!CreateProjectileShaders())
	{
		return false;
	}
	if (!CreateShadowShaders())
	{
		return false;
	}
	if (!CreateHUDShaders())
	{
		return false;
	}

	return true;
}

bool GraphicComponents::CreateSwapChainAndDevice(HWND &windowHandle) {

	// CreateSwapChainAndDevice function is responsible for:
	// - giving us the back buffer from the created swap chain
	// - create a render target view of it to use as the target for rendering
	// - We need to make sure that we create the swap chain before we ever call on our CreateDepthBuffer function

	// A description of our swap chain will be defined with different resources and settings
	// Create the swap chain struct to hold all required information
	DXGI_SWAP_CHAIN_DESC chainDescription;

	// Just as a safety routine, we clear out the struct of any data to prepare it 
	ZeroMemory(&chainDescription, sizeof(DXGI_SWAP_CHAIN_DESC));

	// The most important settings are filled in below
	chainDescription.BufferCount = 1;									// There is only one back buffer
	chainDescription.BufferDesc.Height = HEIGHT;
	chainDescription.BufferDesc.Width = WIDTH;
	chainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// We are using 32-bit color
	chainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// Defines use of swap chain. It now uses the surface or resource as an output render target.
	chainDescription.OutputWindow = windowHandle;						// The window to be used defined in "Global Window Variables"
	chainDescription.SampleDesc.Count = 1;
	chainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// How many multisamples to be used and default seems to be 4
	chainDescription.Windowed = LaunchInWindowedMode;					// Specify whether to launch application window in windowed or fullscreen mode		

																		// A trick here is that we can take advantage of a debug layer, and therefore Debug should be defined as a build option

	UINT createDeviceFlags = 0;
#if defined (DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL supportedFeatureLevel;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(

		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&chainDescription,
		&gSwapChain,
		&gDevice,
		&supportedFeatureLevel,
		&gDeviceContext);

	if (FAILED(hr)) {

		cout << "Device Creation Error: Device, DeviceContext and Swap Chain could not be created" << endl;
		return false;
	}

	if (supportedFeatureLevel < D3D_FEATURE_LEVEL_11_0) {

		cout << "DirectX Error: DirectX 11 is not supported" << endl;
		return false;
	}

	return true;
}

bool GraphicComponents::CreateDepthStencil()
{
	HRESULT hr;

	// Depth Texture
	//----------------------------------------------------------------------------------------------------------------------------------//

	// THe depth buffer texture is used to store the distance of each fragment to the camera

	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = WIDTH;
	descDepth.Height = HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	hr = gDevice->CreateTexture2D(&descDepth, nullptr, &depthStencil);

	if (FAILED(hr)) {

		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------------------//


	// Depth View Description
	//----------------------------------------------------------------------------------------------------------------------------------//

	// Create depth view description. A depth-stencil-view interface process a texture resource during the depth-stencil testing.
	D3D11_DEPTH_STENCIL_VIEW_DESC dViewDesc;
	dViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dViewDesc.Texture2D.MipSlice = 0;
	dViewDesc.Flags = 0;

	//Create depth stencil view
	hr = gDevice->CreateDepthStencilView(depthStencil, &dViewDesc, &depthView);

	if (FAILED(hr)) {

		return false;
	}

	// Depth State Description
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_DEPTH_STENCIL_DESC stencilDesc;

	// Depth test
	stencilDesc.DepthEnable = true;
	stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test
	stencilDesc.StencilEnable = true;
	stencilDesc.StencilReadMask = 0xFF;
	stencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if the pixel is facing forward

	stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if the pixel is facing backward

	stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create Depth State
	hr = gDevice->CreateDepthStencilState(&stencilDesc, &depthState);

	if (FAILED(hr)) {

		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------------------//

	return true;

}

bool GraphicComponents::CreateRenderTargetView(){

	// CreateRenderTargetView function is responsible for:
	// - Giving us the back buffer from the created swap chain
	// - Create a render target view of it to use as the target for rendering

	ID3D11Texture2D* backBuffer;

	HRESULT hr = gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

	if (FAILED(hr)) {

		// This error will arise when the back buffer could not be retrieved
		cout << "Buffer Error: Back buffer could not be retrieved" << endl;
		return false;
	}

	hr = gDevice->CreateRenderTargetView(backBuffer, nullptr, &gBackbufferRTV);

	if (FAILED(hr)) {

		// This error is given because the render target view creation failed
		cout << "Render Target View Error: Render target view could not be created" << endl;
		return false;
	}

	backBuffer->Release();
	
	gDeviceContext->OMSetDepthStencilState(depthState, 1);
	gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, depthView);

	return true;
}

void GraphicComponents::SetViewport() {

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;	// This is the top left corner's x coordinate in pixels from the window's top left corner
	viewport.TopLeftY = 0.0f;	// This is the top left corner's y coordinate in pixels from the window's top left corner
	viewport.MinDepth = 0.0f;	// This is the minimum depth value used by DirectX
	viewport.MaxDepth = 1.0f;	// This is the maximum depth value used by Direct3D
	viewport.Width = WIDTH;		// This viewport will use a width of user defined pixels. We are using the already defined macro values
	viewport.Height = HEIGHT;	// This viewport will use a height of user defined pixels. We are using the already defined macro values

	gDeviceContext->RSSetViewports(1, &viewport);	// Sets the viewport to be used
}

bool GraphicComponents::CreateStandardShaders() {

	HRESULT hr;

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\StandardShaders\\Vertex.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}

		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gVertexShader);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] = {

		{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT",			0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES",	0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	int inputLayoutSize = sizeof(vertexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gVertexLayout);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();


	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\StandardShaders\\Fragment.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&psBlob,
		&psErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob) {

			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gPixelShader);

	if (FAILED(hr)) {

		cout << "Pixel Shader Error: Pixel Shader could not be created" << endl;
		return false;
	}

	psBlob->Release();

	ID3DBlob* gsBlob = nullptr;
	ID3DBlob* gsErrorBlob = nullptr;
	hr = D3DCompileFromFile(
		L"Shaders\\StandardShaders\\Geometry.hlsl",
		nullptr,
		nullptr,
		"GS_main",
		"gs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&gsBlob,
		&gsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Geometry Shader Error: Geometry Shader could not be compiled or loaded from file" << endl;

		if (gsErrorBlob) {

			OutputDebugStringA((char*)gsBlob->GetBufferPointer());
			gsErrorBlob->Release();
		}

	}

	hr = gDevice->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &gGeometryShader);

	if (FAILED(hr)) {

		cout << "Geometry Shader Error: Geometry Shader could not be created" << endl;
		return false;
	}

	gsBlob->Release();

	return true;
}

bool GraphicComponents::CreatePlatformShaders() {

	HRESULT hr;

	ID3DBlob* vsBlob = nullptr; // lvl 1 blue slime
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\CubeShaders\\CubeVertex.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Cube Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}

		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gPlatformVertexShader);

	if (FAILED(hr)) {

		cout << "Cube Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] = {

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SV_InstanceID", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};


	int inputLayoutSize = sizeof(vertexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gPlatformLayout);

	if (FAILED(hr)) {

		cout << "Cube Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();


	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\CubeShaders\\CubeFragment.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG,
		0,
		&psBlob,
		&psErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Cube Fragment Shader Error: Fragment Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob) {

			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gPlatformPixelShader);

	if (FAILED(hr)) {

		cout << "Cube Pixel Shader Error: Pixel Shader could not be created" << endl;
		return false;
	}

	psBlob->Release();

	ID3DBlob* gsBlob = nullptr;
	ID3DBlob* gsErrorBlob = nullptr;
	hr = D3DCompileFromFile(
		L"Shaders\\CubeShaders\\CubeGeometry.hlsl",
		nullptr,
		nullptr,
		"GS_main",
		"gs_5_0",
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG,
		0,
		&gsBlob,
		&gsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Geometry Shader Error: Geometry Shader could not be compiled or loaded from file" << endl;

		if (gsErrorBlob) {

			OutputDebugStringA((char*)gsBlob->GetBufferPointer());
			gsErrorBlob->Release();
		}

	}

	hr = gDevice->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &gPlatformGeometryShader);

	if (FAILED(hr)) {

		cout << "Cube Geometry Shader Error: Geometry Shader could not be created" << endl;
		return false;
	}

	gsBlob->Release();

	return true;
}

bool GraphicComponents::CreateFortressShader() {

	HRESULT hr;

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\FortressShaders\\FortressVertex.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Fortress Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}

		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gFortressVertexShader);

	if (FAILED(hr)) {

		cout << "Fortress Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] = {

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	int inputLayoutSize = sizeof(vertexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gFortressLayout);

	if (FAILED(hr)) {

		cout << "Fortress Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();


	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\FortressShaders\\FortressFragment.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG,
		0,
		&psBlob,
		&psErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Fortress Fragment Shader Error: Fragment Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob) {

			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gFortressPixelShader);

	if (FAILED(hr)) {

		cout << "Fortress Pixel Shader Error: Pixel Shader could not be created" << endl;
		return false;
	}

	psBlob->Release();

	return true;
}

bool GraphicComponents::CreateLavaShaders()
{
	HRESULT hr; 

	ID3DBlob* vsBlob = nullptr; 
	ID3DBlob* vsErrorBlob = nullptr; 

	hr = D3DCompileFromFile(
		L"Shaders\\LavaShaders\\VertexShaderLava.hlsl",
		nullptr, 
		nullptr, 
		"VS_main", 
		"vs_5_0", 
		D3DCOMPILE_DEBUG,
		0, 
		&vsBlob, 
		&vsErrorBlob
	);

	if (FAILED(hr)) {
		cout << "Vertex shader Lava Error: Vertex shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {
			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer()); 
			vsErrorBlob->Release(); 
		}

		return false; 
	}

	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gLavaVertexShader); 

	if (FAILED(hr)) {
		cout << "Vertex Shader Lava Error: Vertex Shader could not be created" << endl; 
		return false; 
	}

	D3D11_INPUT_ELEMENT_DESC vetrexInputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	int inputLayoutSize = sizeof(vetrexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	hr = gDevice->CreateInputLayout(vetrexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gLavaVertexLayout); 

	if (FAILED(hr)) {
		cout << "Vertex Shader Lava Error: Shader Input Layout could not be created" << endl; 
	}

	vsBlob->Release(); 

	ID3DBlob* psBlob = nullptr; 
	ID3DBlob* psErrorBlob = nullptr; 

	hr = D3DCompileFromFile(
		L"Shaders\\LavaShaders\\FragmentShaderLava.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&psBlob,
		&psErrorBlob
	); 

	if (FAILED(hr)) {
		cout << "Fragment shader Lava Error: Fragment Shader could not be compiled or loaded from file" << endl; 

		if (psErrorBlob)
		{
			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer()); 
			psErrorBlob->Release();
		}

		return false; 
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gLavaPixelShader); 

	if (FAILED(hr)) {
		cout << "Pixel Shader Lava Error: Pixel Shader could not be created" << endl; 
		return false; 
	}

	psBlob->Release(); 

	ID3DBlob* gsBlob = nullptr; 
	ID3DBlob* gsErrorBlob = nullptr; 

	hr = D3DCompileFromFile(
		L"Shaders\\LavaShaders\\GeometryShaderLava.hlsl",
		nullptr,
		nullptr,
		"GS_main",
		"gs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&gsBlob,
		&gsErrorBlob
	); 

	if (FAILED(hr))
	{
		cout << "Geometry Shader Lava Error: Geometry Shader could not be compiled or loaded from file" << endl;
		if (gsErrorBlob) {
			OutputDebugStringA((char*)gsBlob->GetBufferPointer()); 
			gsErrorBlob->Release();
		}

	}

	hr = gDevice->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &gLavaGeometryShader); 

	if (FAILED(hr)) {
		cout << "Geometry Shader Lava Error: Geometry Shader could not be created" << endl; 
		return false; 
	}

	gsBlob->Release(); 

	return true; 


}

bool GraphicComponents::CreateProjectileShaders()
{
	HRESULT hr;

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\ProjectileShaders\\ProjectileVertex.hlsl",
		nullptr,
		nullptr,
		"VS_Main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {
		cout << "Vertex shader Projectile Error: Vertex shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {
			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gProjectileVertexShader);

	if (FAILED(hr)) {
		cout << "Vertex Shader Projectile Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vetrexInputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SV_InstanceID", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	int inputLayoutSize = sizeof(vetrexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	hr = gDevice->CreateInputLayout(vetrexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gProjectileVertexLayout);

	if (FAILED(hr)) {
		cout << "Vertex Shader Projectile Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();

	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\ProjectileShaders\\ProjectileFragment.hlsl",
		nullptr,
		nullptr,
		"PS_Main",
		"ps_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&psBlob,
		&psErrorBlob
	);

	if (FAILED(hr)) {
		cout << "Fragment shader Projectile Error: Fragment Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob)
		{
			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gProjectilePixelShader);

	if (FAILED(hr)) {
		cout << "Pixel Shader Projectile Error: Pixel Shader could not be created" << endl;
		return false;
	}

	psBlob->Release();

	return true;
}


bool GraphicComponents::CreateMenuShaders()
{
	HRESULT hr;

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\MenuShaders\\MenuVertex.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}
		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gMenuVertex);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] = {

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	int inputLayoutSize = sizeof(vertexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gMenuLayout);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();

	//Pixel shader

	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\MenuShaders\\MenuFragment.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&psBlob,
		&psErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Pixel Shader Error: Pixel Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob) {

			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gMenuPixel);

	if (FAILED(hr)) {

		cout << "Pixel Shader Error: Pixel Shader could not be created" << endl;
		return false;
	}

	psBlob->Release();

	return true;
}

bool GraphicComponents::CreateIceEnemyShaders()
{
	HRESULT hr;

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\EnemyShaders\\EnemyIceVertexShader.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Ice Enemy Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}
		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gIceEnemyVertexShader);

	if (FAILED(hr)) {

		cout << "Ice Enemy Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] = {

		{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT",			0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES",	0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SV_InstanceID", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	int inputLayoutSize = sizeof(vertexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gIceEnemyVertexLayout);

	if (FAILED(hr)) {

		cout << "Ice Enemy Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();

	//Pixel shader

	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\EnemyShaders\\EnemyIcePixelShader.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&psBlob,
		&psErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Ice Enemy Pixel Shader Error: Pixel Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob) {

			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gIceEnemyPixelShader);

	if (FAILED(hr)) {

		cout << "Ice Enemy Pixel Shader Error: Pixel Shader could not be created" << endl;
		return false;
	}

	psBlob->Release();

	return true;
}

bool GraphicComponents::CreateLavaEnemyShaders() {

	HRESULT hr;

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\EnemyShaders\\EnemyLavaVertexShader.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Lava Enemy Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}
		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gLavaEnemyVertexShader);

	if (FAILED(hr)) {

		cout << "Lava Enemy Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] = {

		{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT",			0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES",	0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SV_InstanceID", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	int inputLayoutSize = sizeof(vertexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gLavaEnemyVertexLayout);

	if (FAILED(hr)) {

		cout << "Lava Enemy Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();

	//Pixel shader

	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\EnemyShaders\\EnemyLavaPixelShader.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&psBlob,
		&psErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Lava Enemy Pixel Shader Error: Pixel Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob) {

			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gLavaEnemyPixelShader);

	if (FAILED(hr)) {

		cout << "Lava Enemy Pixel Shader Error: Pixel Shader could not be created" << endl;
		return false;
	}

	psBlob->Release();

	return true;
}

bool GraphicComponents::CreateDebugShaders()
{
	HRESULT hr;

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\DebugShaders\\DebugVertex.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Debug Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}
		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gDebugVertexShader);

	if (FAILED(hr)) {

		cout << "Debug Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] = {

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SV_InstanceID", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 }

	};

	int inputLayoutSize = sizeof(vertexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gDebugVertexLayout);

	if (FAILED(hr)) {

		cout << "Debug Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();

	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\DebugShaders\\DebugFragment.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&psBlob,
		&psErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Debug Pixel Shader Error: Pixel Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob) {

			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gDebugPixelShader);

	if (FAILED(hr)) {

		cout << "Debug Pixel Shader Error: Pixel Shader could not be created" << endl;
		return false;
	}

	psBlob->Release();


	return true;
}

bool GraphicComponents::CreateShadowShaders()
{
	HRESULT hr;
	//Vertex shader for shadowmapping (main character)-------------------------------------------------------------------//
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\ShadowShaders\\ShadowVertex.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
		//	vsErrorBlob->Release();
		}
		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gShadowVertexShader);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] = {

		{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT",			0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES",	0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	int inputLayoutSize = sizeof(vertexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gShadowVertexLayout);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}
	//------------------------------------------------------------------------------------------------------------------------------//
	//vsBlob->Release();

	//Vertex shader for shadow mapping (platforms)---------------------------------------------------------------------------------//
	
	vsBlob = nullptr;
	vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\ShadowShaders\\ShadowPlatformVertex.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}
		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gShadowPlatformVertex);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc1[] = {

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SV_InstanceID", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	int inputLayoutSize1 = sizeof(vertexInputDesc1) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc1, inputLayoutSize1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gShadowPlatformLayout);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	//vsBlob->Release();
	//------------------------------------------------------------------------------------------------------------------------------//

	//Shadow map Lava enemy---------------------------------------------------------------------------------------------------------//
	

	vsBlob = nullptr;
	vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\ShadowShaders\\ShadowLavaVertex.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Shadow Lava Enemy Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}
		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gShadowLavaVertex);

	if (FAILED(hr)) {

		cout << "Shadow Lava Enemy Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc2[] = {

		{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT",			0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES",	0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SV_InstanceID", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	int inputLayoutSize2 = sizeof(vertexInputDesc2) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc2, inputLayoutSize2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gShadowLayaLayout);

	if (FAILED(hr)) {

		cout << "Shadow Lava Enemy Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}
	//vsBlob->Release();


	//Ice enemy shader-----------------------------------------------------------------------------------------------------------------//
	vsBlob = nullptr;
	vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\ShadowShaders\\ShadowIceVertex.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Shadow Ice Enemy Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}
		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gShadowIceVertex);

	if (FAILED(hr)) {

		cout << "Shadow Ice Enemy Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc3[] = {

		{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT",			0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES",	0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SV_InstanceID", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	int inputLayoutSize3 = sizeof(vertexInputDesc3) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc3, inputLayoutSize3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gShadowIceLayout);

	if (FAILED(hr)) {

		cout << "Shadow Lava Enemy Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}
	vsBlob->Release();


	return true;
}

bool GraphicComponents::CreateHUDShaders()
{
	HRESULT hr;

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\HUDShaders\\HUDVertexShader.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "HUD Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}
		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gHUDVertexShader);

	if (FAILED(hr)) {

		cout << "HUD Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] = {

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		/*{ "SV_InstanceID", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 }*/

	};

	int inputLayoutSize = sizeof(vertexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gHUDVertexLayout);

	if (FAILED(hr)) {

		cout << "HUD Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();

	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\HUDShaders\\HUDPixelShader.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&psBlob,
		&psErrorBlob
	);

	if (FAILED(hr)) {

		cout << "HUD Pixel Shader Error: Pixel Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob) {

			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gHUDPixelShader);

	if (FAILED(hr)) {

		cout << "Debug Pixel Shader Error: Pixel Shader could not be created" << endl;
		return false;
	}

	psBlob->Release();		


	return true;
}