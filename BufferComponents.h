#ifndef BUFFERCOMPONENTS_H
#define BUFFERCOMPONENTS_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include "MacroDefinitions.h"
#include "Camera.h"
#include "VertexType.h"

using namespace DirectX;

// We require a combined transformation matrix from all the previously created matrices and a matrix to preserve the world positions throughout the pipeline

struct GS_CONSTANT_BUFFER {

	XMMATRIX worldViewProj;
	XMMATRIX matrixWorld;
	XMMATRIX matrixView;
	XMMATRIX matrixProjection;
	XMFLOAT3 cameraPos;

};

struct PLAYER_TRANSFORM {

	XMMATRIX matrixW;
	XMMATRIX matrixWVP;
};

struct CubeObjects {

	XMMATRIX objectWorldMatrix;
	ID3D11Buffer* gCubeVertexBuffer;;
	BoundingBox bbox;
	bool renderCheck;
};


class BufferComponents {

public:

	BufferComponents();
	~BufferComponents();
	void ReleaseAll();

// Storing both the transposed WorldViewProj and WorldMatrix as global variables to easily reach them when performing calculations in the update loop

	XMMATRIX transformMatrix;
	XMMATRIX tWorldMatrix;

	ID3D11Buffer* planeVertexBuffer;	// Vertex buffer
	ID3D11Buffer* gConstantBuffer;	// Constant buffer to provide the vertex shader with updated transformation data per frame
	ID3D11Buffer* gPlayerTransformBuffer;

	ID3D11Buffer* gCylinderBuffer;
	ID3D11Buffer* gCylinderIndexBuffer;
	ID3D11Buffer* gCubeIndexBuffer;

	CubeObjects cubeObjects[CUBECAPACITY];
	int nrOfCubes;

	bool SetupScene(ID3D11Device* &gDevice);

	bool CreatePlaneVertexBuffer(ID3D11Device* &gDevice);
	bool CreateCubeVertices(ID3D11Device* &gDevice);
	bool CreateCubeIndices(ID3D11Device* &gDevice);
	bool DrawCubeRow(ID3D11Device* &gDevice, float xOffset, float yOffset, float spacing, int cubes);
	float RandomNumber(float Minimum, float Maximum);
	bool CreateConstantBuffer(ID3D11Device* &gDevice);
	bool CreatePlayerTransformBuffer(ID3D11Device* &gDevice);

};

#endif BUFFERCOMPONENTS_H
