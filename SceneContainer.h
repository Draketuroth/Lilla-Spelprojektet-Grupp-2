#ifndef SCENECONTAINER_H
#define SCENECONTAINER_H

#include <windows.h>

#include <d3d11.h>	
#include <d3dcompiler.h>	

#include <iostream>
#include <Windows.h>
#include "Camera.h"
#include "WICTextureLoader.h"
#include "Window.h"

#include "DeferredBuffersClass.h"
#include "DeferredShaderClass.h"
#include "LightShaderClass.h"

#include "GraphicComponents.h"
#include "BufferComponents.h"
#include "TextureComponents.h"
#include "BulletComponents.h"

#include "MainCharacter.h"
#include "Enemies.h"

#include "FBXLoader.h"

class SceneContainer {

private:


public:

	SceneContainer();
	~SceneContainer();

	void releaseAll();

	bool initialize(HWND &windowHandle);
	void update(HWND &windowHandle);

	GraphicComponents gHandler;
	BufferComponents bHandler;
	TextureComponents tHandler;

	DeferredBuffersClass deferredObject;
	DeferredShaderClass deferredShaders;
	LightShaderClass lightShaders;

	MainCharacter character;
	FbxImport fbxImporter;

	Enemy enemy;
	Enemy enemies[3];

	BulletComponents bulletPhysicsHandler;

	int nrOfEnemies;

	//------------------------------------------------------------//
	// RENDER FUNCTIONS
	//------------------------------------------------------------//

	void drawPlatforms();
	
	void clear();
	void resetRenderTarget(GraphicComponents &gHandler);

	void render();

	bool renderDeferred();

	bool renderSceneToTexture();

	void renderScene();
	void renderCharacters();
	void renderEnemies();

};




#endif SCENECONTAINER_H

