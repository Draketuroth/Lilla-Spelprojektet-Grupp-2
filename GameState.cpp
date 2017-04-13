#include "GameState.h"
GameState::GameState()
{
	this->state = MAIN_MENU;
}
GameState::~GameState()
{

}
int GameState::menuHandler(HWND windowHandle, SceneContainer scene)
{
	createBufferData(scene.gHandler.gDevice);
	createVertexBuffer(scene.gHandler.gDevice);
	createIndexBuffer(scene.gHandler.gDevice);
	mainMenu(windowHandle, scene);

	return state;
}
int GameState::mainMenu(HWND windowHandle, SceneContainer scene)
{

	while (this->state == MAIN_MENU)
	{
		renderMainMenu(scene);
		if (GetAsyncKeyState('T') & 0x8000)
		{
			this->state = START_GAME;
		}
		if (GetAsyncKeyState(VK_LBUTTON) & VK_LBUTTON)
		{
			
			
			getMousePos(windowHandle, scene);
		//	cout << "MousePos X: " << this->floatMouse.x << endl << "MousePos Y: " << this->floatMouse.y << endl;
			if (this->floatMouse.x <= 0.3f && this->floatMouse.x >= -0.3f && this->floatMouse.y <= 0.7f && this->floatMouse.y >= 0.4f)
			{
				cout << "Position X: " << this->floatMouse.x << endl << "Position Y: " << this->floatMouse.y << endl;
				this->state = START_GAME;
			}
		}
	}
	return state;
}
int GameState::pauseMenu(HWND windowHandle)
{
	while (this->state == PAUSE_MENU)
	{

	}

	return state;
}
int GameState::gameOver(HWND windowHandle)
{

	return state;
}
void GameState::createBufferData(ID3D11Device* gDevice)
{
	HRESULT hr;
	
	XMFLOAT4 bgColor = { 0.6f, 0.4f, 0.2f, 1.0f };
	XMFLOAT4 rectColor = { 0.31f, 0.64f, 0.15f, 1.0f };

	VS_CONSTANT_BUFFER menuConstant;

	menuConstant.backGroundColor = bgColor;
	menuConstant.rectangleColor = rectColor;
	menuConstant.menuWorld = XMMatrixIdentity();

	// The buffer description is filled in below, mainly so the graphic card understand the structure of it

	D3D11_BUFFER_DESC constBufferDesc;
	ZeroMemory(&constBufferDesc, sizeof(constBufferDesc));
	constBufferDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc.MiscFlags = 0;
	constBufferDesc.StructureByteStride = 0;

	// We set the the subresource data

	D3D11_SUBRESOURCE_DATA constData;
	constData.pSysMem = &menuConstant;
	constData.SysMemPitch = 0;
	constData.SysMemSlicePitch = 0;

	// Finally after creating description and subresource data, we create the constant buffer

	hr = gDevice->CreateBuffer(&constBufferDesc, &constData, &gMenuConstant);

}
void GameState::releaseAll()
{
	SAFE_RELEASE(this->gMenuConstant);
	SAFE_RELEASE(this->gMenuVertexBuffer);
	SAFE_RELEASE(this->gMenuIndex);
}
void GameState::renderMainMenu(SceneContainer scene)
{
	scene.clear();

	scene.gHandler.gDeviceContext->VSSetShader(scene.gHandler.gMenuVertex, nullptr, 0);
	scene.gHandler.gDeviceContext->PSSetShader(scene.gHandler.gMenuPixel, nullptr, 0);
	scene.gHandler.gDeviceContext->PSSetShaderResources(0, 0, nullptr);
	scene.gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &this->gMenuConstant);
	scene.gHandler.gDeviceContext->GSSetShader(nullptr, nullptr, 0);

	UINT32 vertexsize = sizeof(RectangleData);
	UINT32 offset = 0;
	scene.gHandler.gDeviceContext->IASetIndexBuffer(this->gMenuIndex, DXGI_FORMAT_R32_UINT, 0);
	scene.gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &this->gMenuVertexBuffer, &vertexsize, &offset);

	scene.gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	scene.gHandler.gDeviceContext->IASetInputLayout(scene.gHandler.gMenuLayout);

	scene.gHandler.gDeviceContext->DrawIndexed(6, 0, 0);
	//Don't forget the swapchain, måste ha för att byta mellan det som ritas och det som visas
	scene.gHandler.gSwapChain->Present(0, 0);
}
bool GameState::createVertexBuffer(ID3D11Device* gDevice)
{
	RectangleData quad[4]
	{
		-0.3f, 0.7f, 0.0f,//Top left pos
		0.0f, 0.0f,//top left uv

		-0.3f, 0.4f, 0.0f,//Bot left pos
		0.0f, 1.0f,//Bot left uv

		0.3f, 0.7f, 0.0f,//Top right pos
		1.0f, 0.0f,//Top right uv

		0.3f, 0.4f, 0.0f,//Bot right pos
		1.0f, 1.0f
	};
	HRESULT hr;

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(quad);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = quad;
	hr = gDevice->CreateBuffer(&bufferDesc, &data, &this->gMenuVertexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}
bool GameState::createIndexBuffer(ID3D11Device* gDevice)
{
	HRESULT hr;

	unsigned int indices[6] =
	{
		1, 0, 2, 1, 2, 3
	};

	// Create the buffer description
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(unsigned int) * 6;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Set the subresource data

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = indices;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	// Create the buffer

	hr = gDevice->CreateBuffer(&bufferDesc, &initData, &gMenuIndex);

	if (FAILED(hr)) {

		return false;
	}



	return true;
}
void GameState::getMousePos(HWND windowHandle, SceneContainer scene)
{

	GetCursorPos(&this->mousePos);
	ScreenToClient(windowHandle, &this->mousePos);
	this->floatMouse.x = mousePos.x;
	this->floatMouse.y = mousePos.y;
	//XMVECTOR vecMouse;
	this->floatMouse.x = (2 * this->floatMouse.x) / WIDTH - 1;
	this->floatMouse.y = -(2 * this->floatMouse.y) / HEIGHT + 1;
	//Matrix calculations for taking the pixel coordinates to clip space
	//XMMATRIX inViewProj;
	//inViewProj = scene.character.camera.ViewProj();
	//XMVECTOR det = XMMatrixDeterminant(inViewProj);
	//XMMatrixInverse(&det, inViewProj);

	//XMFLOAT2 mP;//Storing the mousePos in a xmfloat2
	//mP.x = this->mousePos.x;
	//mP.y = this->mousePos.y;

	//vecMouse = XMLoadFloat2(&mP);//Storing the mousepos in a vector

	//XMVector2Transform(vecMouse, inViewProj);//Multiply the inverse of ViewProj with the mousePos
	//XMStoreFloat2(&mP, vecMouse);
	//this->mousePos.x = mP.x;
	//this->mousePos.y = mP.y;
}
//bool GameState::createMenuDepthStencil(ID3D11Device* gDevice)
//{
//	HRESULT hr;
//
//	// Depth Texture
//	//----------------------------------------------------------------------------------------------------------------------------------//
//
//	// THe depth buffer texture is used to store the distance of each fragment to the camera
//
//	D3D11_TEXTURE2D_DESC descDepth;
//	descDepth.Width = WIDTH;
//	descDepth.Height = HEIGHT;
//	descDepth.MipLevels = 1;
//	descDepth.ArraySize = 1;
//	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	descDepth.SampleDesc.Count = 1;
//	descDepth.SampleDesc.Quality = 0;
//	descDepth.Usage = D3D11_USAGE_DEFAULT;
//	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
//	descDepth.CPUAccessFlags = 0;
//	descDepth.MiscFlags = 0;
//
//	hr = gDevice->CreateTexture2D(&descDepth, nullptr, &menuDepthStencil);
//
//	if (FAILED(hr)) {
//
//		return false;
//	}
//
//	//----------------------------------------------------------------------------------------------------------------------------------//
//
//
//	// Depth View Description
//	//----------------------------------------------------------------------------------------------------------------------------------//
//
//	// Create depth view description. A depth-stencil-view interface process a texture resource during the depth-stencil testing.
//	D3D11_DEPTH_STENCIL_VIEW_DESC dViewDesc;
//	dViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
//	dViewDesc.Texture2D.MipSlice = 0;
//	dViewDesc.Flags = 0;
//
//	//Create depth stencil view
//	hr = gDevice->CreateDepthStencilView(menuDepthStencil, &dViewDesc, &menuDepthView);
//
//	if (FAILED(hr)) {
//
//		return false;
//	}
//
//	// Depth State Description
//	//----------------------------------------------------------------------------------------------------------------------------------//
//
//	D3D11_DEPTH_STENCIL_DESC stencilDesc;
//
//	// Depth test
//	stencilDesc.DepthEnable = true;
//	stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//	stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
//
//	// Stencil test
//	stencilDesc.StencilEnable = true;
//	stencilDesc.StencilReadMask = 0xFF;
//	stencilDesc.StencilWriteMask = 0xFF;
//
//	// Stencil operations if the pixel is facing forward
//
//	stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
//	stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//
//	// Stencil operations if the pixel is facing backward
//
//	stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
//	stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//
//	// Create Depth State
//	hr = gDevice->CreateDepthStencilState(&stencilDesc, &menuDepthState);
//
//	if (FAILED(hr)) {
//
//		return false;
//	}
//
//	//----------------------------------------------------------------------------------------------------------------------------------//
//	return true;
//}
