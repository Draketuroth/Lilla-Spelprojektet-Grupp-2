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

	ID3D11InputLayout* gMenuLayout;
	ID3D11VertexShader* gMenuVertex;
	ID3D11PixelShader* gMenuPixel;

	ID3D11InputLayout* gPlatformLayout;
	ID3D11VertexShader* gPlatformVertexShader;
	ID3D11PixelShader* gPlatformPixelShader;
	ID3D11GeometryShader* gPlatformGeometryShader;

	ID3D11InputLayout* gFortressLayout;
	ID3D11VertexShader* gFortressVertexShader;
	ID3D11PixelShader* gFortressPixelShader;

	ID3D11InputLayout* gLavaVertexLayout;
	ID3D11VertexShader* gLavaVertexShader;
	ID3D11PixelShader* gLavaPixelShader;
	ID3D11GeometryShader* gLavaGeometryShader;

	ID3D11InputLayout* gProjectileVertexLayout;
	ID3D11VertexShader* gProjectileVertexShader;
	ID3D11PixelShader* gProjectilePixelShader;

	ID3D11Texture2D* depthStencil;	// Depth-stencil texture
	ID3D11DepthStencilState* depthState;	// Depth-stencil state used for the output merger
	ID3D11DepthStencilView* depthView;	// Depth-stencil view to access the depth stencil texture

	ID3D11InputLayout* gIceEnemyVertexLayout;
	ID3D11VertexShader* gIceEnemyVertexShader;
	ID3D11PixelShader* gIceEnemyPixelShader;

	ID3D11InputLayout* gLavaEnemyVertexLayout;
	ID3D11VertexShader* gLavaEnemyVertexShader;
	ID3D11PixelShader* gLavaEnemyPixelShader;

	ID3D11InputLayout* gDebugVertexLayout;
	ID3D11VertexShader* gDebugVertexShader;
	ID3D11PixelShader* gDebugPixelShader;

	ID3D11InputLayout* gHUDVertexLayout;
	ID3D11VertexShader* gHUDVertexShader;
	ID3D11PixelShader* gHUDPixelShader;

	ID3D11VertexShader* gShadowVertexShader;//Player shadow map
	ID3D11InputLayout* gShadowVertexLayout;

	ID3D11VertexShader* gShadowPlatformVertex;//Shadow map for platform
	ID3D11InputLayout* gShadowPlatformLayout;

	ID3D11VertexShader* gShadowLavaVertex;
	ID3D11InputLayout* gShadowLayaLayout;

	ID3D11VertexShader* gShadowIceVertex;
	ID3D11InputLayout* gShadowIceLayout;

	ID3D11VertexShader* rayVertexShader;
	ID3D11PixelShader* rayPixelShader;
	ID3D11InputLayout* rayInputLayout;



	bool InitalizeDirect3DContext(HWND &windowHandle);	// Function called to initalize the necessary components, as listen below
	bool CreateDepthStencil();
	bool CreateRenderTargetView();	// We require a render target view for rendering and we create this by taking the back buffer from the swap chain
	bool CreateSwapChainAndDevice(HWND &windowHandle);	// Function to create the graphic device responsible for interactions with the graphic card and the swap chain to switch between back & front buffer

	bool CreateShadowShaders();

	bool CreateDebugShaders();
	bool CreateMenuShaders();
	bool CreateStandardShaders();
	bool CreatePlatformShaders();
	bool CreateFortressShader();
	bool CreateRayShaders();
	bool CreateIceEnemyShaders();
	bool CreateLavaEnemyShaders();

	bool CreateLavaShaders(); 
	bool CreateHUDShaders();

	bool CreateProjectileShaders();
	void SetViewport();		// Functions to define the properties of our viewport

};

#endif GRAPHICCOMPONENTS_H
