
#include "TextureComponents.h"

TextureComponents::TextureComponents() {

	ID3D11ShaderResourceView* standardResource = nullptr;
	ID3D11SamplerState* texSampler = nullptr;

	ID3D11ShaderResourceView* LavaResurce = nullptr; 

}

TextureComponents::~TextureComponents() {


}

void TextureComponents::ReleaseAll() {

	SAFE_RELEASE(standardResource);
	SAFE_RELEASE(texSampler);
	SAFE_RELEASE(LavaResurce); 
}

bool TextureComponents::CreateTexture(ID3D11Device* &gDevice) {

	HRESULT hr;

	// In order to create the texture, we require a sampler state to determine how the texture data should be sampled using filtering, addressing modes 
	// and level of detail

	//----------------------------------------------------------------------------------------------------------------------------------//
	// SAMPLER STATE DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = gDevice->CreateSamplerState(&sampDesc, &texSampler);

	if (FAILED(hr)) {

		return false;
	}

	ID3D11Texture2D* texture = nullptr;

	CoInitialize(NULL);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\small.jpg", NULL, &standardResource, 1024);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\Lava1.jpg", NULL, &LavaResurce, 1024);

	if (SUCCEEDED(hr) && texture != 0) {

		gDevice->CreateShaderResourceView(texture, nullptr, &standardResource);
		gDevice->CreateShaderResourceView(texture, nullptr, &LavaResurce);

		if (FAILED(hr)) {

			texture->Release();
		}
	}

	return true;
}