
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
	SAFE_RELEASE(shadowSRV);
	SAFE_RELEASE(shadowDepthView);
	SAFE_RELEASE(shadowSampler);
	SAFE_RELEASE(ShadowMap);
	for (size_t i = 0; i < 9; i++)
	{
		SAFE_RELEASE(this->menuResources[i]);
	}
	for (size_t i = 0; i < 2; i++)
	{
		SAFE_RELEASE(samplerArr[i]);
		SAFE_RELEASE(texArr[i]);
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
	//sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
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

		if (FAILED(hr)) {

			texture->Release();
		}
	}

	return true;
}
bool TextureComponents::CreateShadowMap(ID3D11Device* &gDevice)
{
	HRESULT hr;

	//DXGI_FORMAT resformat = DXGI_FORMAT_R32_TYPELESS;//DXGI_FORMAT_R32G8X24_TYPELESS;
	//DXGI_FORMAT srvformat = DXGI_FORMAT_R32_TYPELESS;//DXGI_FORMAT_R32G8X24_TYPELESS;

	DXGI_FORMAT resformat = GetDepthResourceFormat(DXGI_FORMAT_D32_FLOAT_S8X24_UINT);
	DXGI_FORMAT srvformat = GetDepthSRVFormat(DXGI_FORMAT_D32_FLOAT_S8X24_UINT);

	//Shadow map sampler
	D3D11_SAMPLER_DESC shadowSamp;
	ZeroMemory(&shadowSamp, sizeof(shadowSamp));
	shadowSamp.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	shadowSamp.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	shadowSamp.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	shadowSamp.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	shadowSamp.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	shadowSamp.MinLOD = 0;
	shadowSamp.MaxLOD = D3D11_FLOAT32_MAX;


	hr = gDevice->CreateSamplerState(&shadowSamp, &this->shadowSampler);
	if (FAILED(hr))
	{
		return false;
	}
	//Shadow map texture desc
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = 1024;
	texDesc.Height = 1024;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = resformat;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	hr = gDevice->CreateTexture2D(&texDesc, NULL, &this->ShadowMap);
	if (FAILED(hr))
	{
		return false;
	}
	
	//Depth stencil view description
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = gDevice->CreateDepthStencilView(this->ShadowMap, &descDSV, &this->shadowDepthView);
	if (FAILED(hr))
	{
		return false;
	}

	//Shader resource view description
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = srvformat;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;

	hr = gDevice->CreateShaderResourceView(this->ShadowMap, &srvDesc, &this->shadowSRV);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}
DXGI_FORMAT TextureComponents::GetDepthResourceFormat(DXGI_FORMAT depthformat)
{
	DXGI_FORMAT resformat;
	switch (depthformat)
	{
	case DXGI_FORMAT::DXGI_FORMAT_D16_UNORM:
		resformat = DXGI_FORMAT::DXGI_FORMAT_R16_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT:
		resformat = DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT:
		resformat = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		resformat = DXGI_FORMAT::DXGI_FORMAT_R32G8X24_TYPELESS;
		break;
	}

	return resformat;
}
DXGI_FORMAT TextureComponents::GetDepthSRVFormat(DXGI_FORMAT depthformat)
{
	DXGI_FORMAT srvformat;
	switch (depthformat)
	{
	case DXGI_FORMAT::DXGI_FORMAT_D16_UNORM:
		srvformat = DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT:
		srvformat = DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT:
		srvformat = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		srvformat = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		break;
	}
	return srvformat;
}