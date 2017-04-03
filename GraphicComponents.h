#ifndef GRAPHICCOMPONENTS_H
#define GRAPHICCOMPONENTS_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <iostream>
#include <DirectXMath.h>
#include "MacroDefinitions.h"
#include "BufferComponents.h"

using namespace std;

class GraphicComponents {

public:

	GraphicComponents();
	~GraphicComponents();
	void ReleaseAll();

	IDXGISwapChain* gSwapChain;	// Swap Chain responsible for switching between a collection of buffers and show images to the user
	ID3D11Device* gDevice;	// Graphic Device
	ID3D11DeviceContext* gDeviceContext;	// Graphic Contect
	ID3D11RenderTargetView* gBackbufferRTV;	// Render Target View

	ID3D11InputLayout* gVertexLayout;
	ID3D11VertexShader* gVertexShader;	
	ID3D11PixelShader* gPixelShader;	
	ID3D11GeometryShader* gGeometryShader;

	ID3D11Texture2D* depthStencil;	// Depth-stencil texture
	ID3D11DepthStencilState* depthState;	// Depth-stencil state used for the output merger
	ID3D11DepthStencilView* depthView;	// Depth-stencil view to access the depth stencil texture

	bool InitalizeDirect3DContext(HWND &windowHandle);	// Function called to initalize the necessary components, as listen below
	bool CreateDepthStencil();
	bool CreateRenderTargetView();	// We require a render target view for rendering and we create this by taking the back buffer from the swap chain
	bool CreateSwapChainAndDevice(HWND &windowHandle);	// Function to create the graphic device responsible for interactions with the graphic card and the swap chain to switch between back & front buffer

	bool CreateStandardShaders();
	void SetViewport();		// Functions to define the properties of our viewport

};

#endif GRAPHICCOMPONENTS_H
