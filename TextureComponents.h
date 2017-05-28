#ifndef TEXTURECOMPONENTS_H
#define TEXTURECOMPONENTS_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include "MacroDefinitions.h"
#include "WICTextureLoader.h"

using namespace std;
using namespace DirectX;

class TextureComponents {

public:

	TextureComponents();
	~TextureComponents();
	void ReleaseAll();

	ID3D11ShaderResourceView* platformGrass;
	ID3D11ShaderResourceView* platformStone;
	ID3D11ShaderResourceView* platformStoneCracks;
	ID3D11ShaderResourceView* fortressResource;
	ID3D11ShaderResourceView* iceEnemyResource;
	ID3D11ShaderResourceView* lavaEnemyResource;
	ID3D11ShaderResourceView* playerResource;
	ID3D11ShaderResourceView* LavaResource;
	ID3D11ShaderResourceView* projectileResource;
	ID3D11ShaderResourceView* menuResources[9];
	ID3D11ShaderResourceView* texArr[4];
	ID3D11SamplerState* samplerArr[2];
	


	ID3D11SamplerState* shadowSampler;
	ID3D11Texture2D* ShadowMap;
	ID3D11DepthStencilView* shadowDepthView;
	ID3D11ShaderResourceView* shadowSRV;

	DXGI_FORMAT GetDepthSRVFormat(DXGI_FORMAT depthformat);
	DXGI_FORMAT GetDepthResourceFormat(DXGI_FORMAT depthformat);
	ID3D11ShaderResourceView* HUDResource;

	ID3D11ShaderResourceView* HUDPortrait;
	ID3D11ShaderResourceView* HUDHealth;
	ID3D11BlendState* blendState;

	ID3D11SamplerState* texSampler;

	bool CreateTexture(ID3D11Device* &gDevice);
	bool CreateShadowMap(ID3D11Device* &gDevice);
};

#endif TEXTURECOMPONENTS_H
