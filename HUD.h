#pragma once
#ifndef HUD_H
#define HUD_H
#include "MacroDefinitions.h"
#include<string>
#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <SFML\Graphics.hpp>
using namespace DirectX;

struct HUDElements
{
	float x, y, z;
	float u, v;
};

class HUDClass
{

private:







public:

	

	HUDClass();
	~HUDClass();

	bool setElementPos(ID3D11Device* &gDevice);
	bool CreateIndexBuffer(ID3D11Device* &gDevice);
	void ReleaseAll();

	sf::Font font;
	sf::Text text;

	ID3D11Buffer* gElementVertexBuffer;
	ID3D11Buffer* gElementIndexBuffer;
};





#endif // !HUD_H
