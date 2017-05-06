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

	ID3D11ShaderResourceView* platformResource;
	ID3D11ShaderResourceView* fortressResource;
	ID3D11ShaderResourceView* LavaResource;
	ID3D11ShaderResourceView* menuResources[9];

	ID3D11SamplerState* texSampler;

	bool CreateTexture(ID3D11Device* &gDevice);
};

#endif TEXTURECOMPONENTS_H
