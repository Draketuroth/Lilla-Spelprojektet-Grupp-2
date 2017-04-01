#ifndef DEFERREDBUFFERCLASS_H
#define DEFERREDBUFFERCLASS_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>

#include "MacroDefinitions.h"

#define BUFFER_COUNT 2

using namespace std;
using namespace DirectX;

class DeferredBuffersClass {

private:

	int d_textureWidth, d_textureHeight;

	ID3D11Texture2D* d_renderTargetTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView* d_renderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* d_shaderResourceViewArray[BUFFER_COUNT];
	ID3D11Texture2D* d_depthStencilBuffer;
	ID3D11DepthStencilView* d_depthStencilView;
	ID3D11DepthStencilState* d_depthStencilState;	// Depth-stencil state used for the output merger
	D3D11_VIEWPORT d_viewPort;

public:

	DeferredBuffersClass();
	~DeferredBuffersClass();
	void ReleaseAll();

	bool Initialize(ID3D11Device* gDevice);
	bool SetupResources(ID3D11Device* gDevice);
	bool CreateDepthStencil(ID3D11Device* gDevice);
	void CreateViewport();

	void SetRenderTargets(ID3D11DeviceContext* gDeviceContext);
	void ClearRenderTargets(ID3D11DeviceContext* gDeviceContext, float red, float green, float blue, float alpha);

	ID3D11ShaderResourceView* GetShaderResourceView(int);


};


#endif DEFERREDBUFFERCLASS_H
