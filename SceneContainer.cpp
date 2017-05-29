
#include "SceneContainer.h"

SceneContainer::SceneContainer() {

	// Initialize handler and main character

	mainCharacterFile = FileImporter();
	gHandler = GraphicComponents();
	bHandler = BufferComponents();
	tHandler = TextureComponents();

	character = MainCharacter();

	this->nrOfIceEnemies = 2;
	this->nrOfLavaEnemies = 0;
	this->nrOfEnemies = nrOfIceEnemies + nrOfLavaEnemies;

	bulletPhysicsHandler = BulletComponents();

	platformSoundBuffer[0].loadFromFile("Sounds//platformCrumbling.wav");
	platformSoundBuffer[1].loadFromFile("Sounds//rumble.wav");

	this->waveDelay = 10.0f;
	this->level = 1;

	this->ai = AI();

}

SceneContainer::~SceneContainer() {

}

void SceneContainer::releaseAll() {

	character.releaseAll(bulletPhysicsHandler.bulletDynamicsWorld);

	for(UINT i = 0; i < nrOfEnemies; i++){

		enemies[i]->releaseAll(bulletPhysicsHandler.bulletDynamicsWorld);

	}

	SAFE_RELEASE(enemyIceVertexBuffer);
	SAFE_RELEASE(enemyLavaVertexBuffer);

	SAFE_RELEASE(gProjectileBuffer);

	SAFE_RELEASE(ExplosionVertexBuffer);
	SAFE_RELEASE(projectileVertexBuffer);

	deferredObject.ReleaseAll();
	deferredShaders.ReleaseAll();
	lightShaders.ReleaseAll();

	lava.ReleaseAll();
	HUD.ReleaseAll();
	animHandler.ReleaseAll();
	bulletPhysicsHandler.ReleaseAll();

	bHandler.ReleaseAll();
	tHandler.ReleaseAll();

	gHandler.ReleaseAll();

}

