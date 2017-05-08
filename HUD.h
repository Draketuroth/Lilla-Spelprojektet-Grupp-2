#pragma once
#ifndef HUD_H
#define HUD_H
#include "MacroDefinitions.h"
#include<string>
#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
using namespace DirectX;

struct HUDElements
{
	float x, y;
};

class HUDClass
{

private:







public:

	

	HUDClass();
	~HUDClass();

	bool setElementPos(ID3D11Device* &gDevice);
	void ReleaseAll();




	ID3D11Buffer* gElementVertexBuffer;
};





#endif // !HUD_H
