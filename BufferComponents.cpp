
#include "BufferComponents.h"

BufferComponents::BufferComponents() {

	gConstantBuffer = nullptr;	
	projectionMatrix = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	viewMatrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	eyePosF = { 0, 0, 4 };
	lookAtF = { 0, 1, 0 };
	upF = { 0, 1, 0 };


	eyePos = XMLoadFloat3(&eyePosF);
	lookAt = XMLoadFloat3(&lookAtF);
	up = XMLoadFloat3(&upF);

	nrOfCubes = 0;
}

BufferComponents::~BufferComponents() {

}

void BufferComponents::ReleaseAll() {

	SAFE_RELEASE(gConstantBuffer);
	SAFE_RELEASE(gCubeIndexBuffer);

	SAFE_RELEASE(gPlayerTransformBuffer);

	for (int i = 0; i < nrOfCubes; i++) {

		SAFE_RELEASE(cubeObjects[i].gCubeVertexBuffer);
	}

}

bool BufferComponents::SetupScene(ID3D11Device* &gDevice) {

	if (!CreateCubeVertices(gDevice)) {

		return false;
	}

	if (!CreateCubeIndices(gDevice)) {

		return false;
	}

	if (!CreateConstantBuffer(gDevice)) {

		return false;
	}

	if (!CreatePlayerTransformBuffer(gDevice)) {

		return false;
	}

	return true;

}

bool BufferComponents::CreateCubeVertices(ID3D11Device* &gDevice) {

	//----------------------------------------------------------------------------------------------------------------------------------//
	// INITIALIZE OFFSET VARIABLES
	//----------------------------------------------------------------------------------------------------------------------------------//

	float spacing = 2.3f;

	DrawCubeRow(gDevice, -4.6f, 0.0f, spacing, 6);

	DrawCubeRow(gDevice, -2.3f, 0.0f, spacing, 6);

	DrawCubeRow(gDevice, 0.0f, 0.0f, spacing, 6);

	DrawCubeRow(gDevice, 2.3f, 0.0f, spacing, 6);

	DrawCubeRow(gDevice, 4.6f, 0.0f, spacing, 6);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// RENDER CHECK TEST
	//----------------------------------------------------------------------------------------------------------------------------------//

	return true;
}