bool SceneContainer::initialize(HWND &windowHandle) {

	if (!readFiles()) {

		// If the required files can't be read during startup, quit the application
		MessageBox(
			NULL,
			L"CRITICAL ERROR: Format couldn't be read, please look for format folder in solution\nClosing application...",
			L"ERROR",
			MB_OK);
			PostQuitMessage(0);
	}
	
	if (!WindowInitialize(windowHandle)) {

		// If the window cannot be created during startup, it's more known as a terminal error
		// The MessageBox function will display a message and inform us of the problem
		MessageBox(
			NULL,
			L"CRITICAL ERROR: Window couldn't be initialized, investigate window initializer function\nClosing application...",
			L"ERROR",
			MB_OK);
			PostQuitMessage(0);
	}

	if (!gHandler.InitalizeDirect3DContext(windowHandle)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: DirectX couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
			PostQuitMessage(0);
	}

	bulletPhysicsHandler.InitializeBulletPhysics();

	if (!bHandler.SetupScene(gHandler.gDevice, bulletPhysicsHandler, PlatformFile, FortressFile, this->nrOfIceEnemies, this->nrOfLavaEnemies)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Primitives couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
			PostQuitMessage(0);
	}

	if (!tHandler.CreateTexture(gHandler.gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Textures couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
			PostQuitMessage(0);
	}
	if (!tHandler.CreateShadowMap(gHandler.gDevice))
	{
		MessageBox(
			NULL,
			L"CRITICAL ERROR: Shadow Map couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
		PostQuitMessage(0);
	}

	if (!deferredObject.Initialize(gHandler.gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Deferred buffer couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
			PostQuitMessage(0);
	}

	if (!deferredShaders.InitializeShader(gHandler.gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Deferred shader couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
			PostQuitMessage(0);
	}

	if (!lightShaders.Initialize(gHandler.gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Light shaders couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
			PostQuitMessage(0);
	}

	character.initialize(gHandler.gDevice, XMFLOAT3(2, 2, 5), bulletPhysicsHandler, animHandler, mainCharacterFile, getRadiusCharacter(), getHeightCharacter());
	
	HUD.setElementPos(gHandler.gDevice, character.getHealth());
	HUD.CreateIndexBuffer(gHandler.gDevice);
	HUD.loadBitMap();
	HUD.setText(level);
	HUD.setFont(gHandler.gDevice);
	HUD.CreateFontIndexBuffer(gHandler.gDevice);
	
	InitializeEnemies(gHandler.gDevice, bulletPhysicsHandler);

	createSideBoundingBoxes();
	
	return true;

}

void SceneContainer::InitializeEnemies(ID3D11Device* graphicDevice, BulletComponents &bulletPhysicsHandle) {

	enemies.clear();
	enemies.resize(nrOfEnemies);

	XMFLOAT3 initSpawnPos;

	for (UINT i = 0; i < nrOfEnemies; i++) {

		initSpawnPos.x = RandomNumber(-15, 15);
		initSpawnPos.y = 2;
		initSpawnPos.z = RandomNumber(-15, 15);

		if (i < nrOfIceEnemies){

		enemies[i] = new Enemy(0, { initSpawnPos.x, initSpawnPos.y, initSpawnPos.z });
		enemies[i]->Spawn(gHandler.gDevice, bulletPhysicsHandler, i, getRadiusIce(), getHeightIce());

		}

		else if (i >= nrOfIceEnemies) {

			enemies[i] = new Enemy(1, { initSpawnPos.x, initSpawnPos.y, initSpawnPos.z });
			enemies[i]->Spawn(gHandler.gDevice, bulletPhysicsHandler, i, getRadiusLava(), getHeightLava());
			enemies[i]->createProjectile(bulletPhysicsHandler);

		}

	}

	// Create the projectile vertex and index buffer all enemies will share
	createProjectileBox(gHandler.gDevice);

	// Load the enemy vertices 
	loadIceEnemyVertices(iceEnemyFile, graphicDevice);
	loadLavaEnemyVertices(lavaEnemyFile, graphicDevice);

	// Create the vertex buffer all the ice enemies will share
	createLavaEnemyBuffer(gHandler.gDevice);
	createIceEnemyBuffer(gHandler.gDevice);

	// Create the enemy bone buffers
	createIceEnemyBoneBuffer(graphicDevice, iceEnemySkinData);
	createLavaEnemyBoneBuffer(graphicDevice, lavaEnemySkinData);

}

void SceneContainer::RespawnEnemies() {

	// Remove enemy rigid bodies
	for (UINT i = 0; i < nrOfIceEnemies; i++)
	{
		if(enemies[i]->rigidBody->isInWorld()){

			// Remove ice enemy rigid body
			bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(enemies[i]->rigidBody);
			btMotionState* enemyMotion = enemies[i]->rigidBody->getMotionState();
			btCollisionShape* enemyShape = enemies[i]->rigidBody->getCollisionShape();
			delete enemies[i]->rigidBody;
			delete enemyMotion;
			delete enemyShape;

		}
	}

	for (UINT i = nrOfIceEnemies; i < nrOfEnemies; i++) {

		if (enemies[i]->rigidBody->isInWorld()) {
			// Remove lava enemy rigid body
			bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(enemies[i]->rigidBody);
			btMotionState* enemyMotion = enemies[i]->rigidBody->getMotionState();
			btCollisionShape* enemyShape = enemies[i]->rigidBody->getCollisionShape();
			delete enemies[i]->rigidBody;
			delete enemyMotion;
			delete enemyShape;

		}

		if(enemies[i]->fireBall.projectileRigidBody->isInWorld()){

			// Remove lava enemy projectile rigid body
			bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(enemies[i]->fireBall.projectileRigidBody);
			btMotionState* projectileMotion = enemies[i]->fireBall.projectileRigidBody->getMotionState();
			btCollisionShape* projectileShape = enemies[i]->fireBall.projectileRigidBody->getCollisionShape();
			delete enemies[i]->fireBall.projectileRigidBody;
			delete projectileMotion;
			delete projectileShape;

		}
		
	}

	// Reset platform hit flag
	for (UINT i = 0; i < bHandler.nrOfCubes; i++) {

		bHandler.cubeObjects[i].Hit = false;
		bHandler.cubeObjects[i].Damaged = false;
		bHandler.cubeObjects[i].descensionTimer = 0;
		bHandler.cubeObjects[i].breakTimer = 0;
		bHandler.cubeObjects[i].ascensionTimer = 0;
		bHandler.cubeObjects[i].rigidBody->setWorldTransform(bHandler.cubeObjects[i].originMatrix);
		
	}

	// Clear enemy rigid bodies vector
	bulletPhysicsHandler.enemyRigidBodies.clear();
	bulletPhysicsHandler.projectileRigidBodies.clear();

	// Recreate enemies and their rigid bodies

	XMFLOAT3 initSpawnPos;

	if (nrOfEnemies % 2 == 0 && nrOfLavaEnemies < 15 || nrOfIceEnemies == 15 && nrOfLavaEnemies < 15)
	{
		nrOfLavaEnemies++;
		nrOfEnemies++;
	}
	else if( nrOfIceEnemies < 15)
	{
		nrOfIceEnemies++;
		nrOfEnemies++;
	}

	enemies.clear();
	enemies.resize(nrOfEnemies);

	for (UINT i = 0; i < nrOfEnemies; i++) {

		initSpawnPos.x = RandomNumber(-15, 15);
		initSpawnPos.y = 2;
		initSpawnPos.z = RandomNumber(-15, 15);

		if (i < nrOfIceEnemies) {

			enemies[i] = new Enemy(0, { initSpawnPos.x, initSpawnPos.y, initSpawnPos.z });
			enemies[i]->Spawn(gHandler.gDevice, bulletPhysicsHandler, i, getRadiusIce(), getHeightIce());

		}

		else if (i >= nrOfIceEnemies) {

			enemies[i] = new Enemy(1, { initSpawnPos.x, initSpawnPos.y, initSpawnPos.z });
			enemies[i]->Spawn(gHandler.gDevice, bulletPhysicsHandler, i, getRadiusLava(), getHeightLava());
			enemies[i]->createProjectile(bulletPhysicsHandler);

		}

	}
	

}

bool SceneContainer::createProjectileBox(ID3D11Device* gDevice)
{
	HRESULT hr;

	/*for (UINT i = 0; i < projectileFile.standardMeshes[0].vertices.size(); i++)
	{

		StandardVertex vertex;

		vertex.x = projectileFile.standardMeshes[0].vertices[i].pos[0];
		vertex.y = projectileFile.standardMeshes[0].vertices[i].pos[1];
		vertex.z = projectileFile.standardMeshes[0].vertices[i].pos[2];

		vertex.u = projectileFile.standardMeshes[0].vertices[i].uv[0];
		vertex.v = projectileFile.standardMeshes[0].vertices[i].uv[1];

		vertex.nx = projectileFile.standardMeshes[0].vertices[i].normal[0];
		vertex.ny = projectileFile.standardMeshes[0].vertices[i].normal[1];
		vertex.nz = projectileFile.standardMeshes[0].vertices[i].normal[2];

		ExplosionVertices.push_back(vertex);
	}*/

	for (UINT i = 0; i < projectileFile.standardMeshes[1].vertices.size(); i++)
	{

		StandardVertex vertex;

		vertex.x = projectileFile.standardMeshes[1].vertices[i].pos[0];
		vertex.y = projectileFile.standardMeshes[1].vertices[i].pos[1];
		vertex.z = projectileFile.standardMeshes[1].vertices[i].pos[2];

		vertex.u = projectileFile.standardMeshes[1].vertices[i].uv[0];
		vertex.v = projectileFile.standardMeshes[1].vertices[i].uv[1];

		vertex.nx = projectileFile.standardMeshes[1].vertices[i].normal[0];
		vertex.ny = projectileFile.standardMeshes[1].vertices[i].normal[1];
		vertex.nz = projectileFile.standardMeshes[1].vertices[i].normal[2];

		projectileVertices.push_back(vertex);
	}
	

	
/*
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(StandardVertex) * ExplosionVertices.size();

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = &ExplosionVertices[0];
	hr = gDevice->CreateBuffer(&bufferDesc, &data, &ExplosionVertexBuffer);

	if (FAILED(hr)) {

		return false;
	}*/


	D3D11_BUFFER_DESC bufferDesc2;
	ZeroMemory(&bufferDesc2, sizeof(bufferDesc2));

	memset(&bufferDesc2, 0, sizeof(bufferDesc2));
	bufferDesc2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc2.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc2.ByteWidth = sizeof(StandardVertex) * projectileVertices.size();

	D3D11_SUBRESOURCE_DATA data2;
	ZeroMemory(&data2, sizeof(data2));
	data2.pSysMem = &projectileVertices[0];
	hr = gDevice->CreateBuffer(&bufferDesc2, &data2, &projectileVertexBuffer);

	if (FAILED(hr)) 
	{

		return false;
	}

	

	return true;
}

void SceneContainer::loadIceEnemyVertices(FileImporter &importer, ID3D11Device* &graphicDevice)
{

	HRESULT hr;
	//load mesh vertices
	for (UINT i = 0; i < importer.skinnedMeshes[0].vertices.size(); i++) 
	{

		Vertex_Bone vertex;

		vertex.pos.x = importer.skinnedMeshes[0].vertices[i].pos[0];
		vertex.pos.y = importer.skinnedMeshes[0].vertices[i].pos[1];
		vertex.pos.z = importer.skinnedMeshes[0].vertices[i].pos[2];

		vertex.uv.x = importer.skinnedMeshes[0].vertices[i].uv[0];
		vertex.uv.y = importer.skinnedMeshes[0].vertices[i].uv[1];

		vertex.normal.x = importer.skinnedMeshes[0].vertices[i].normal[0];
		vertex.normal.y = importer.skinnedMeshes[0].vertices[i].normal[1];
		vertex.normal.z = importer.skinnedMeshes[0].vertices[i].normal[2];

		vertex.boneIndices[0] = importer.skinnedMeshes[0].vertices[i].boneIndices[0];
		vertex.boneIndices[1] = importer.skinnedMeshes[0].vertices[i].boneIndices[1];
		vertex.boneIndices[2] = importer.skinnedMeshes[0].vertices[i].boneIndices[2];
		vertex.boneIndices[3] = importer.skinnedMeshes[0].vertices[i].boneIndices[3];

		vertex.weights[0] = importer.skinnedMeshes[0].vertices[i].weights[0];
		vertex.weights[1] = importer.skinnedMeshes[0].vertices[i].weights[1];
		vertex.weights[2] = importer.skinnedMeshes[0].vertices[i].weights[2];
		vertex.weights[3] = importer.skinnedMeshes[0].vertices[i].weights[3];

		iceEnemyVertices.push_back(vertex);

	}
}

void SceneContainer::loadLavaEnemyVertices(FileImporter &importer, ID3D11Device* &graphicDevice) {

	HRESULT hr;
	//load mesh vertices
	for (UINT i = 0; i < importer.skinnedMeshes[0].vertices.size(); i++) {

		Vertex_Bone vertex;

		vertex.pos.x = importer.skinnedMeshes[0].vertices[i].pos[0];
		vertex.pos.y = importer.skinnedMeshes[0].vertices[i].pos[1];
		vertex.pos.z = importer.skinnedMeshes[0].vertices[i].pos[2];

		vertex.uv.x = importer.skinnedMeshes[0].vertices[i].uv[0];
		vertex.uv.y = importer.skinnedMeshes[0].vertices[i].uv[1];

		vertex.normal.x = importer.skinnedMeshes[0].vertices[i].normal[0];
		vertex.normal.y = importer.skinnedMeshes[0].vertices[i].normal[1];
		vertex.normal.z = importer.skinnedMeshes[0].vertices[i].normal[2];

		vertex.boneIndices[0] = importer.skinnedMeshes[0].vertices[i].boneIndices[0];
		vertex.boneIndices[1] = importer.skinnedMeshes[0].vertices[i].boneIndices[1];
		vertex.boneIndices[2] = importer.skinnedMeshes[0].vertices[i].boneIndices[2];
		vertex.boneIndices[3] = importer.skinnedMeshes[0].vertices[i].boneIndices[3];

		vertex.weights[0] = importer.skinnedMeshes[0].vertices[i].weights[0];
		vertex.weights[1] = importer.skinnedMeshes[0].vertices[i].weights[1];
		vertex.weights[2] = importer.skinnedMeshes[0].vertices[i].weights[2];
		vertex.weights[3] = importer.skinnedMeshes[0].vertices[i].weights[3];

		lavaEnemyVertices.push_back(vertex);

	}
}

bool SceneContainer::createIceEnemyBuffer(ID3D11Device* &graphicDevice) {

	HRESULT hr;

	//----------------------------------------------------------------------//
	// VERTEX BUFFER
	//----------------------------------------------------------------------//

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex_Bone) * iceEnemyVertices.size();

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = &iceEnemyVertices[0];
	hr = graphicDevice->CreateBuffer(&bufferDesc, &data, &enemyIceVertexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;

}

bool SceneContainer::createLavaEnemyBuffer(ID3D11Device* &graphicDevice) {

	HRESULT hr;

	//----------------------------------------------------------------------//
	// VERTEX BUFFER
	//----------------------------------------------------------------------//

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex_Bone) * lavaEnemyVertices.size();

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = &lavaEnemyVertices[0];
	hr = graphicDevice->CreateBuffer(&bufferDesc, &data, &enemyLavaVertexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

bool SceneContainer::createIceEnemyBoneBuffer(ID3D11Device* &graphicDevice, ICE_ENEMY_SKINNED_DATA &skinData) {

	HRESULT hr;

	for (UINT i = 0; i < nrOfIceEnemies; i++) {

		for (UINT j = 0; j < 24; j++) {

			XMStoreFloat4x4(&skinData.enemyInstance[i].gBoneTransform[j], XMMatrixIdentity());
		}
	}

	//----------------------------------------------------------------------------------------------------------------------------------//
	// ICE ENEMY BONE BUFFER DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_BUFFER_DESC boneBufferDesc;

	memset(&boneBufferDesc, 0, sizeof(boneBufferDesc));

	boneBufferDesc.ByteWidth = sizeof(ICE_ENEMY_SKINNED_DATA);
	boneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	boneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	boneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	boneBufferDesc.MiscFlags = 0;
	boneBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA boneData;
	boneData.pSysMem = &skinData;
	boneData.SysMemPitch = 0;
	boneData.SysMemSlicePitch = 0;

	hr = graphicDevice->CreateBuffer(&boneBufferDesc, &boneData, &animHandler.gIceEnemyBoneBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

bool SceneContainer::createLavaEnemyBoneBuffer(ID3D11Device* &graphicDevice, LAVA_ENEMY_SKINNED_DATA &skinData) {

	HRESULT hr;

	for (UINT i = 0; i < nrOfLavaEnemies; i++) {

		for (UINT j = 0; j < 16; j++) {

			XMStoreFloat4x4(&skinData.enemyInstance[i].gBoneTransform[j], XMMatrixIdentity());
		}
	}

	//----------------------------------------------------------------------------------------------------------------------------------//
	// LAVA ENEMY BONE BUFFER DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_BUFFER_DESC boneBufferDesc;

	memset(&boneBufferDesc, 0, sizeof(boneBufferDesc));

	boneBufferDesc.ByteWidth = sizeof(LAVA_ENEMY_SKINNED_DATA);
	boneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	boneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	boneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	boneBufferDesc.MiscFlags = 0;
	boneBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA boneData;
	boneData.pSysMem = &skinData;
	boneData.SysMemPitch = 0;
	boneData.SysMemSlicePitch = 0;

	hr = graphicDevice->CreateBuffer(&boneBufferDesc, &boneData, &animHandler.gLavaEnemyBoneBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

void SceneContainer::reportLiveObjects() {

#ifdef _DEBUG

	ID3D11Debug* DebugDevice = nullptr;
	HRESULT result = gHandler.gDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&DebugDevice));

	result = DebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

	SAFE_RELEASE(DebugDevice);

#endif
}

float SceneContainer::RandomNumber(float Minimum, float Maximum) {

	return ((float(rand()) / float(RAND_MAX)) * (Maximum - Minimum)) + Minimum;
}

void SceneContainer::ReRelease() {

	//----------------------------------------------------------------------------------------------------------------------------------//
	// RIGID BODIES
	//----------------------------------------------------------------------------------------------------------------------------------//

	// Remove player rigid body
	bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(character.rigidBody);
	btMotionState* playerMotion = character.rigidBody->getMotionState();
	btCollisionShape* playerShape = character.rigidBody->getCollisionShape();
	delete character.rigidBody;
	delete playerMotion;
	delete playerShape;

	for (UINT i = 0; i < nrOfIceEnemies; i++) 
	{
		if(enemies[i]->rigidBody->isInWorld()){

			// Remove ice enemy rigid body
			bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(enemies[i]->rigidBody);
			btMotionState* enemyMotion = enemies[i]->rigidBody->getMotionState();
			btCollisionShape* enemyShape = enemies[i]->rigidBody->getCollisionShape();
			delete enemies[i]->rigidBody;
			delete enemyMotion;
			delete enemyShape;

		}

	}

	for (UINT i = nrOfIceEnemies; i < nrOfEnemies; i++) {

		if (enemies[i]->rigidBody->isInWorld()) {

			// Remove lava enemy enemy rigid body
			bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(enemies[i]->rigidBody);
			btMotionState* enemyMotion = enemies[i]->rigidBody->getMotionState();
			btCollisionShape* enemyShape = enemies[i]->rigidBody->getCollisionShape();
			delete enemies[i]->rigidBody;
			delete enemyMotion;
			delete enemyShape;

		}

		if(enemies[i]->fireBall.projectileRigidBody->isInWorld()) {

			// Remove projectile rigid body
			bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(enemies[i]->fireBall.projectileRigidBody);
			btMotionState* projectileMotion = enemies[i]->fireBall.projectileRigidBody->getMotionState();
			btCollisionShape* projectileShape = enemies[i]->fireBall.projectileRigidBody->getCollisionShape();
			delete enemies[i]->fireBall.projectileRigidBody;
			delete projectileMotion;
			delete projectileShape;

		}
	}

	for (UINT i = 0; i < nrOfEnemies; i++) {

		delete enemies[i];
	}

	nrOfEnemies = 2;
	nrOfIceEnemies = 2;
	nrOfLavaEnemies = 0;
	level = 0;
	incrementLevels();


	// Remove platform rigid bodies
	for(UINT i = 0; i < bHandler.nrOfCubes; i++)
	{
		if(bHandler.cubeObjects[i].rigidBody->isInWorld()){

			bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(bHandler.cubeObjects[i].rigidBody);
			btMotionState* cubeMotion = bHandler.cubeObjects[i].rigidBody->getMotionState();
			btCollisionShape* cubeShape = bHandler.cubeObjects[i].rigidBody->getCollisionShape();
			delete bHandler.cubeObjects[i].rigidBody;
			delete cubeMotion;
			delete cubeShape;

		}

	}

	// Remove lava plane rigid body

	if (bHandler.lavaPitRigidBody->isInWorld()) {

		bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(bHandler.lavaPitRigidBody);
		btMotionState* planeMotion = bHandler.lavaPitRigidBody->getMotionState();
		btCollisionShape* planeShape = bHandler.lavaPitRigidBody->getCollisionShape();
		delete bHandler.lavaPitRigidBody;
		delete planeMotion;
		delete planeShape;

	}

	//----------------------------------------------------------------------------------------------------------------------------------//
	// CLEAR THE RIGID BODIES VECTORS
	//----------------------------------------------------------------------------------------------------------------------------------//

	bulletPhysicsHandler.rigidBodies.clear();
	bulletPhysicsHandler.enemyRigidBodies.clear();
	bulletPhysicsHandler.projectileRigidBodies.clear();

}

void SceneContainer::ReInitialize()
{

	srand(time(NULL));

	enemies.clear();
	enemies.resize(nrOfEnemies);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// RE-CREATE RIGID BODIES
	//----------------------------------------------------------------------------------------------------------------------------------//
	
	// Recreate player
	character.setPos(XMFLOAT3(2, 2, 5));
	character.setHealth(10);

	character.CreatePlayerBoundingBox(0.10, character.getPos(), getRadiusCharacter(), getHeightCharacter(), bulletPhysicsHandler);
	this->character.rigidBody->setIslandTag(characterRigid);//This is for checking intersection ONLY between the projectile of the player and any possible enemy, not with platforms or other rigid bodies

	XMFLOAT3 initSpawnPos;

	for (UINT i = 0; i < nrOfEnemies; i++) {

		initSpawnPos.x = RandomNumber(-15, 15);
		initSpawnPos.y = 2;
		initSpawnPos.z = RandomNumber(-15, 15);

		if (i < nrOfIceEnemies) {

			enemies[i] = new Enemy(0, { initSpawnPos.x, initSpawnPos.y, initSpawnPos.z });
			enemies[i]->Spawn(gHandler.gDevice, bulletPhysicsHandler, i, getRadiusIce(), getHeightIce());

		}

		else if (i >= nrOfIceEnemies) {

			enemies[i] = new Enemy(1, { initSpawnPos.x, initSpawnPos.y, initSpawnPos.z });
			enemies[i]->Spawn(gHandler.gDevice, bulletPhysicsHandler, i, getRadiusLava(), getHeightLava());
			enemies[i]->createProjectile(bulletPhysicsHandler);

		}

	}

	// Recreate the platforms
	bHandler.nrOfCubes = 0;
	bHandler.CreatePlatforms(gHandler.gDevice, bulletPhysicsHandler);

	// Reset platforms
	for (UINT i = 0; i < bHandler.nrOfCubes; i++) {

		bHandler.cubeObjects[i].Hit = false;
		bHandler.cubeObjects[i].Damaged = false;
		bHandler.randomNumbers[i] = rand() % 2;

		bHandler.cubeObjects[i].descensionTimer = 0;
		bHandler.cubeObjects[i].breakTimer = 0;
		bHandler.cubeObjects[i].ascensionTimer = 0;
		bHandler.cubeObjects[i].rigidBody->setWorldTransform(bHandler.cubeObjects[i].originMatrix);
	}

	// Recreate the lava plane rigid body
	bHandler.CreateCollisionPlane(bulletPhysicsHandler, XMFLOAT3(0, -4, 0));

}

bool SceneContainer::readFiles() {

	// Load file for the main character
	if (!mainCharacterFile.readFormat("Format//mainCharacter_Binary.txt")) {

		return false;
	}

	// Load file for the ice enemy
	if (!iceEnemyFile.readFormat("Format//iceEnemy_Binary.txt")) {

		return false;
	}

	// Load file for fortress
	if (!FortressFile.readFormat("Format//Fortress_binary.txt"))
	{
		return false;
	}

	// Laod file for platforms
	if (!PlatformFile.readFormat("Format//Platform_binary.txt"))
	{
		return false;
	}

	// Load file for lava enemy
	if (!lavaEnemyFile.readFormat("Format//lavaEnemy_binary.txt"))
	{
		return false;
	}

	// Load file for projectile and explosion shape
	if (!projectileFile.readFormat("Format//Projectile_binary.txt"))
	{
		return false;
	}

	return true;
}

void SceneContainer::update(HWND &windowHandle, float enemyTimePoses[30], Timer timer)
{
	int deadEnemies = 0;
	bHandler.CreateRigidBodyTags();
	character.meleeAttack(windowHandle, this->nrOfEnemies, this->enemies, bulletPhysicsHandler.bulletDynamicsWorld, enemyTimePoses);
	character.rangeAttack(windowHandle, this->nrOfEnemies, this->enemies, bulletPhysicsHandler.bulletDynamicsWorld, gHandler, bHandler, enemyTimePoses);
	
	if (character.currentHealth != character.getHealth())
	{
		HUD.setElementPos(gHandler.gDevice, character.getHealth());
		character.currentHealth = character.getHealth();
	}

	for (UINT i = 0; i < this->nrOfEnemies; i++){
	

		/*if (i < nrOfIceEnemies){

			this->useAI(character, enemies[i]);

		}

		else if (i >= nrOfIceEnemies) {

			this->useAI(character, enemies[i]);
			enemies[i]->updateProjectile();

		}*/

		if(enemies[i]->getAlive() == true){

			this->useAI(character, enemies[i], enemyTimePoses[i]);
			this->enemies[i]->timer.updateCurrentTime();

			if (enemies[i]->getType() == 1)
			{
				
				enemies[i]->updateProjectile();
				
			}
		}

		

		btVector3 velVec = enemies[i]->rigidBody->getLinearVelocity();
		
		XMFLOAT3 vel = { velVec.getX(), velVec.getY(), velVec.getZ()};

		if (enemies[i]->attackFlag == false) {

		
			if ( vel.x < 0.1 && vel.z < 0.1)
			{
				enemies[i]->currentAnimIndex = 1;
			}
			else
			{ 
				enemies[i]->currentAnimIndex = 0;
			}

		}


	}

	for (int i = 0; i < nrOfEnemies; i++)
	{
		if (!enemies[i]->getAlive())
		{
			deadEnemies++;
		}
	}

	for (UINT i = 0; i < bHandler.nrOfCubes; i++) {

		// If the platform was hit by a projectile...
		if(bHandler.cubeObjects[i].Hit == true){

			// Add delta time to platform break timer
			bHandler.cubeObjects[i].breakTimer += timer.getDeltaTime();

			// If the break timer is greater than five...
			if (bHandler.cubeObjects[i].breakTimer > BREAK_LIMIT) {

				// Add delta time to platform descension timer
				bHandler.cubeObjects[i].descensionTimer += timer.getDeltaTime();

				// If descension timer is greater than 20...
				if (bHandler.cubeObjects[i].descensionTimer >= DESCENSION_LIMIT) {

					// Add delta time to to ascension timer
					//bHandler.cubeObjects[i].ascensionTimer += timer.getDeltaTime();

					// If ascension timer is greater than 20...
					if (bHandler.cubeObjects[i].ascensionTimer >= 1) {

						// Restore platform
						bHandler.cubeObjects[i].Hit = false;
						bHandler.cubeObjects[i].Damaged = false;
						bHandler.cubeObjects[i].descensionTimer = 0;
						bHandler.cubeObjects[i].breakTimer = 0;
						bHandler.cubeObjects[i].ascensionTimer = 0;

					}

				}
			}

		}
	}


	if (deadEnemies == nrOfEnemies)
	{
		delayWave(timer);
		respawnDelay = true;

		if (waveDelay <= 0)
		{
			respawnDelay = false;
			waveDelay = 10.0f;

			incrementLevels();
			RespawnEnemies();

			if (character.getHealth() < 10)
			{
				character.setHealth(character.getHealth() + 1);
			}
		}
	}
	
	
	render();
}

void SceneContainer::useAI(MainCharacter &player, Enemy* &enemy, float enemyTimePos)
{
	btVector3 edge = ai.collisionEdge(sides, enemy);

	enemy->rigidBody->applyCentralForce(edge);

	if (enemy->getType() == 0)
	{
		this->ai.iceAI(player, enemy, enemyTimePos);
	}
	else if (enemy->getType() == 1)
	{
		this->ai.fireAI(player, enemy, this->bulletPhysicsHandler, enemyTimePos);
	}
}

void SceneContainer::delayWave(Timer timer)
{
	if (waveDelay >= 0.0f)
	{
		waveDelay -= timer.getDeltaTime();
	}
	sceneTimer.updateCurrentTime();
}

void SceneContainer::incrementLevels()
{
	level++;

	HUD.setElementPos(gHandler.gDevice, this->character.getHealth());
	HUD.CreateIndexBuffer(gHandler.gDevice);
	HUD.loadBitMap();
	HUD.setText(level);
	HUD.setFont(gHandler.gDevice);
	HUD.CreateFontIndexBuffer(gHandler.gDevice);

}

void SceneContainer::PlatformCollisionCheck() {

	// Create sphere rigid body
	for (UINT i = nrOfIceEnemies; i < nrOfEnemies; i++) {

		// Create new sphere shape
		btSphereShape* sphereShape = new btSphereShape(1);

		// Get projectile transform
		btTransform projectileTransform = enemies[i]->fireBall.projectileRigidBody->getWorldTransform();

		// Set the motion state
		btMotionState* motion = new btDefaultMotionState(projectileTransform);

		// Definition of the rigid body
		btScalar mass(0.0f);
		btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphereShape);

		// Create the rigid body
		btRigidBody* sphereRigidBody = new btRigidBody(info);

		int arenaCollideWith = COL_PLAYER | COL_ENEMY;
		bulletPhysicsHandler.bulletDynamicsWorld->addRigidBody(sphereRigidBody, COL_LEVEL, 0);

		for (UINT j = 0; j < bHandler.nrOfCubes; j++) {

			MyPlatformContactResultCallback platformCallBack(&bHandler.cubeObjects[j]);
			bulletPhysicsHandler.bulletDynamicsWorld->contactPairTest(sphereRigidBody, bHandler.cubeObjects[j].rigidBody, platformCallBack);

		}

		// Delete the sphere rigid body 
		bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(sphereRigidBody);
		btMotionState* destructMotion = sphereRigidBody->getMotionState();
		btCollisionShape* destructShape = sphereRigidBody->getCollisionShape();

		delete sphereRigidBody;
		delete destructMotion;
		delete destructShape;

	}

}

void SceneContainer::PlatformManagement(Timer timer) {

	if (respawnDelay == false) {

		for (UINT i = 0; i < bHandler.nrOfCubes; i++) {

			if (bHandler.cubeObjects[i].Hit == true) {

				if (bHandler.cubeObjects[i].breakTimer < BREAK_LIMIT) {

					if (platformRumble.getStatus() != 2) {

						platformRumble.setBuffer(platformSoundBuffer[1]);
						platformRumble.play();
					}

					bHandler.cubeObjects[i].Damaged = true;
					bHandler.cubeObjects[i].platformBreaking();


				}

				else {

					if (bHandler.cubeObjects[i].descensionTimer < DESCENSION_LIMIT) {

						if (bHandler.cubeObjects[i].descensionTimer < 1) {

							if (platformCrumbling.getStatus() != 2) {

								platformCrumbling.setBuffer(platformSoundBuffer[0]);
								platformCrumbling.play();
							}
						}

						bHandler.cubeObjects[i].Damaged = true;
						bHandler.cubeObjects[i].platformDecension(timer.getDeltaTime(), 0.1);

					}

					else {

						bHandler.cubeObjects[i].Damaged = false;
						bHandler.cubeObjects[i].ascensionTimer = bHandler.cubeObjects[i].platformAcension(timer.getDeltaTime(), 0.2);

					}

				}

			}
		}

	}

	// Restore the platforms for the next round
	else {

		for (UINT i = 0; i < bHandler.nrOfCubes; i++) {

			if (bHandler.cubeObjects[i].Hit == true) {

				bHandler.cubeObjects[i].Damaged = false;
				bHandler.cubeObjects[i].platformAcension(timer.getDeltaTime(), 0.4);

			}
		}

	}
}

void SceneContainer::renderFortress() {

	gHandler.gDeviceContext->VSSetShader(gHandler.gFortressVertexShader, nullptr, 0);
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);

	ID3D11GeometryShader* nullShader = nullptr;
	gHandler.gDeviceContext->GSSetShader(nullShader, nullptr, 0);

	gHandler.gDeviceContext->PSSetShader(gHandler.gFortressPixelShader, nullptr, 0);
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.fortressResource);
	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	UINT32 vertexSize = sizeof(StandardVertex);
	UINT32 offset = 0;

	gHandler.gDeviceContext->IASetInputLayout(gHandler.gFortressLayout);
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &bHandler.gFortressBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	character.draw(gHandler.gDeviceContext, FortressFile.standardMeshes[0].vertices.size());
}

void SceneContainer::renderPlatforms() {

	tHandler.texArr[0] = tHandler.shadowSRV;
	tHandler.texArr[1] = tHandler.platformGrass;
	tHandler.texArr[2] = tHandler.platformStone;
	tHandler.texArr[3] = tHandler.platformStoneCracks;
	tHandler.samplerArr[0] = tHandler.texSampler;
	tHandler.samplerArr[1] = tHandler.shadowSampler;

	ID3D11ShaderResourceView* nullResource[4] = { nullptr };

	gHandler.gDeviceContext->VSSetShader(gHandler.gPlatformVertexShader, nullptr, 0);
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(1, 1, &bHandler.gInstanceBuffer);

	ID3D11GeometryShader* nullShader = { nullptr };
	gHandler.gDeviceContext->GSSetShader(nullShader, nullptr, 0);

	gHandler.gDeviceContext->PSSetShader(gHandler.gPlatformPixelShader, nullptr, 0);
	//gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.platformResource);
	//gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);
	gHandler.gDeviceContext->PSSetShaderResources(0, 4, tHandler.texArr);
	gHandler.gDeviceContext->PSSetSamplers(0, 2, tHandler.samplerArr);

	UINT32 vertexSize = sizeof(StandardVertex);
	UINT32 offset = 0;

	gHandler.gDeviceContext->IASetInputLayout(gHandler.gPlatformLayout);
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &bHandler.gCubeVertexBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	gHandler.gDeviceContext->DrawInstanced(PlatformFile.standardMeshes[0].vertices.size(), bHandler.nrOfCubes, 0, 0);

	gHandler.gDeviceContext->PSSetShaderResources(0, 4, nullResource);


}

void SceneContainer::renderDebugCubes() {

	gHandler.gDeviceContext->VSSetShader(gHandler.gDebugVertexShader, nullptr, 0);
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(1, 1, &animHandler.gCharacterBoneBuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(2, 1, &bHandler.gPlayerTransformBuffer);

	ID3D11GeometryShader* nullBuffer = nullptr;
	gHandler.gDeviceContext->GSSetShader(nullBuffer, nullptr, 0);

	gHandler.gDeviceContext->PSSetShader(gHandler.gDebugPixelShader, nullptr, 0);

	UINT32 vertexSize = sizeof(TriangleVertex);
	UINT32 offset = 0;

	gHandler.gDeviceContext->IASetInputLayout(gHandler.gDebugVertexLayout);
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &bHandler.gDebugVertexBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetIndexBuffer(bHandler.gDebugIndexBuffer, DXGI_FORMAT_R32_UINT, offset);
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gHandler.gDeviceContext->DrawIndexedInstanced(36, 16, 0, 0, 0);
}

void SceneContainer::clear()
{
	// clear the back buffer to a deep blue
	float clearColor[] = { 0, 0, 1, 1 };	// Back buffer clear color as an array of floats (rgba)
	gHandler.gDeviceContext->ClearRenderTargetView(gHandler.gBackbufferRTV, clearColor);	// Clear the render target view using the specified color
	gHandler.gDeviceContext->ClearDepthStencilView(gHandler.depthView, D3D11_CLEAR_DEPTH, 1.0f, 0);	// Clear the depth stencil view
}

void SceneContainer::resetRenderTarget(GraphicComponents &gHandler) {

	ID3D11DepthStencilView* nullDepthView = { nullptr };
	gHandler.gDeviceContext->OMSetRenderTargets(1, &gHandler.gBackbufferRTV, nullDepthView);
}

void SceneContainer::render() 
{
	clear();

	if (character.renderRay)
	{
		renderRay();
	}

	renderShadowMap();
	renderLava(); 
	renderCharacters();
	renderIceEnemies();

	if(nrOfLavaEnemies > 0){

		renderLavaEnemies();
		renderProjectile();
	
	}

	renderScene();
	renderHUD();
	
}

void SceneContainer::renderRay()
{
	gHandler.gDeviceContext->VSSetShader(gHandler.rayVertexShader, nullptr, 0);
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);
	gHandler.gDeviceContext->PSSetShader(gHandler.rayPixelShader, nullptr, 0);
	
	UINT32 vertexSize = sizeof(RayVertex);
	UINT32 offset = 0;

	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &character.rayBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.rayInputLayout);

	gHandler.gDeviceContext->Draw(2, 0);

}

bool SceneContainer::renderDeferred() {

	bool result;

	// Step 1: Render the scene to the render buffers
	result = renderSceneToTexture();

	if (!result) {

		return false;
	}

	// Step 2: Unbinding

	//gHandler.gDeviceContext->ClearState();

	ID3D11GeometryShader* nullShader = { nullptr };
	gHandler.gDeviceContext->GSSetShader(nullShader, nullptr, 0);

	ID3D11RenderTargetView* nullRenderTargets = { nullptr };
	ID3D11DepthStencilView* nullDepthStencilView = { nullptr };
	gHandler.gDeviceContext->OMSetRenderTargets(1, &nullRenderTargets, nullDepthStencilView);

	ID3D11ShaderResourceView* nullShaderResourceView = { nullptr };
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

	// Step 3: Switch back to backbuffer as render target
	// Turn the render target back to the original back buffer and not the render buffers anymore
	// Turns off the z-buffer for 2D rendering
	resetRenderTarget(gHandler);

	// Step 4: 2D rendering of light calculations

	lightShaders.SetShaderParameters(gHandler.gDeviceContext,
									deferredObject.d_shaderResourceViewArray[0],
									deferredObject.d_shaderResourceViewArray[1],
									deferredObject.d_shaderResourceViewArray[2],
									deferredObject.d_depthResourceView);

	gHandler.gDeviceContext->PSSetConstantBuffers(1, 1, &bHandler.gConstantBuffer);
									
	lightShaders.Render(gHandler.gDeviceContext, deferredObject.ImportStruct.size());

	return true;
}

bool SceneContainer::renderSceneToTexture() {

	// Set the render buffers to be the render target
	deferredObject.SetRenderTargets(gHandler.gDeviceContext);

	// Clear the render buffers
	deferredObject.ClearRenderTargets(gHandler.gDeviceContext, 0.0f, 0.0f, 0.0f, 1.0f);

	// Set the object vertex buffer to prepare it for drawing
	deferredObject.SetObjectBuffer(gHandler.gDeviceContext);

	// Render the object using the deferred shader
	int indexCounter = deferredObject.ImportStruct.size();

	// Don't forget to set the constant buffer to the geometry shader
	gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);
	
	deferredShaders.Render(gHandler.gDeviceContext, tHandler.texSampler, tHandler.platformGrass, indexCounter);

	return true;
	
}

void SceneContainer::renderScene() {

	renderPlatforms();
	renderFortress();
}

void SceneContainer::renderCharacters()
{
	
	tHandler.texArr[0] = tHandler.playerResource;
	tHandler.texArr[1] = tHandler.shadowSRV;

	tHandler.samplerArr[0] = tHandler.texSampler;
	tHandler.samplerArr[1] = tHandler.shadowSampler;

	ID3D11ShaderResourceView* nullResouce[2] = { nullptr };

	gHandler.gDeviceContext->VSSetShader(gHandler.gVertexShader, nullptr, 0);
	gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);
	gHandler.gDeviceContext->GSSetConstantBuffers(1, 1, &bHandler.gPlayerTransformBuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &animHandler.gCharacterBoneBuffer);
	gHandler.gDeviceContext->GSSetShader(gHandler.gGeometryShader, nullptr, 0);

	gHandler.gDeviceContext->PSSetShader(gHandler.gPixelShader, nullptr, 0);
	//gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.defaultResource);
	//gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);
	gHandler.gDeviceContext->PSSetShaderResources(0, 2, tHandler.texArr);
	gHandler.gDeviceContext->PSSetSamplers(0, 2, tHandler.samplerArr);

	UINT32 vertexSize = sizeof(Vertex_Bone);
	UINT32 offset = 0;

	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &character.vertexBuffer, &vertexSize, &offset);

	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gVertexLayout);

	character.draw(gHandler.gDeviceContext, mainCharacterFile.skinnedMeshes[0].vertices.size());

	character.resetWorldMatrix();

	gHandler.gDeviceContext->PSSetShaderResources(0, 2, nullResouce);


}

