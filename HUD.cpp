#include "HUD.h"
#include <iostream>
using namespace std;
HUDClass::HUDClass()
{
	if (!font.loadFromFile("Fonts\\RingB.ttf"))
	{
		cout << "Could not load Font." << endl;
	}
	text.setFont(font);
	text.setString("Hello world!");
	text.setCharacterSize(24);
	text.setColor(sf::Color::Red);

}

HUDClass::~HUDClass()
{

}

bool HUDClass::setElementPos(ID3D11Device* &gDevice)
{
	HRESULT hr;
	
	float width = 0, height = 0;
	width = 25.0f / 256;
	height = 32.0f / 256;

	HUDElements Elements[4] =
	{
		// POS				// UV
		-1.0f, 0.8f, 0.0f,	0.0f, height, //Bot left
		-1.0f, 1.0f, 0.0f,	0.0f, 0.0f, //Top left
		-0.7f, 1.0f, 0.0f,	width, 0.0f, //Top right

		-0.7f, 0.8f, 0.0f,	width, height	//Bot right	

		
		


	};


	D3D11_BUFFER_DESC ElementBufferDesc;
	ZeroMemory(&ElementBufferDesc, sizeof(ElementBufferDesc));

	ElementBufferDesc.ByteWidth = sizeof(HUDElements) * 4;
	ElementBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ElementBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ElementBufferDesc.MiscFlags = 0;
	ElementBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = Elements;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&ElementBufferDesc, &vertexData, &gElementVertexBuffer);

	if (FAILED(hr)) {

		return false;
	}
	return true;
}
void HUDClass::ReleaseAll()
{
	SAFE_RELEASE(gElementVertexBuffer);
	SAFE_RELEASE(gElementIndexBuffer);
}

bool HUDClass::CreateIndexBuffer(ID3D11Device* &gDevice)
{
	HRESULT hr;

	int indexBuffer[6] = { 0, 1, 2, 0, 2, 3 };


	D3D11_BUFFER_DESC ElementBufferDesc;
	ZeroMemory(&ElementBufferDesc, sizeof(ElementBufferDesc));

	ElementBufferDesc.ByteWidth = sizeof(int) * 6;
	ElementBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ElementBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ElementBufferDesc.MiscFlags = 0;
	ElementBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = indexBuffer;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&ElementBufferDesc, &vertexData, &gElementIndexBuffer);

	if (FAILED(hr)) {

		return false;
	}
	return true;
}