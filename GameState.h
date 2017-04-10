#pragma once

#include "MacroDefinitions.h"
#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;

class GameState
{

private:
	int state;



public:
	int mainMenu(HWND windowHandle);
	int pauseMenu(HWND windowHandle);
	int gameOver(HWND windowHandle);

	void renderMainMenu();
	void renderPauseMenu();
	void renderGameOverMenu();



};