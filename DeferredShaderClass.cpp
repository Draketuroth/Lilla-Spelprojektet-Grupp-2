
#include "DeferredShaderClass.h"

DeferredShaderClass::DeferredShaderClass() {

	d_vertexShader = nullptr;
	d_pixelShader = nullptr;
	d_layout = nullptr;
}

DeferredShaderClass::~DeferredShaderClass() {


}

void DeferredShaderClass::ReleaseAll() {

	SAFE_RELEASE(d_vertexShader);
	SAFE_RELEASE(d_pixelShader);
	SAFE_RELEASE(d_layout);
}

bool DeferredShaderClass::InitializeShader(ID3D11Device* gDevice, HWND hwnd, WCHAR* vsFileName, WCHAR*) {

	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC vertexInputLayout[2];

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		vsFileName,
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}

		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &d_vertexShader);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	vertexInputLayout[0].SemanticName = "POSITION";
	vertexInputLayout[0].SemanticIndex = 0;
	vertexInputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexInputLayout[0].InputSlot = 0;
	vertexInputLayout[0].AlignedByteOffset = 0;
	vertexInputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexInputLayout[0].InstanceDataStepRate = 0;

	vertexInputLayout[1].SemanticName = "TEXCOORD";
	vertexInputLayout[1].SemanticIndex = 0;
	vertexInputLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexInputLayout[1].InputSlot = 0;
	vertexInputLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexInputLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexInputLayout[1].InstanceDataStepRate = 0;

	int inputLayoutSize = sizeof(vertexInputLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputLayout, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &d_layout);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();
}