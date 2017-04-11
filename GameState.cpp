#include "GameState.h"

int GameState::menuHandler(HWND windowHandle, SceneContainer scene)
{
	createBufferData(scene.gHandler.gDevice);
	createMenuDepthStencil(scene.gHandler.gDevice);



	return state;
}
int GameState::mainMenu(HWND windowHandle)
{
	
	return MAIN_MENU;
}
int GameState::pauseMenu(HWND windowHandle)
{

	return PAUSE_MENU;
}
int GameState::gameOver(HWND windowHandle)
{

	return GAME_OVER;
}
void GameState::createBufferData(ID3D11Device* gDevice)
{
	HRESULT hr;
	
	XMFLOAT4 bgColor = { 0.6f, 0.4f, 0.2f, 1.0f };
	XMFLOAT4 rectColor = { 0.31f, 0.64f, 0.15f, 1.0f };


	VS_CONSTANT_BUFFER menuConstant;

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
	SAFE_RELEASE(this->menuDepthState);
	SAFE_RELEASE(this->menuDepthStencil);
	SAFE_RELEASE(this->menuDepthView);
}
void GameState::renderMainMenu(SceneContainer scene)
{
	scene.gHandler.gDeviceContext->VSSetShader(scene.gHandler.gMenuVertex, nullptr, 0);
	scene.gHandler.gDeviceContext->PSSetShader(scene.gHandler.gMenuPixel, nullptr, 0);
	scene.gHandler.gDeviceContext->GSSetShader(nullptr, nullptr, 0);

	UINT32 vertexsize = sizeof(RectangleData);
	UINT32 offset = 0;

	scene.gHandler.gDeviceContext->PSSetShaderResources(0, 0, nullptr);
	scene.gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	scene.gHandler.gDeviceContext->IASetVertexBuffers(0, 1, this->gMenuVertexBuffer, &vertexsize, &offset);

	scene.gHandler.gDeviceContext->Draw(6, 0);
}
void GameState::createVertexBuffer(ID3D11Device* gDevice)
{


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
