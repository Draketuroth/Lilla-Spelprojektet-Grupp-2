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

	bool InitializeShader(ID3D11Device* gDevice);
	bool Render(ID3D11DeviceContext* gDeviceContext, ID3D11ShaderResourceView* texture, int indexCount);
	bool SetShaderParameters(ID3D11DeviceContext* gDeviceContext, ID3D11ShaderResourceView* texture);
	void RenderShader(ID3D11DeviceContext* gDeviceContext, int indexCount);

	ID3D11VertexShader* d_vertexShader;
	ID3D11GeometryShader* d_geometryShader;
	ID3D11PixelShader* d_pixelShader;
	ID3D11InputLayout* d_layout;
};

#endif
