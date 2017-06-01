#ifndef GRAPHICCOMPONENTS_H
#define GRAPHICCOMPONENTS_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <iostream>
#include <DirectXMath.h>
#include "DebugHelper.h"
#include "BufferComponents.h"

using namespace std;

class GraphicComponents {

public:

	GraphicComponents();
	~GraphicComponents();
	void ReleaseAll();

	IDXGISwapChain* gSwapChain;	// Released
	ID3D11Device* gDevice; // Released
	ID3D11DeviceContext* gDeviceContext; // Released
	ID3D11RenderTargetView* gBackbufferRTV;	// Released

	ID3D11InputLayout* gVertexLayout; // Released
	ID3D11VertexShader* gVertexShader; // Releasrd
	ID3D11PixelShader* gPixelShader; // Released

	ID3D11InputLayout* gMenuLayout; // Released
	ID3D11VertexShader* gMenuVertex; // Released
	ID3D11PixelShader* gMenuPixel; // Released

	ID3D11InputLayout* gPlatformLayout; // Released
	ID3D11VertexShader* gPlatformVertexShader; // Released
	ID3D11PixelShader* gPlatformPixelShader; // Released

	ID3D11InputLayout* gFortressLayout; // Released
	ID3D11VertexShader* gFortressVertexShader; // Released
	ID3D11PixelShader* gFortressPixelShader; // Released

	ID3D11InputLayout* gLavaVertexLayout; // Released
	ID3D11VertexShader* gLavaVertexShader; // Released
	ID3D11PixelShader* gLavaPixelShader; // Released
	ID3D11GeometryShader* gLavaGeometryShader; // Released

	ID3D11InputLayout* gProjectileVertexLayout;
	ID3D11VertexShader* gProjectileVertexShader;
	ID3D11PixelShader* gProjectilePixelShader;

	ID3D11Texture2D* depthStencil; // Released
	ID3D11DepthStencilState* depthState; // Released
	ID3D11DepthStencilView* depthView; // Released

	ID3D11InputLayout* gIceEnemyVertexLayout; // Released
	ID3D11VertexShader* gIceEnemyVertexShader; // Released
	ID3D11PixelShader* gIceEnemyPixelShader; // Released

	ID3D11InputLayout* gLavaEnemyVertexLayout; // Released
	ID3D11VertexShader* gLavaEnemyVertexShader; // Released
	ID3D11PixelShader* gLavaEnemyPixelShader; // Released

	ID3D11InputLayout* gDebugVertexLayout; // Released
	ID3D11VertexShader* gDebugVertexShader; // Released
	ID3D11PixelShader* gDebugPixelShader; // Released

	ID3D11InputLayout* gHUDVertexLayout; // Released
	ID3D11VertexShader* gHUDVertexShader; // Released
	ID3D11PixelShader* gHUDPixelShader; // Released

	ID3D11VertexShader* gShadowVertexShader; // Released
	ID3D11InputLayout* gShadowVertexLayout; // Released

	ID3D11VertexShader* gShadowPlatformVertex; // Released
	ID3D11InputLayout* gShadowPlatformLayout; // Released

	ID3D11VertexShader* gShadowLavaVertex; // Released
	ID3D11InputLayout* gShadowLavaLayout; // Released

	ID3D11VertexShader* gShadowIceVertex; // Released
	ID3D11InputLayout* gShadowIceLayout; // Released

	ID3D11VertexShader* rayVertexShader; // Released
	ID3D11PixelShader* rayPixelShader; // Released
	ID3D11InputLayout* rayInputLayout; // Released



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