bool BufferComponents::CreateCubeIndices(ID3D11Device* &gDevice) {

	HRESULT hr;

	// Create Indices
	unsigned int indices[] = {

		// Front face
		0,1,2,
		2,1,3,

		// Back face

		4,5,6,
		6,5,7,

		// Left face

		8,9,10,
		10,9,11,

		// Right face

		12,13,14,
		14,13,15,

		// Top face

		16,17,18,
		18,17,19,

		// Bottom face

		20,21,22,
		22,21,23 };

	// Create the buffer description
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(unsigned int) * 36;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Set the subresource data

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = indices;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	// Create the buffer

	hr = gDevice->CreateBuffer(&bufferDesc, &initData, &gCubeIndexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

bool BufferComponents::DrawCubeRow(ID3D11Device* &gDevice, float xOffset, float yOffset, float spacing, int cubes) {

	HRESULT hr;

	float xOffsetValue = 0.0f;
	float yOffsetValue = 0.0f;
	float zOffsetValue = 0.0f;

	float offset = 0;

	for (int i = 0; i < cubes; i++) {

		xOffsetValue = xOffset;
		yOffsetValue = 0;
		zOffsetValue = offset;

		//----------------------------------------------------------------------------------------------------------------------------------//
		// HARDCODED VERTICES
		//----------------------------------------------------------------------------------------------------------------------------------//

		TriangleVertex cubeVertices[24] =
		{

			//Front face

			-1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,
			1.0, -1.0f, -1.0f, 1.0f, 1.0f,

			// Back face

			1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
			-1.0, -1.0f, 1.0f, 1.0f, 1.0f,

			// Left face

			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, -1.0f, 1.0f, 1.0f,

			// Right face

			1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			1.0f, -1.0f, -1.0f, 0.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 1.0f, 1.0f,

			// Top face

			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, -1.0f, 1.0f, 1.0f,

			// Bottom face

			1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
			1.0f, -1.0f, -1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, -1.0f, 1.0f, 1.0f


		};

		//----------------------------------------------------------------------------------------------------------------------------------//
		// OFFSET VERTICES
		//----------------------------------------------------------------------------------------------------------------------------------//

		for (int j = 0; j < 24; j++) {

			cubeVertices[j].x += xOffsetValue + spacing;
			cubeVertices[j].y += yOffsetValue;
			cubeVertices[j].z += zOffsetValue + spacing;

		}

		//----------------------------------------------------------------------------------------------------------------------------------//
		// FILL LIST OF VERTICES FOR BOUNDING BOX CREATION
		//----------------------------------------------------------------------------------------------------------------------------------//

		XMFLOAT3 boundingPoints[24];

		for (int k = 0; k < 24; k++) {

			boundingPoints[k].x = cubeVertices[k].x;
			boundingPoints[k].y = cubeVertices[k].y;
			boundingPoints[k].z = cubeVertices[k].z;
		}

		//----------------------------------------------------------------------------------------------------------------------------------//
		// CREATE VERTEX BUFFER
		//----------------------------------------------------------------------------------------------------------------------------------//

		D3D11_BUFFER_DESC bufferDesc;
		memset(&bufferDesc, 0, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(cubeVertices);

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = cubeVertices;
		hr = gDevice->CreateBuffer(&bufferDesc, &data, &cubeObjects[nrOfCubes].gCubeVertexBuffer);

		if (FAILED(hr)) {

			return false;
		}

		//----------------------------------------------------------------------------------------------------------------------------------//
		// TRANSFORM BOUNDING BOX AND INITIALIZE RENDER CHECK BOOLEAN VARIABLE
		//----------------------------------------------------------------------------------------------------------------------------------//

		cubeObjects[i].objectWorldMatrix = XMMatrixIdentity();
		XMMATRIX transform = XMMATRIX(cubeObjects[i].objectWorldMatrix);

		BoundingBox::CreateFromPoints(cubeObjects[nrOfCubes].bbox, 24, boundingPoints, 0);

		cubeObjects[i].bbox.Extents = { 2, 2, 2 };

		cubeObjects[i].bbox.Transform(cubeObjects[nrOfCubes].bbox, transform);

		//----------------------------------------------------------------------------------------------------------------------------------//
		// INITIALIZE RENDER CHECK TO FALSE FOR ALL CUBES
		//----------------------------------------------------------------------------------------------------------------------------------//

		cubeObjects[nrOfCubes].renderCheck = true;
		XMFLOAT3 corners[8];
		cubeObjects[nrOfCubes].bbox.GetCorners(corners);

		offset += spacing;
		
		nrOfCubes++;

	}

	return true;
}

float BufferComponents::RandomNumber(float Minimum, float Maximum) {

	return ((float(rand()) / float(RAND_MAX)) * (Maximum - Minimum)) + Minimum;
}

bool BufferComponents::CreateConstantBuffer(ID3D11Device* &gDevice) {	// Function to create the constant buffer

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

	

	/*XMVECTOR eyePos = XMLoadFloat3(&eyePosF);
	XMVECTOR lookAt = XMLoadFloat3(&lookAtF);
	XMVECTOR up = XMLoadFloat3(&upF);*/
	
	viewMatrix = XMMatrixLookAtLH(eyePos, lookAt, up);

	//----------------------------------------------------------------------------------------------------------------------------------//

	// The projection matrix is the actual camera we are looking through when viewing the world with its far and near clipping plane.
	// It's important to create this since we need it to link between view space and clip space

	float fov = PI * 0.45f;		// We recieve the field of view in radians by multiplying with PI

	float aspectRatio = float(WIDTH) / HEIGHT;		// Using the already defined macros for the width and height of the viewport

	float nearPlane = NEARPLANE;

	float farPlane = FARPLANE;

	projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);

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

bool BufferComponents::CreatePlayerTransformBuffer(ID3D11Device* &gDevice) {

	HRESULT hr;

	PLAYER_TRANSFORM pTransformData;

	pTransformData.matrixW = XMMatrixIdentity();
	pTransformData.matrixWVP = XMMatrixIdentity();

	D3D11_BUFFER_DESC playerBufferDesc;
	ZeroMemory(&playerBufferDesc, sizeof(playerBufferDesc));
	playerBufferDesc.ByteWidth = sizeof(GS_CONSTANT_BUFFER);
	playerBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	playerBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	playerBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	playerBufferDesc.MiscFlags = 0;
	playerBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA constData;
	constData.pSysMem = &pTransformData;
	constData.SysMemPitch = 0;
	constData.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&playerBufferDesc, &constData, &gPlayerTransformBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}