void SceneContainer::renderIceEnemies()
{
	tHandler.texArr[0] = tHandler.iceEnemyResource;
	tHandler.texArr[1] = tHandler.shadowSRV;

	tHandler.samplerArr[0] = tHandler.texSampler;
	tHandler.samplerArr[1] = tHandler.shadowSampler;

	gHandler.gDeviceContext->VSSetShader(gHandler.gIceEnemyVertexShader, nullptr, 0);
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(1, 1, &bHandler.gIceEnemyTransformBuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(2, 1, &animHandler.gIceEnemyBoneBuffer);

	ID3D11GeometryShader* nullShader = nullptr;
	gHandler.gDeviceContext->GSSetShader(nullShader, nullptr, 0);

	gHandler.gDeviceContext->PSSetShader(gHandler.gIceEnemyPixelShader, nullptr, 0);
	gHandler.gDeviceContext->PSSetShaderResources(0, 2, tHandler.texArr);
	gHandler.gDeviceContext->PSSetSamplers(0, 2, tHandler.samplerArr);

	UINT32 vertexSize = sizeof(Vertex_Bone);
	UINT32 offset = 0;

	ID3D11Buffer* nullBuffer = { nullptr };
	gHandler.gDeviceContext->IASetIndexBuffer(nullBuffer, DXGI_FORMAT_R32_UINT, 0);

	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &enemyIceVertexBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gIceEnemyVertexLayout);

	gHandler.gDeviceContext->DrawInstanced(this->iceEnemyVertices.size(), this->nrOfIceEnemies, 0, 0);

	ID3D11Buffer* nullbuffer = nullptr;

	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &nullbuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(1, 1, &nullbuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(2, 1, &nullbuffer);
	
}

void SceneContainer::renderLavaEnemies()
{
	tHandler.texArr[0] = tHandler.lavaEnemyResource;
	tHandler.texArr[1] = tHandler.shadowSRV;
	
	tHandler.samplerArr[0] = tHandler.texSampler;
	tHandler.samplerArr[1] = tHandler.shadowSampler;

	gHandler.gDeviceContext->VSSetShader(gHandler.gLavaEnemyVertexShader, nullptr, 0);
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(1, 1, &bHandler.gLavaEnemyTransformBuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(2, 1, &animHandler.gLavaEnemyBoneBuffer);

	ID3D11GeometryShader* nullShader = nullptr;
	gHandler.gDeviceContext->GSSetShader(nullShader, nullptr, 0);

	gHandler.gDeviceContext->PSSetShader(gHandler.gLavaEnemyPixelShader, nullptr, 0);
	gHandler.gDeviceContext->PSSetShaderResources(0, 2, tHandler.texArr);
	gHandler.gDeviceContext->PSSetSamplers(0, 2, tHandler.samplerArr);

	UINT32 vertexSize = sizeof(Vertex_Bone);
	UINT32 offset = 0;

	ID3D11Buffer* nullBuffer = { nullptr };
	gHandler.gDeviceContext->IASetIndexBuffer(nullBuffer, DXGI_FORMAT_R32_UINT, 0);

	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &enemyLavaVertexBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gLavaEnemyVertexLayout);

	gHandler.gDeviceContext->DrawInstanced(this->lavaEnemyVertices.size(), this->nrOfLavaEnemies, 0, 0);

	ID3D11Buffer* nullbuffer = nullptr;

	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &nullbuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(1, 1, &nullbuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(2, 1, &nullbuffer);


}

void SceneContainer::renderLava()
{

	gHandler.gDeviceContext->VSSetShader(gHandler.gLavaVertexShader, nullptr, 0);	//vs
	gHandler.gDeviceContext->GSSetShader(gHandler.gLavaGeometryShader, nullptr, 0); //gs
	gHandler.gDeviceContext->PSSetShader(gHandler.gLavaPixelShader, nullptr, 0); //ps

	//texture
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.LavaResource);
	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);
	

	UINT32 vertexSize = sizeof(LavaVertex);
	UINT32 offset = 0;

	//set vertex buffer
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &lava.LavaVB, &vertexSize, &offset);
	//Set index buffer
	gHandler.gDeviceContext->IASetIndexBuffer(lava.LavaIB, DXGI_FORMAT_R32_UINT, offset);
	//set triagel list
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gLavaVertexLayout);

	gHandler.gDeviceContext->DrawIndexed(lava.indexCounter, 0, 0);
}

