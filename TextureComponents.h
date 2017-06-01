#ifndef TEXTURECOMPONENTS_H
#define TEXTURECOMPONENTS_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include "DebugHelper.h"
#include "WICTextureLoader.h"

using namespace std;
using namespace DirectX;

class TextureComponents {

public:

	TextureComponents();
	~TextureComponents();
	void ReleaseAll();

	ID3D11ShaderResourceView* platformGrass; // Released
	ID3D11ShaderResourceView* platformStone; // Released
	ID3D11ShaderResourceView* platformStoneCracks; // Released

	ID3D11ShaderResourceView* fortressResource; // Released
	ID3D11ShaderResourceView* iceEnemyResource; // Released
	ID3D11ShaderResourceView* lavaEnemyResource; // Released
	ID3D11ShaderResourceView* playerResource; // Released

	ID3D11ShaderResourceView* LavaResource; // Released

	ID3D11ShaderResourceView* projectileResource; // Released
	ID3D11SamplerState* texSampler; // Released
	ID3D11BlendState* blendState; // Released

	ID3D11SamplerState* shadowSampler; // Released
	ID3D11ShaderResourceView* shadowSRV; // Released
	ID3D11DepthStencilView* shadowDepthView; // Released
	ID3D11Texture2D* ShadowMap; // Released

	ID3D11ShaderResourceView* HUDResource; // Released
	ID3D11ShaderResourceView* HUDPortrait; // Released
	ID3D11ShaderResourceView* HUDHealth; // Released

	ID3D11ShaderResourceView* menuResources[9];
	ID3D11ShaderResourceView* texArr[4];
	ID3D11SamplerState* samplerArr[2];

	DXGI_FORMAT GetDepthSRVFormat(DXGI_FORMAT depthformat);
	DXGI_FORMAT GetDepthResourceFormat(DXGI_FORMAT depthformat);

	bool CreateTexture(ID3D11Device* &gDevice);
	bool CreateShadowMap(ID3D11Device* &gDevice);
};

#endif TEXTURECOMPONENTS_H
