#include "HUD.h"

HUDClass::HUDClass()
{

}
HUDClass::~HUDClass()
{

}

bool HUDClass::setElementPos(ID3D11Device* &gDevice)
{
	HRESULT hr;

	HUDElements Elements[6] =
	{
		0.0f, 0.0f,
		0.1f, 0.0f,
		0.0f,0.1f,

		0.1f, 0.0f,
		0.0f,0.1f,
		0.1f,0.1f


	};


	D3D11_BUFFER_DESC ElementBufferDesc;
	ZeroMemory(&ElementBufferDesc, sizeof(ElementBufferDesc));

	ElementBufferDesc.ByteWidth = sizeof(HUDElements) * 24;
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

}
void HUDClass::ReleaseAll()
{
	SAFE_RELEASE(gElementVertexBuffer);
}