void SceneContainer::renderProjectile()
{
	gHandler.gDeviceContext->VSSetShader(gHandler.gProjectileVertexShader, nullptr, 0);
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(1, 1, &bHandler.gProjectileTransformBuffer);

	ID3D11GeometryShader* nullShader = nullptr;
	gHandler.gDeviceContext->GSSetShader(nullShader, nullptr, 0);

	gHandler.gDeviceContext->PSSetShader(gHandler.gProjectilePixelShader, nullptr, 0);
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.projectileResource);
	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	UINT32 vertexSize = sizeof(StandardVertex);
	UINT32 offset = 0;

	gHandler.gDeviceContext->IASetInputLayout(gHandler.gProjectileVertexLayout);
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &projectileVertexBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, offset);

	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	gHandler.gDeviceContext->DrawInstanced(projectileVertices.size(), this->nrOfLavaEnemies, 0, 0);

	ID3D11Buffer* nullbuffer = nullptr;

	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &nullbuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(1, 1, &nullbuffer);
}

void SceneContainer::createSideBoundingBoxes()
{
	//We need the extents of the BBox. should we make thinn boxes? Or big squares that cover the right areas?
	//We also need the Center
	XMFLOAT3 extentsUpDown = { 25, 1, 1 };
	XMFLOAT3 extentsLeftRight = { 1, 1, 25 };

	XMFLOAT3 centerUp = {-7, 0, 17};
	XMFLOAT3 centerDown = {-7, 0, -18};
	XMFLOAT3 centerLeft = {-17, 0, -7};
	XMFLOAT3 centerRight = {17, 0, -7};

	sides[0] = BoundingBox(centerUp, extentsUpDown);		//Up
	sides[1] = BoundingBox(centerDown, extentsUpDown);		//Down
	sides[2] = BoundingBox(centerLeft, extentsLeftRight);	//Left
	sides[3] = BoundingBox(centerRight, extentsLeftRight);	//Right

	
}

