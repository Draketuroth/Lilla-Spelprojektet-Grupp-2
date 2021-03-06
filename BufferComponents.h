#ifndef BUFFERCOMPONENTS_H
#define BUFFERCOMPONENTS_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include "DebugHelper.h"
#include "VertexType.h"
#include <vector>

#include "BulletComponents.h"
#include "FileImporter.h"
#include "Platform.h"

#include <time.h>

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
	XMFLOAT4 textureFlag[400];
};

struct PLAYER_TRANSFORM {

	XMMATRIX matrixW;
	XMMATRIX matrixWVP;
};

struct ICE_ENEMY_TRANSFORM {

	XMMATRIX matrixW[20];
};

struct LAVA_ENEMY_TRANSFORM {

	XMMATRIX matrixW[10];
};

struct PROJECTILE_TRANSFORM
{
	XMMATRIX worldMatrix[30];
	
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

	ID3D11Buffer* gConstantBuffer; // Released
	ID3D11Buffer* gInstanceBuffer; // Released
	ID3D11Buffer* gPlayerTransformBuffer; // Released

	ID3D11Buffer* gIceEnemyTransformBuffer; // Released
	ID3D11Buffer* gLavaEnemyTransformBuffer; // Released

	ID3D11Buffer* gProjectileTransformBuffer; // Released

	ID3D11Buffer* gCubeVertexBuffer; // Released

	ID3D11Buffer* gDebugVertexBuffer; // Released
	ID3D11Buffer* gDebugIndexBuffer; // Released

	ID3D11Buffer* gFortressBuffer; // Released

	Platform cubeObjects[CUBECAPACITY];
	int randomNumbers[400];

	int nrOfCubes;
	XMFLOAT3 cubeScaling;
	XMFLOAT3 fortressScaling;

	btRigidBody* lavaPitRigidBody;

	bool SetupScene(ID3D11Device* &gDevice, BulletComponents &bulletPhysicsHandler, FileImporter &platFormImporter, FileImporter &fortressImporter, int nrOfIceEnemies, int nrOfLavaEnemies);

	bool CreateDebugVertexBuffer(ID3D11Device* &gDevice);
	bool CreatePlatformVertexBuffer(ID3D11Device* &gDevice, FileImporter &importer);
	bool CreateFortressBuffer(ID3D11Device* &gDevice, FileImporter &fortressImporter);
	bool CreatePlatforms(ID3D11Device* &gDevice, BulletComponents &bulletPhysicsHandler);
	void CreateCollisionPlane(BulletComponents &bulletPhysicsHandler, XMFLOAT3 translation);
	bool DrawCubeRow(ID3D11Device* &gDevice, float xOffset, float yOffset, float spacing, int cubes, BulletComponents &bulletPhysicsHandler);
	void updatePlatformWorldMatrices();

	bool CreateConstantBuffer(ID3D11Device* &gDevice);
	bool CreateInstanceBuffer(ID3D11Device* &gDevice);
	bool CreatePlayerTransformBuffer(ID3D11Device* &gDevice);

	bool CreateIceEnemyTransformBuffer(ID3D11Device* &gDevice, int nrOfEnemies);
	bool CreateLavaEnemyTransformBuffer(ID3D11Device* &gDevice, int nrOfEnemies);

	bool CreateProjectileTransformBuffer(ID3D11Device* &gDevice, int nrOfEnemies);
	void CreateRigidBodyTags();

	float spaceX;
	float spaceZ;

	float incrementSpace(float offset);
	float centerPlatformsColls(float offset);
	float centerPlatformsRows(float offset);

private:

	XMFLOAT3 eyePosF;
	XMFLOAT3 lookAtF;
	XMFLOAT3 upF;
};

#endif BUFFERCOMPONENTS_H
