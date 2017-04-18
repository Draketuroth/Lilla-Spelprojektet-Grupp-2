#include "GameState.h"
GameState::GameState()
{
	this->state = MAIN_MENU;

}
GameState::~GameState()
{

}
int GameState::menuHandler(HWND windowHandle, SceneContainer scene, MSG windowMessage)
{
	if (this->state == MAIN_MENU)
	{
		mainMenu(windowHandle, scene);
	}
	else if (this->state == PAUSE_MENU)
	{
		pauseMenu(windowHandle, scene, windowMessage);
	}
	return state;
}
int GameState::mainMenu(HWND windowHandle, SceneContainer scene)
{
	if (this->state == MAIN_MENU)
	{
		renderMainMenu(scene);
		if (GetAsyncKeyState(VK_LBUTTON))
		{			
			getMousePos(windowHandle, scene);	
			if (this->floatMouse.x <= 0.3f && this->floatMouse.x >= -0.3f && this->floatMouse.y <= 0.7f && this->floatMouse.y >= 0.4f)
			{
				this->state = START_GAME;
			}
		}
	}
	return state;
}
int GameState::pauseMenu(HWND windowHandle, SceneContainer scene, MSG windowMessage)
{
	if (this->state == PAUSE_MENU)
	{
		renderMainMenu(scene);
		if (GetAsyncKeyState(VK_LBUTTON))
		{
			getMousePos(windowHandle, scene);
			if (this->floatMouse.x <= 0.3f && this->floatMouse.x >= -0.3f && this->floatMouse.y <= 0.7f && this->floatMouse.y >= 0.4f)
			{
				this->state = START_GAME;
			}
		}
	}

	return state;
}
int GameState::gameOver(HWND windowHandle, SceneContainer scene)
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
	this->floatMouse.x = (2 * this->floatMouse.x) / WIDTH - 1;
	this->floatMouse.y = -(2 * this->floatMouse.y) / HEIGHT + 1;
}
void GameState::checkGameState()
{
	if (this->state == START_GAME)
	{
		if (GetAsyncKeyState('P'))
		{
			this->state = PAUSE_MENU;
		}
	}
}