void SceneContainer::renderShadowMap()
{
	gHandler.gDeviceContext->ClearDepthStencilView(tHandler.shadowDepthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	gHandler.gDeviceContext->OMSetRenderTargets(0, nullptr, tHandler.shadowDepthView);

	bHandler.gBufferArr[0] = bHandler.gConstantBuffer;
	bHandler.gBufferArr[1] = bHandler.gPlayerTransformBuffer;
	bHandler.gBufferArr[2] = animHandler.gCharacterBoneBuffer;

	// Character pass-------------------------------------------------------------------------------------//
	gHandler.gDeviceContext->VSSetShader(gHandler.gShadowVertexShader, nullptr, 0);
	gHandler.gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gHandler.gDeviceContext->PSSetShader(nullptr, nullptr, 0);
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 3, bHandler.gBufferArr);

	UINT32 vertexSize = sizeof(Vertex_Bone);
	UINT32 offset = 0;

	gHandler.gDeviceContext->IASetInputLayout(gHandler.gVertexLayout);
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &character.vertexBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	character.draw(gHandler.gDeviceContext, mainCharacterFile.skinnedMeshes[0].vertices.size());
	//-----------------------------------------------------------------------------------------------------//

	//Lava enemy pass------------------------------------------------------------------------------------------------//

	bHandler.gBufferArr[1] = bHandler.gLavaEnemyTransformBuffer;
	bHandler.gBufferArr[2] = animHandler.gLavaEnemyBoneBuffer;

	gHandler.gDeviceContext->VSSetShader(gHandler.gShadowLavaVertex, nullptr, 0);
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 3, bHandler.gBufferArr);

	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &enemyLavaVertexBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gShadowLayaLayout);

	gHandler.gDeviceContext->DrawInstanced(this->lavaEnemyVertices.size(), this->nrOfLavaEnemies, 0, 0);


	//--------------------------------------------------------------------------------------------------------------//
	//Ice enemy pass-----------------------------------------------------------------------------------------------//
	bHandler.gBufferArr[1] = bHandler.gIceEnemyTransformBuffer;
	bHandler.gBufferArr[2] = animHandler.gIceEnemyBoneBuffer;

	gHandler.gDeviceContext->VSSetShader(gHandler.gShadowIceVertex, nullptr, 0);
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 3, bHandler.gBufferArr);

	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &enemyIceVertexBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gShadowIceLayout);

	gHandler.gDeviceContext->DrawInstanced(this->iceEnemyVertices.size(), this->nrOfIceEnemies, 0, 0);

	//Platform pass--------------------------------------------------------------------------------------------------//
	bHandler.gBufferArr[1] = bHandler.gInstanceBuffer;

	gHandler.gDeviceContext->VSSetShader(gHandler.gShadowPlatformVertex, nullptr, 0);
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 2, bHandler.gBufferArr);

	vertexSize = sizeof(StandardVertex);
	offset = 0;

	gHandler.gDeviceContext->IASetInputLayout(gHandler.gShadowPlatformLayout);
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &bHandler.gCubeVertexBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gHandler.gDeviceContext->DrawInstanced(PlatformFile.standardMeshes[0].vertices.size(), bHandler.nrOfCubes, 0, 0);
	//----------------------------------------------------------------------------------------------------------------//

	//Set the rendertarget to the normal render target view and depthstencilview
	gHandler.gDeviceContext->OMSetRenderTargets(1, &gHandler.gBackbufferRTV, gHandler.depthView);

}

