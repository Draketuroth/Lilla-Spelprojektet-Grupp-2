#ifndef DEFERREDSHADERCLASS_H
#define DEFERREDSHADERCLASS_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>

#include "MacroDefinitions.h"

using namespace std;

class DeferredShaderClass {

private:

public:

	DeferredShaderClass();
	~DeferredShaderClass();

	void ReleaseAll();

	bool InitializeShader(ID3D11Device* gDevice, HWND hwnd, WCHAR* vsFileName, WCHAR*);

	ID3D11VertexShader* d_vertexShader;
	ID3D11PixelShader* d_pixelShader;
	ID3D11InputLayout* d_layout;
};

#endif
