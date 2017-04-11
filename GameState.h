#pragma once

#include "MacroDefinitions.h"
//#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
//#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
//#include <DirectXMath.h>
//#include <DirectXCollision.h>
#include "SceneContainer.h"
using namespace DirectX;
using namespace std;
struct VS_CONSTANT_BUFFER
{
	XMFLOAT4 backGroundColor;
	XMFLOAT4 rectangleColor;
	bool backgroundCheck;
	//15 bytes of padding
	//-------------------
	XMFLOAT3 pad1;
	int16_t pad2;		//15 bytes
	char pad3;
	//-------------------

};
struct RectangleData
{
	float x, y, z;
	float u, v;
};

class GameState
{
	
	
private:
	
	ID3D11Buffer* gMenuConstant;
	ID3D11Buffer* gMenuVertexBuffer;

	//ID3D11Texture2D* menuDepthStencil;
	//ID3D11DepthStencilState* menuDepthState;
	//ID3D11DepthStencilView* menuDepthView;

public:
	int state;

	GameState();
	~GameState();

	int menuHandler(HWND windowHandle, SceneContainer scene);
	int mainMenu(HWND windowHandle);
	int pauseMenu(HWND windowHandle);
	int gameOver(HWND windowHandle);

	void createVertexBuffer(ID3D11Device* gDevice);
	void createBufferData(ID3D11Device* gDevice);
	//bool createMenuDepthStencil(ID3D11Device* gDevice);
	void releaseAll();


	void renderMainMenu(SceneContainer scene);
	//void renderPauseMenu();
	//void renderGameOverMenu();



};