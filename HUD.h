#pragma once
#ifndef HUD_H
#define HUD_H
#include "MacroDefinitions.h"
#include <string>
#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
using namespace std;
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

	bool setFont(ID3D11Device* &gDevice);
	bool CreateFontIndexBuffer(ID3D11Device* &gDevice);
	void ReleaseAll();

	void setText(int wave);
	void loadBitMap();


	ID3D11Buffer* gElementVertexBuffer;
	ID3D11Buffer* gElementIndexBuffer;

	ID3D11Buffer* gFontVertexBuffer;
	ID3D11Buffer* gFontIndexBuffer;

	string waveText;
	int nrOfChars;
	int indexArr[69][5];
	int foo;
	int Vtxs;

	vector<char> text;
	vector<int> ascii;
};





#endif // !HUD_H
