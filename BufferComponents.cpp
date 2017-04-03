
#include "BufferComponents.h"

BufferComponents::BufferComponents() {

	gVertexBuffer = nullptr;	
	gConstantBuffer = nullptr;	
}

BufferComponents::~BufferComponents() {

}

void BufferComponents::ReleaseAll() {

	SAFE_RELEASE(gVertexBuffer);
	SAFE_RELEASE(gConstantBuffer);
	SAFE_RELEASE(depthStencil);
	SAFE_RELEASE(depthView);
	SAFE_RELEASE(depthState);

}

void BufferComponents::SetupScene(ID3D11Device* &gDevice, Camera &mCam) {

	CreateVertexBuffer(gDevice);
	CreateConstantBuffer(gDevice, mCam);

}

bool BufferComponents::CreateVertexBuffer(ID3D11Device* &gDevice) {

	HRESULT hr;

	TriangleVertex triangleVertices[6] =
	{

		-0.5f, -0.5f, 5.0f,	//v1 position	(LEFT BOTTOM)
		0.0f, 1.0f,	//v1 uv coordinates

		-0.5f, 0.5f, 5.0f,	//v2 position	(LEFT TOP)
		0.0f, 0.0f,	//v2 uv coordinates

		0.5f, 0.5f, 5.0f, //v3 position	(RIGHT TOP)
		1.0f, 0.0f,	//v3 uv coordinates

		-0.5f, -0.5f, 5.0f,	//v4 pos position	(LEFT BOTTOM)
		0.0f, 1.0f,	//v4 uv coordinates

		0.5f, 0.5f, 5.0f,	//v5 position	(RIGHT TOP)
		1.0f, 0.0f,	//v5 uv coordinates

		0.5f, -0.5f, 5.0f,  //v6 position	(RIGHT BOTTOM)
		1.0f, 1.0f    //v6 uv coordinates
	};

	

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	hr = gDevice->CreateBuffer(&bufferDesc, &data, &gVertexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	//----------CUBE-------------------------
	//TriangleVertex cubeVertices[24] =
	//{
	//	//Front face
	//	-0.5f,  0.5f, -0.5f, 0.0f, 0.0f,
	//	 0.5f,  0.5f, -0.5f, 1.0f, 0.0f,
	//	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	//	 0.5,  -0.5f, -0.5f, 1.0f, 1.0f,

	//	// Back face
	// 	 0.5f,  0.5f, 0.5f, 0.0f, 0.0f,
	//	-0.5f,  0.5f, 0.5f, 1.0f, 0.0f,
	//	 0.5f, -0.5f, 0.5f, 0.0f, 1.0f,
	//	-0.5,  -0.5f, 0.5f, 1.0f, 1.0f,

	//	// Left face
	//	-0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
	//	-0.5f,  0.5f, -0.5f, 1.0f, 0.0f,
	//	-0.5f, -0.5f,  0.5f, 0.0f, 1.0f,
	//	-0.5f, -0.5f, -0.5f, 1.0f, 1.0f,

	//	// Right face
	//	0.5f,  0.5f, -0.5f, 0.0f, 0.0f,
	//	0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
	//	0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	//	0.5f, -0.5f,  0.5f, 1.0f, 1.0f,

	//	// Top face
	//	-0.5f, 0.5f,  0.5f, 0.0f, 0.0f,
	//	 0.5f, 0.5f,  0.5f, 1.0f, 0.0f,
	//	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
	//	 0.5f, 0.5f, -0.5f, 1.0f, 1.0f,

	//	// Bottom face
	//	 0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
	//	-0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
	//	 0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	//	-0.5f, -0.5f, -0.5f, 1.0f, 1.0f
	//};
	//ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	//memset(&bufferDesc, 0, sizeof(bufferDesc));
	//bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.ByteWidth = sizeof(cubeVertices);

	//ZeroMemory(&data, sizeof(data));
	//data.pSysMem = cubeVertices;
	//hr = gDevice->CreateBuffer(&bufferDesc, &data, &cubeVertexBuffer);

	//if (FAILED(hr)) {

	//	return false;
	//}

	//// Create Indices
	//unsigned int indices[36] = {

	//	// Front face
	//	0,1,2,
	//	2,1,3,

	//	// Back face

	//	4,5,6,
	//	6,5,7,

	//	// Left face

	//	8,9,10,
	//	10,9,11,

	//	// Right face

	//	12,13,14,
	//	14,13,15,

	//	// Top face

	//	16,17,18,
	//	18,17,19,

	//	// Bottom face

	//	20,21,22,
	//	22,21,23 };

	//// Create the buffer description
	//ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.ByteWidth = sizeof(indices);
	//bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//bufferDesc.CPUAccessFlags = 0;
	//bufferDesc.MiscFlags = 0;

	//// Set the subresource data

	//D3D11_SUBRESOURCE_DATA initData;
	//initData.pSysMem = indices;
	//initData.SysMemPitch = 0;
	//initData.SysMemSlicePitch = 0;

	//// Create the buffer

	//hr = gDevice->CreateBuffer(&bufferDesc, &initData, &cubeIndexBuffer);

	//if (FAILED(hr)) {

	//	return false;
	//}

	return true;
}

bool BufferComponents::CreateConstantBuffer(ID3D11Device* &gDevice, Camera &mCam) {	// Function to create the constant buffer

	HRESULT hr;

	// Below I define the constant data used to communicate with the shaders

	//----------------------------------------------------------------------------------------------------------------------------------//

	// The world matrix is responsible for determining the position and orientation of an object in 3D space. This should only represent rotation around the Y-axis.
	// Since we know cos(0) returns 1, the world matrix should resemble an identity matrix
	XMMATRIX worldMatrix = XMMatrixIdentity();
	//----------------------------------------------------------------------------------------------------------------------------------//

	// The view matrix is used to to transform an object's vertices from world space to view space, written in Row major.
	// Using the following method, the matrix can be computed from the world position of the camera (eye), a global up vector, and a 
	// target point.

	DirectX::XMVECTOR eyePos = DirectX::XMLoadFloat3(&XMFLOAT3(0, 0, 4));
	DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&XMFLOAT3(0, 0, 1));
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&XMFLOAT3(0, 1, 0));
	
	XMMATRIX viewMatrix = XMMatrixLookAtLH(eyePos, lookAt, up);
	mCam.LookAt(eyePos, lookAt, up);

	//----------------------------------------------------------------------------------------------------------------------------------//

	// The projection matrix is the actual camera we are looking through when viewing the world with its far and near clipping plane.
	// It's important to create this since we need it to link between view space and clip space

	float fov = PI * 0.45f;		// We recieve the field of view in radians by multiplying with PI

	float aspectRatio = float(WIDTH) / HEIGHT;		// Using the already defined macros for the width and height of the viewport

	float nearPlane = NEARPLANE;

	float farPlane = FARPLANE;

	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
	mCam.SetLens(fov, aspectRatio, nearPlane, farPlane);

	//----------------------------------------------------------------------------------------------------------------------------------//

	// Final calculation for the transform matrix and the transpose function rearranging it to "Column Major" before being sent to the GPU
	// (Required for the HLSL to read it correctly, doesn't accept matrices written in "Row Major"

	XMMATRIX finalCalculation = worldMatrix * viewMatrix * projectionMatrix;
	XMMATRIX tWorld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tWorldViewProj = XMMatrixTranspose(finalCalculation);
	transformMatrix = tWorldViewProj;
	tWorldMatrix = tWorld;

	//----------------------------------------------------------------------------------------------------------------------------------//

	// Here we supply the constant buffer data

	GS_CONSTANT_BUFFER GsConstData;

	GsConstData.matrixWorld = { tWorldMatrix };
	GsConstData.matrixView = { XMMatrixTranspose(viewMatrix) };
	GsConstData.matrixProjection = { XMMatrixTranspose(projectionMatrix) };
	GsConstData.worldViewProj = { tWorldViewProj };
	GsConstData.cameraPos = XMFLOAT3(0.0f, 0.0f, 2.0f);

	// The buffer description is filled in below, mainly so the graphic card understand the structure of it

	D3D11_BUFFER_DESC constBufferDesc;
	ZeroMemory(&constBufferDesc, sizeof(constBufferDesc));
	constBufferDesc.ByteWidth = sizeof(GS_CONSTANT_BUFFER);
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc.MiscFlags = 0;
	constBufferDesc.StructureByteStride = 0;

	// We set the the subresource data

	D3D11_SUBRESOURCE_DATA constData;
	constData.pSysMem = &GsConstData;
	constData.SysMemPitch = 0;
	constData.SysMemSlicePitch = 0;

	// Finally after creating description and subresource data, we create the constant buffer

	hr = gDevice->CreateBuffer(&constBufferDesc, &constData, &gConstantBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}