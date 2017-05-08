#ifndef BUFFERCOMPONENTS_H
#define BUFFERCOMPONENTS_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include "MacroDefinitions.h"
#include "VertexType.h"
#include <vector>

#include "BulletComponents.h"
#include "FileImporter.h"

using namespace DirectX;

// We require a combined transformation matrix from all the previously created matrices and a matrix to preserve the world positions throughout the pipeline

struct GS_CONSTANT_BUFFER {

	XMMATRIX worldViewProj;
	XMMATRIX matrixWorld;
	XMMATRIX matrixView;
	XMMATRIX matrixProjection;
	XMMATRIX fortressWorldMatrix;
	XMMATRIX lightViewProj;
	XMFLOAT3 cameraPos;

};

struct PLATFORM_INSTANCE_BUFFER {

	XMMATRIX worldMatrix[400];
};

struct PLAYER_TRANSFORM {

	XMMATRIX matrixW;
	XMMATRIX matrixWVP;
};

struct CubeObjects {

	btRigidBody* rigidBody;
	XMMATRIX worldMatrix;
	bool renderCheck;
	int ID;
	btVector3 startPos;
};

class BufferComponents {

public:

	BufferComponents();
	~BufferComponents();
	void ReleaseAll();

// Storing both the transposed WorldViewProj and WorldMatrix as global variables to easily reach them when performing calculations in the update loop

	XMMATRIX transformMatrix;
	XMMATRIX tWorldMatrix;
	XMMATRIX projectionMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX fortressWorld;
	XMMATRIX tLightViewProj;

	XMVECTOR eyePos;
	XMVECTOR lookAt;
	XMVECTOR up;

	ID3D11Buffer* gConstantBuffer;	// Constant buffer to provide the vertex shader with updated transformation data per frame
	ID3D11Buffer* gInstanceBuffer;
	ID3D11Buffer* gPlayerTransformBuffer;
	ID3D11Buffer* gEnemyTransformBuffer;

	ID3D11Buffer* gCubeVertexBuffer;

	ID3D11Buffer* gDebugVertexBuffer;
	ID3D11Buffer* gDebugIndexBuffer;

	ID3D11Buffer* gFortressBuffer;

	CubeObjects cubeObjects[CUBECAPACITY];
	int nrOfCubes;
	XMFLOAT3 cubeScaling;
	XMFLOAT3 fortressScaling;

	btRigidBody* lavaPitRigidBody;

	bool SetupScene(ID3D11Device* &gDevice, BulletComponents &bulletPhysicsHandler, FileImporter &platFormImporter, FileImporter &fortressImporter);

	bool CreateDebugVertexBuffer(ID3D11Device* &gDevice);
	bool CreatePlatformVertexBuffer(ID3D11Device* &gDevice, FileImporter &importer);
	bool CreateFortressBuffer(ID3D11Device* &gDevice, FileImporter &fortressImporter);
	bool CreatePlatforms(ID3D11Device* &gDevice, BulletComponents &bulletPhysicsHandler);
	bool CreateCubeIndices(ID3D11Device* &gDevice);
	void CreateCollisionPlane(BulletComponents &bulletPhysicsHandler, XMFLOAT3 translation);
	bool DrawCubeRow(ID3D11Device* &gDevice, float xOffset, float yOffset, float spacing, int cubes, BulletComponents &bulletPhysicsHandler);
	void updatePlatformWorldMatrices();

	bool CreateConstantBuffer(ID3D11Device* &gDevice);
	bool CreateInstanceBuffer(ID3D11Device* &gDevice);
	bool CreatePlayerTransformBuffer(ID3D11Device* &gDevice);
	bool CreateEnemyTransformBuffer(ID3D11Device* &gDevice);
	void CreateRigidBodyTags();

	float spaceX;
	float spaceZ;
	float lerpScalar;


	float incrementSpace(float offset);
	float centerPlatformsColls(float offset);
	float centerPlatformsRows(float offset);

	void platformDecension(CubeObjects cube);
	void platformAcension(CubeObjects cube);
	void platformBreaking(CubeObjects cube);



private:

	XMFLOAT3 eyePosF;
	XMFLOAT3 lookAtF;
	XMFLOAT3 upF;
};

#endif BUFFERCOMPONENTS_H