void SceneContainer::renderHUD()
{


	///////////////////////////////////////////////////////////////////////////////////////
	//						PORTRAIT DRAWCALL
	///////////////////////////////////////////////////////////////////////////////////////
	gHandler.gDeviceContext->OMSetBlendState(tHandler.blendState, 0, 0xffffffff);
	//gHandler.gDeviceContext->ClearDepthStencilView(gHandler.depthView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	gHandler.gDeviceContext->VSSetShader(gHandler.gHUDVertexShader, nullptr, 0);	//vs
	
	gHandler.gDeviceContext->PSSetShader(gHandler.gHUDPixelShader, nullptr, 0); //ps

																				 //texture
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.HUDPortrait);
	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	gHandler.gDeviceContext->GSSetConstantBuffers(0, 0, nullptr);


	UINT32 vertexSize = sizeof(HUDElements);
	UINT32 offset = 0;

	//set vertex buffer
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &HUD.gElementVertexBuffer, &vertexSize, &offset);
	//Set index buffer
	gHandler.gDeviceContext->IASetIndexBuffer(HUD.gElementIndexBuffer, DXGI_FORMAT_R32_UINT, offset);
	//set triangel list
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gHUDVertexLayout);

	gHandler.gDeviceContext->DrawIndexed(6, 0, 0);

	///////////////////////////////////////////////////////////////////////////////////////
	//						FONT DRAWCALL
	///////////////////////////////////////////////////////////////////////////////////////

	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.HUDResource);

	//set vertex buffer
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &HUD.gFontVertexBuffer, &vertexSize, &offset);
	//Set index buffer
	gHandler.gDeviceContext->IASetIndexBuffer(HUD.gFontIndexBuffer, DXGI_FORMAT_R32_UINT, offset);


	gHandler.gDeviceContext->DrawIndexed(HUD.foo, 0, 0);


	///////////////////////////////////////////////////////////////////////////////////////
	//						HPBAR DRAWCALL
	///////////////////////////////////////////////////////////////////////////////////////

	gHandler.gDeviceContext->VSSetShader(gHandler.gHUDVertexShader, nullptr, 0);	//vs

	gHandler.gDeviceContext->PSSetShader(gHandler.gHUDPixelShader, nullptr, 0); //ps

																				//texture
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.HUDHealth);
	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	gHandler.gDeviceContext->GSSetConstantBuffers(0, 0, nullptr);


	 vertexSize = sizeof(HUDElements);
	 offset = 0;

	//set vertex buffer
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &HUD.gHPBarVtxBuffer, &vertexSize, &offset);
	//Set index buffer
	gHandler.gDeviceContext->IASetIndexBuffer(HUD.gElementIndexBuffer, DXGI_FORMAT_R32_UINT, offset);
	//set triangel list
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gHUDVertexLayout);

	gHandler.gDeviceContext->DrawIndexed(6, 0, 0);


	gHandler.gDeviceContext->OMSetBlendState(nullptr, 0, 0xffffffff);





}


