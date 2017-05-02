
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
	SAFE_RELEASE(gInstanceBuffer);
	
	SAFE_RELEASE(gCubeVertexBuffer);
	SAFE_RELEASE(gCubeIndexBuffer);

	SAFE_RELEASE(gPlayerTransformBuffer);
	SAFE_RELEASE(gEnemyTransformBuffer);
}

bool BufferComponents::SetupScene(ID3D11Device* &gDevice, BulletComponents &bulletPhysicsHandler, FileImporter &importer) {

	if (!CreatePlatformVertexBuffer(gDevice, importer)) {

		return false;
	}

	if (!CreatePlatforms(gDevice, bulletPhysicsHandler)) {

		return false;
	}

	if (!CreateInstanceBuffer(gDevice)) {

		return false;
	}

	if (!CreateCubeIndices(gDevice)) {

		return false;
	}

	CreateCollisionPlane(bulletPhysicsHandler, XMFLOAT3(0, -8, 0));

	if (!CreateConstantBuffer(gDevice)) {

		return false;
	}

	if (!CreatePlayerTransformBuffer(gDevice)) {

		return false;
	}
	if (!CreateEnemyTransformBuffer(gDevice))
	{
		return false;
	}

	return true;

}

bool BufferComponents::CreatePlatformVertexBuffer(ID3D11Device* &gDevice, FileImporter &importer) {

	HRESULT hr;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// HARDCODED VERTICES
	//----------------------------------------------------------------------------------------------------------------------------------//
	/*TriangleVertex cubeVertices[132];
	cubeScaling = importer.standardMeshes[0].meshTransformation.meshScale;
	for (UINT i = 0; i < importer.standardMeshes[0].vertices.size(); i++) {

		cubeVertices[i].x = importer.standardMeshes[0].vertices[i].pos[0];
		cubeVertices[i].y = importer.standardMeshes[0].vertices[i].pos[1];
		cubeVertices[i].z = importer.standardMeshes[0].vertices[i].pos[2];

		cubeVertices[i].u = importer.standardMeshes[0].vertices[i].uv[0];
		cubeVertices[i].v = importer.standardMeshes[0].vertices[i].uv[1];
	}*/

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
	// CREATE VERTEX BUFFER
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(cubeVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = cubeVertices;
	hr = gDevice->CreateBuffer(&bufferDesc, &data, &gCubeVertexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

bool BufferComponents::CreatePlatforms(ID3D11Device* &gDevice, BulletComponents &bulletPhysicsHandler) {

	//----------------------------------------------------------------------------------------------------------------------------------//
	// INITIALIZE OFFSET VARIABLES
	//----------------------------------------------------------------------------------------------------------------------------------//

	float spacing = 2;
	spaceX = centerPlatformsColls(spaceX);
	spaceZ = centerPlatformsRows(spaceZ);
	
	for (int i = 0; i < PLATCOLLS; i++)
	{
		DrawCubeRow(gDevice, spaceX, 0.0f, spaceZ, PLATROWS, bulletPhysicsHandler);
		spaceX = incrementSpace(spaceX);
		
	}
	

	

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

void BufferComponents::CreateCollisionPlane(BulletComponents &bulletPhysicsHandler, XMFLOAT3 translation) {

	btTransform planeTransform;
	planeTransform.setIdentity();
	planeTransform.setOrigin(btVector3(translation.x, translation.y, translation.z));

	btStaticPlaneShape* staticPlaneShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	btMotionState* planeMotion = new btDefaultMotionState(planeTransform);
	btRigidBody::btRigidBodyConstructionInfo planeRigidBodyInfo(0, planeMotion, staticPlaneShape);
	btRigidBody* planeRigidBody = new btRigidBody(planeRigidBodyInfo);

	this->lavaPitRigidBody = planeRigidBody;
	bulletPhysicsHandler.bulletDynamicsWorld->addRigidBody(planeRigidBody);
	bulletPhysicsHandler.rigidBodies.push_back(planeRigidBody);
}

bool BufferComponents::DrawCubeRow(ID3D11Device* &gDevice, float xOffset, float yOffset, float spacing, int cubes, BulletComponents &bulletPhysicsHandler) {

	HRESULT hr;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// INITIALIZE OFFSET VALUES
	//----------------------------------------------------------------------------------------------------------------------------------//

	float xOffsetValue = 0.0f;
	float yOffsetValue = 0.0f;
	float zOffsetValue = 0.0f;

	

	for (int i = 0; i < cubes; i++) {

		cubeObjects[i].ID = i + 1;
		xOffsetValue = xOffset;
		yOffsetValue = 0;
		zOffsetValue = spacing;

		//----------------------------------------------------------------------------------------------------------------------------------//
		// CREATE RIGID BODY
		//----------------------------------------------------------------------------------------------------------------------------------//

		// Platform Rigid Body only uses an identity matrix as its world matrix
		btTransform transform;
		XMFLOAT4X4 platFormWorldMatrix;
		XMMATRIX platformTranslation = XMMatrixTranslation(xOffsetValue, yOffsetValue, zOffsetValue);
		XMStoreFloat4x4(&platFormWorldMatrix, platformTranslation);

		transform.setFromOpenGLMatrix((float*)&platFormWorldMatrix);

		// Define the kind of shape we want and construct rigid body information
		btBoxShape* boxShape = new btBoxShape(btVector3(1, 1, 1));
		btMotionState* motion = new btDefaultMotionState(transform);

		// Definition of the rigid body
		btScalar mass(0.0f);
		btRigidBody::btRigidBodyConstructionInfo info(mass, motion, boxShape);

		// Create the rigid body
		btRigidBody* platformRigidBody = new btRigidBody(info);
		cubeObjects[nrOfCubes].rigidBody = platformRigidBody;

		platformRigidBody->setIslandTag(platformRigid);//Tag to skip rayTesting for character projectiles
		// Add the new rigid body to the dynamic world
		bulletPhysicsHandler.bulletDynamicsWorld->addRigidBody(platformRigidBody);
		bulletPhysicsHandler.rigidBodies.push_back(platformRigidBody);

		cubeObjects[nrOfCubes].worldMatrix = platformTranslation;

		cubeObjects[nrOfCubes].renderCheck = true;


		
		//----------------------------------------------------------------------------------------------------------------------------------//
		// ADD OFFSET FOR THE NEXT PLATFORM
		//----------------------------------------------------------------------------------------------------------------------------------//

		spacing = incrementSpace(spacing);
		
		nrOfCubes++;

	}

	return true;
}

void BufferComponents::updatePlatformWorldMatrices() 
{

	// Prepare matrices for conversion
	XMMATRIX transform;
	XMFLOAT4X4 data;
	XMMATRIX scale = XMMatrixScaling(cubeScaling.x, cubeScaling.y, cubeScaling.z);

	for(int i = 0; i < nrOfCubes; i++)
	{
		XMFLOAT3 pos;
		XMVECTOR t;
		XMVECTOR s;
		XMVECTOR r;
	

		// Gather the rigid body matrix
		btTransform btRigidTransform;
		cubeObjects[i].rigidBody->getMotionState()->getWorldTransform(btRigidTransform);

		// Load it into an XMFLOAT4x4
		btRigidTransform.getOpenGLMatrix((float*)&data);

		// Load it into an XMMATRIX
		transform = XMLoadFloat4x4(&data);

		// Build the new world matrix
		cubeObjects[i].worldMatrix = transform;
		//XMMatrixMultiply(scale, transform)
		

	}

	if (GetAsyncKeyState('L'))
	{
		platformDecension(cubeObjects[50]);
		XMMATRIX transform;
		XMFLOAT4X4 data;
		XMVECTOR t;
		XMVECTOR s;
		XMVECTOR r;
		XMFLOAT3 pos;
		// Gather the rigid body matrix
		btTransform btRigidTransform;
		cubeObjects[50].rigidBody->getMotionState()->getWorldTransform(btRigidTransform);

		// Load it into an XMFLOAT4x4
		btRigidTransform.getOpenGLMatrix((float*)&data);

		// Load it into an XMMATRIX
		transform = XMLoadFloat4x4(&data);

		XMMatrixDecompose(&s, &r, &t, transform);
		XMStoreFloat3(&pos, t);

		// Build the new world matrix
		cout << pos.x << " " << pos.y << " "<< pos.z << endl;
		
	}
	
}

bool BufferComponents::CreateInstanceBuffer(ID3D11Device* &gDevice) {

	HRESULT result;
	PLATFORM_INSTANCE_BUFFER instances;

	// Set the number of instances in the array
	int instanceCount = nrOfCubes;

	// Load the instance array with data
	for (int i = 0; i < instanceCount; i++) {

		instances.worldMatrix[i] = cubeObjects[i].worldMatrix;
	}

	// Setup the instance buffer description
	D3D11_BUFFER_DESC instanceBufferDesc;
	ZeroMemory(&instanceBufferDesc, sizeof(instanceBufferDesc));

	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(PLATFORM_INSTANCE_BUFFER);
	instanceBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	D3D11_SUBRESOURCE_DATA instanceData;

	instanceData.pSysMem = &instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = gDevice->CreateBuffer(&instanceBufferDesc, &instanceData, &gInstanceBuffer);
	
	if (FAILED(result))
	{
		return false;
	}

	return true;
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
	GsConstData.matrixProjection = XMMatrixIdentity();
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

bool BufferComponents::CreateEnemyTransformBuffer(ID3D11Device* &gDevice) {

	HRESULT hr;

	PLAYER_TRANSFORM eTransformData;

	eTransformData.matrixW = XMMatrixIdentity();
	eTransformData.matrixWVP = XMMatrixIdentity();

	D3D11_BUFFER_DESC enemyBufferDesc;
	ZeroMemory(&enemyBufferDesc, sizeof(enemyBufferDesc));
	enemyBufferDesc.ByteWidth = sizeof(GS_CONSTANT_BUFFER);
	enemyBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	enemyBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	enemyBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	enemyBufferDesc.MiscFlags = 0;
	enemyBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA constData;
	constData.pSysMem = &eTransformData;
	constData.SysMemPitch = 0;
	constData.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&enemyBufferDesc, &constData, &gEnemyTransformBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}
void BufferComponents::CreateRigidBodyTags()
{
	for (size_t i = 0; i < this->nrOfCubes; i++)
	{
		this->cubeObjects[i].rigidBody->setIslandTag(platformRigid);
	}
}
float BufferComponents::incrementSpace(float offset)
{
	offset += 2.3;
	return offset;
}
float BufferComponents::centerPlatformsColls(float offset)
{
	offset = PLATCOLLS / 2;
	offset *= 2.3;
	offset *= -1;

	return offset;
}
float BufferComponents::centerPlatformsRows(float offset)
{
	offset = PLATROWS / 2;
	offset *= 2.3;
	offset *= -1;

	return offset;
}
void BufferComponents::platformDecension(CubeObjects cube)
{

	btTransform rigidCube;
	XMFLOAT4X4 data;
	XMMATRIX transform;
	XMFLOAT3 pos;

	XMVECTOR t;
	XMVECTOR s;
	XMVECTOR r;

	cube.rigidBody->getMotionState()->getWorldTransform(rigidCube);
	rigidCube.getOpenGLMatrix((float*)&data);

	transform = XMLoadFloat4x4(&data);
	XMMatrixDecompose(&s, &r, &t, transform);

	XMStoreFloat3(&pos, t);

	btScalar rigidXvalue = pos.x;
	btScalar rigidYvalue = pos.y;
	btScalar rigidZvalue = pos.z;

	btVector3 startPos(rigidXvalue,rigidYvalue,rigidZvalue);
	btVector3 endPos = { rigidXvalue,-11,rigidZvalue };

	btVector3 lerpResult = lerp(startPos, endPos, 0.005f);

	btMatrix3x3 movementMatrix;
	movementMatrix.setIdentity();
	btTransform plat{ movementMatrix,lerpResult };

	
	cube.rigidBody->setCollisionFlags(cube.rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	cube.rigidBody->setActivationState(DISABLE_DEACTIVATION);

	//cube.rigidBody->translate(btVector3(0, 1, 0));
	cube.rigidBody->getMotionState()->setWorldTransform(plat);

}