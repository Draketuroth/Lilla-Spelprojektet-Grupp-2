
#include "TextureComponents.h"

TextureComponents::TextureComponents() {

	ID3D11ShaderResourceView* standardResource = nullptr;
	ID3D11SamplerState* texSampler = nullptr;

	ID3D11ShaderResourceView* LavaResurce = nullptr; 

}

TextureComponents::~TextureComponents() {


}

void TextureComponents::ReleaseAll() {

	SAFE_RELEASE(platformResource);
	SAFE_RELEASE(fortressResource);
	SAFE_RELEASE(defaultResource);
	SAFE_RELEASE(texSampler);
	SAFE_RELEASE(LavaResource); 
	SAFE_RELEASE(HUDResource);
	SAFE_RELEASE(blendState);
	for (size_t i = 0; i < 9; i++)
	{
		SAFE_RELEASE(this->menuResources[i]);
	}
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



	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;//D3D11_BLEND_SRC1_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_ALPHA;//D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = gDevice->CreateBlendState(&blendDesc, &blendState);



	if (FAILED(hr)) {

		return false;
	}

	ID3D11Texture2D* texture = nullptr;

	CoInitialize(NULL);
	CreateWICTextureFromFile(gDevice, NULL, L"Format\\Textures\\platformTexture.png", NULL, &platformResource, 1024);
	CreateWICTextureFromFile(gDevice, NULL, L"Format\\Textures\\FortressTexture.png", NULL, &fortressResource, 1024);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\small.jpg", NULL, &defaultResource, 1024);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\Lava1.jpg", NULL, &LavaResource, 1024);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\MAIN.png", NULL, &menuResources[0], 1920);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\MAIN_PLAY_CLICK.png", NULL, &menuResources[1], 1920);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\MAIN_QUIT_CLICK.png", NULL, &menuResources[2], 1920);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\PAUSE.png", NULL, &menuResources[3], 1920);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\PAUSE_RESUME_CLICK.png", NULL, &menuResources[4], 1920);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\PAUSE_QUIT_CLICK.png", NULL, &menuResources[5], 1920);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\GAMEOVER.png", NULL, &menuResources[6], 1920);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\GAMEOVER_RESTART_CLICK.png", NULL, &menuResources[7], 1920);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\GAMEOVER_QUIT_CLICK.png", NULL, &menuResources[8], 1920);
	CreateWICTextureFromFile(gDevice, NULL, L"Fonts\\HUDFont.png", NULL, &HUDResource, 256);

	if (SUCCEEDED(hr) && texture != 0) {

		gDevice->CreateShaderResourceView(texture, nullptr, &platformResource);
		gDevice->CreateShaderResourceView(texture, nullptr, &fortressResource);
		gDevice->CreateShaderResourceView(texture, nullptr, &defaultResource);
		gDevice->CreateShaderResourceView(texture, nullptr, &LavaResource);
		gDevice->CreateShaderResourceView(texture, nullptr, &menuResources[0]);
		gDevice->CreateShaderResourceView(texture, nullptr, &menuResources[1]);
		gDevice->CreateShaderResourceView(texture, nullptr, &menuResources[2]);
		gDevice->CreateShaderResourceView(texture, nullptr, &menuResources[3]);
		gDevice->CreateShaderResourceView(texture, nullptr, &menuResources[4]);
		gDevice->CreateShaderResourceView(texture, nullptr, &menuResources[5]);
		gDevice->CreateShaderResourceView(texture, nullptr, &menuResources[6]);
		gDevice->CreateShaderResourceView(texture, nullptr, &menuResources[7]);
		gDevice->CreateShaderResourceView(texture, nullptr, &menuResources[8]);
		gDevice->CreateShaderResourceView(texture, nullptr, &HUDResource);

		if (FAILED(hr)) {

			texture->Release();
		}
	}

	return true;
}