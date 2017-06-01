
#include "TextureComponents.h"

TextureComponents::TextureComponents() {

}

TextureComponents::~TextureComponents() {


}

void TextureComponents::ReleaseAll() {

	for (UINT i = 0; i < 4; i++) {

		texArr[i] = nullptr;
	}

	for (UINT i = 0; i < 2; i++) {

		samplerArr[i] = nullptr;
	}

	SAFE_RELEASE(platformGrass);
	SAFE_RELEASE(platformStone);
	SAFE_RELEASE(platformStoneCracks);

	SAFE_RELEASE(fortressResource);
	SAFE_RELEASE(iceEnemyResource);
	SAFE_RELEASE(lavaEnemyResource);
	SAFE_RELEASE(playerResource);

	SAFE_RELEASE(LavaResource);

	SAFE_RELEASE(projectileResource);
	SAFE_RELEASE(texSampler);
	SAFE_RELEASE(blendState);
	
	SAFE_RELEASE(shadowSampler);
	SAFE_RELEASE(shadowSRV);
	SAFE_RELEASE(shadowDepthView);
	SAFE_RELEASE(ShadowMap);

	SAFE_RELEASE(HUDResource);
	SAFE_RELEASE(HUDPortrait);
	SAFE_RELEASE(HUDHealth);

	for (UINT i = 0; i < 9; i++)
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

	setDebugName(texSampler, "STANDARD_TEXTURE_SAMPLER");

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;//D3D11_BLEND_SRC1_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;//D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = gDevice->CreateBlendState(&blendDesc, &blendState);

	if (FAILED(hr)) {

		return false;
	}

	setDebugName(blendState, "BLEND_STATE");

	ID3D11Texture2D* texture = nullptr;

	CoInitialize(NULL); 
	CreateWICTextureFromFile(gDevice, NULL, L"Format\\Textures\\platformTexture.png", NULL, &platformGrass, 1024);
	CreateWICTextureFromFile(gDevice, NULL, L"Format\\Textures\\platformTextureStone.png", NULL, &platformStone, 1024);
	CreateWICTextureFromFile(gDevice, NULL, L"Format\\Textures\\platformTextureStoneCracks.png", NULL, &platformStoneCracks, 1024);
	CreateWICTextureFromFile(gDevice, NULL, L"Format\\Textures\\file1.png", NULL, &fortressResource, 1024);
	CreateWICTextureFromFile(gDevice, NULL, L"Format\\Textures\\playerTexture.png", NULL, &playerResource, 1024);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\small.jpg", NULL, &iceEnemyResource, 1024);
	CreateWICTextureFromFile(gDevice, NULL, L"Format\\Textures\\LavaEnemyTexture.png", NULL, &lavaEnemyResource, 1024);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\Lava1.jpg", NULL, &LavaResource, 1024);
	CreateWICTextureFromFile(gDevice, NULL, L"Format\\Textures\\ProjectileTexture.png", NULL, &projectileResource, 512);
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
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\HP_frame.png", NULL, &HUDPortrait, 300);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\HP_rect.png", NULL, &HUDHealth, 213);

	if (SUCCEEDED(hr) && texture != 0) {

		gDevice->CreateShaderResourceView(texture, nullptr, &platformGrass);
		setDebugName(platformGrass, "PLATFORM_GRASS_TEXTURE");

		gDevice->CreateShaderResourceView(texture, nullptr, &platformStone);
		setDebugName(platformStone, "PLATFORM_STONE_TEXTURE");

		gDevice->CreateShaderResourceView(texture, nullptr, &platformStoneCracks);
		setDebugName(platformStoneCracks, "PLATFORM_STONE_CRACKS_TEXTURE");

		gDevice->CreateShaderResourceView(texture, nullptr, &fortressResource);
		setDebugName(fortressResource, "FORTRESS_TEXTURE");

		gDevice->CreateShaderResourceView(texture, nullptr, &playerResource);
		setDebugName(playerResource, "PLAYER_TEXTURE");

		gDevice->CreateShaderResourceView(texture, nullptr, &iceEnemyResource);
		setDebugName(iceEnemyResource, "ICE_ENEMY_TEXTURE");

		gDevice->CreateShaderResourceView(texture, nullptr, &lavaEnemyResource);
		setDebugName(lavaEnemyResource, "LAVA_ENEMY_TEXTURE");

		gDevice->CreateShaderResourceView(texture, nullptr, &LavaResource);
		setDebugName(LavaResource, "LAVA_PIT_TEXTURE");

		gDevice->CreateShaderResourceView(texture, nullptr, &projectileResource);
		setDebugName(projectileResource, "PROJECTILE_TEXTURE");

		for(UINT i = 0; i < 9; i++){

			gDevice->CreateShaderResourceView(texture, nullptr, &menuResources[i]);
			setDebugName(menuResources[i], "MENU_TEXTURE " + i);

		}


		gDevice->CreateShaderResourceView(texture, nullptr, &HUDResource);
		setDebugName(HUDResource, "HUD_TEXTURE");

		gDevice->CreateShaderResourceView(texture, nullptr, &HUDPortrait);
		setDebugName(HUDPortrait, "HUD_PORTRAIT_TEXTURE");

		gDevice->CreateShaderResourceView(texture, nullptr, &HUDHealth);
		setDebugName(HUDHealth, "HUD_HEALTH_TEXTURE");

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
	shadowSamp.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;//D3D11_FILTER_MIN_MAG_MIP_POINT;
	shadowSamp.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	shadowSamp.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	shadowSamp.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	shadowSamp.ComparisonFunc = D3D11_COMPARISON_NEVER;
	shadowSamp.MinLOD = 0;
	shadowSamp.MaxLOD = D3D11_FLOAT32_MAX;

	
	hr = gDevice->CreateSamplerState(&shadowSamp, &this->shadowSampler);
	if (FAILED(hr))
	{
		return false;
	}

	setDebugName(shadowSampler, "SHADOW_SAMPLER");

	//Shadow map texture desc
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = WIDTH;
	texDesc.Height = HEIGHT;
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

	setDebugName(ShadowMap, "SHADOW_TEXTURE");
	
	//Depth stencil view description
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = gDevice->CreateDepthStencilView(this->ShadowMap, &descDSV, &this->shadowDepthView);
	if (FAILED(hr))
	{
		return false;
	}

	setDebugName(shadowDepthView, "SHADOW_DEPTH_VIEW");

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

	setDebugName(shadowSRV, "SHADOW_RESOURCE_VIEW");

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