float SceneContainer::getRadiusCharacter()
{
	float radius = (mainCharacterFile.skinnedMeshes[0].meshBoundingBox.zMax - mainCharacterFile.skinnedMeshes[0].meshBoundingBox.zMin) / 2; 
	return radius / 10;
}

float SceneContainer::getHeightCharacter()
{
	float height = ((mainCharacterFile.skinnedMeshes[0].meshBoundingBox.yMax-2) - (mainCharacterFile.skinnedMeshes[0].meshBoundingBox.yMin+5));
	return height / 30.0f;
}

float SceneContainer::getRadiusLava()
{
	float radius = (lavaEnemyFile.skinnedMeshes[0].meshBoundingBox.zMax - lavaEnemyFile.skinnedMeshes[0].meshBoundingBox.zMin) / 2;
	return radius / 5;
}

float SceneContainer::getHeightLava()
{
	float height = ((lavaEnemyFile.skinnedMeshes[0].meshBoundingBox.yMax ) - (lavaEnemyFile.skinnedMeshes[0].meshBoundingBox.yMin));
	return height / 30;
}

float SceneContainer::getRadiusIce()
{
	float radius = (iceEnemyFile.skinnedMeshes[0].meshBoundingBox.zMax - iceEnemyFile.skinnedMeshes[0].meshBoundingBox.zMin) / 2;
	return radius / 15;
}

float SceneContainer::getHeightIce()
{
	//max höjd och min höjd ändrar värdena för att trycka ner karaktären i boxen
	float height = ((iceEnemyFile.skinnedMeshes[0].meshBoundingBox.yMax -7) - (iceEnemyFile.skinnedMeshes[0].meshBoundingBox.yMin +4));
	// måste delas för anars blir höjden som i maya
	height = height / 5.0f;
	return  height;
}