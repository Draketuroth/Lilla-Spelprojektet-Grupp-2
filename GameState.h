#pragma once
#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "MacroDefinitions.h"
//#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
//#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
//#include <DirectXMath.h>
//#include <DirectXCollision.h>
#include "SceneContainer.h"

using namespace DirectX;
using namespace std;

class GameState
{
	
	
private:
	
	POINT mousePos;
	XMFLOAT2 floatMouse;

public:
	int state;

	GameState();
	~GameState();

	int menuHandler(HWND windowHandle, SceneContainer scene, MSG windowMessage);
	int mainMenu(HWND windowHandle, SceneContainer scene);
	int pauseMenu(HWND windowHandle, SceneContainer scene, MSG windowMessage);
	int gameOver(HWND windowHandle, SceneContainer scene);

	void checkGameState();

	void getMousePos(HWND windowHandle, SceneContainer scene);

	void renderMenus(SceneContainer scene);

	sf::SoundBuffer soundBuffer;

	sf::Music arenaMusic;
	sf::Sound clickSound;


};
#endif GAMESTATE_H