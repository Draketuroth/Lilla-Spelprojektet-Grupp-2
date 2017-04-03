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

#include "GraphicComponents.h"
#include "BufferComponents.h"
#include "TextureComponents.h"

class SceneContainer {

private:




public:

	SceneContainer();
	~SceneContainer();

	void releaseAll();

	bool initialize(HWND &windowHandle, Camera &mCam);

	GraphicComponents gHandler;
	BufferComponents bHandler;
	TextureComponents tHandler;

};




#endif SCENECONTAINER_H

