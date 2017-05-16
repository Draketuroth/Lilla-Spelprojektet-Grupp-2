
#include "SceneContainer.h"

SceneContainer::SceneContainer() {

	// Initialize handler and main character

	mainCharacterFile = FileImporter();
	gHandler = GraphicComponents();
	bHandler = BufferComponents();
	tHandler = TextureComponents();

	character = MainCharacter();

	this->nrOfIceEnemies = 20;
	this->nrOfLavaEnemies = 10;
	this->nrOfEnemies = nrOfIceEnemies + nrOfLavaEnemies;

	bulletPhysicsHandler = BulletComponents();
	this->level = 0;

	this->ai = AI();

}

SceneContainer::~SceneContainer() {

}

void SceneContainer::releaseAll() {

	character.releaseAll(bulletPhysicsHandler.bulletDynamicsWorld);

	for(UINT i = 0; i < nrOfEnemies; i++){

		enemies[i].releaseAll(bulletPhysicsHandler.bulletDynamicsWorld);

	}

	SAFE_RELEASE(enemyIceVertexBuffer);
	SAFE_RELEASE(enemyLavaVertexBuffer);

	SAFE_RELEASE(gProjectileBuffer);
	SAFE_RELEASE(gProjectileIndexBuffer);

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

	character.initialize(gHandler.gDevice, XMFLOAT3(2, 2, 5), bulletPhysicsHandler, animHandler, mainCharacterFile);
	
	HUD.setElementPos(gHandler.gDevice);
	HUD.CreateIndexBuffer(gHandler.gDevice);
	HUD.loadBitMap();
	HUD.setText(0);
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

		if(i < nrOfIceEnemies){

		enemies[i] = Enemy(0, { initSpawnPos.x, initSpawnPos.y, initSpawnPos.z });
		enemies[i].Spawn(gHandler.gDevice, bulletPhysicsHandler, i);

		}

		else if (i >= nrOfIceEnemies) {

			enemies[i] = Enemy(1, { initSpawnPos.x, initSpawnPos.y, initSpawnPos.z });
			enemies[i].Spawn(gHandler.gDevice, bulletPhysicsHandler, i);
			enemies[i].createProjectile(bulletPhysicsHandler);

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

		// Remove ice enemy rigid body
		bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(enemies[i].rigidBody);
	}

	for (UINT i = nrOfIceEnemies; i < nrOfEnemies; i++) {

		// Remove lava enemy rigid body
		bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(enemies[i].rigidBody);

		// Remove lava enemy projectile rigid body
		bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(enemies[i].fireBall.projectileRigidBody);
	}

	// Clear enemy rigid bodies vector
	bulletPhysicsHandler.enemyRigidBodies.clear();

	// Recreate enemies and their rigid bodies

	XMFLOAT3 initSpawnPos;

	for (UINT i = 0; i < nrOfEnemies; i++) {

		initSpawnPos.x = RandomNumber(-15, 15);
		initSpawnPos.y = 2;
		initSpawnPos.z = RandomNumber(-15, 15);

		if (i < nrOfIceEnemies) {

			enemies[i] = Enemy(0, { initSpawnPos.x, initSpawnPos.y, initSpawnPos.z });
			enemies[i].Spawn(gHandler.gDevice, bulletPhysicsHandler, i);

		}

		else if (i >= nrOfIceEnemies) {

			enemies[i] = Enemy(1, { initSpawnPos.x, initSpawnPos.y, initSpawnPos.z });
			enemies[i].Spawn(gHandler.gDevice, bulletPhysicsHandler, i);
			enemies[i].createProjectile(bulletPhysicsHandler);

		}

	}

}

bool SceneContainer::createProjectileBox(ID3D11Device* gDevice)
{
	HRESULT hr;

	float scale = 0.3f;

	TriangleVertex triangleVertices[24] =
	{

		//Front face

		-scale, scale, -scale, 0.0f, 0.0f,
		scale, scale, -scale, 1.0f, 0.0f,
		-scale, -scale, -scale, 0.0f, 1.0f,
		scale, -scale, -scale, 1.0f, 1.0f,

		// Back face

		scale, scale, scale, 0.0f, 0.0f,
		-scale, scale, scale, 1.0f, 0.0f,
		scale, -scale, scale, 0.0f, 1.0f,
		-scale, -scale, scale, 1.0f, 1.0f,

		// Left face

		-scale, scale, scale, 0.0f, 0.0f,
		-scale, scale, -scale, 1.0f, 0.0f,
		-scale, -scale, scale, 0.0f, 1.0f,
		-scale, -scale, -scale, 1.0f, 1.0f,

		// Right face

		scale, scale, -scale, 0.0f, 0.0f,
		scale, scale, scale, 1.0f, 0.0f,
		scale, -scale, -scale, 0.0f, 1.0f,
		scale, -scale,  scale, 1.0f, 1.0f,

		// Top face

		-scale, scale, scale, 0.0f, 0.0f,
		scale, scale, scale, 1.0f, 0.0f,
		-scale, scale, -scale, 0.0f, 1.0f,
		scale, scale, -scale, 1.0f, 1.0f,

		// Bottom face

		scale, -scale, scale, 0.0f, 0.0f,
		-scale, -scale, scale, 1.0f, 0.0f,
		scale, -scale, -scale, 0.0f, 1.0f,
		-scale, -scale, -scale, 1.0f, 1.0f


	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.ByteWidth = sizeof(TriangleVertex) * 24;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = triangleVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &gProjectileBuffer);

	if (FAILED(hr)) {

		return false;
	}

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

	hr = gDevice->CreateBuffer(&bufferDesc, &initData, &gProjectileIndexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

void SceneContainer::loadIceEnemyVertices(FileImporter &importer, ID3D11Device* &graphicDevice){

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

	for (UINT i = 0; i < nrOfIceEnemies; i++) 
	{

		// Remove ice enemy rigid body
		bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(enemies[i].rigidBody);

	}

	for (UINT i = nrOfIceEnemies; i < nrOfEnemies; i++) {

		// Remove lava enemy enemy rigid body
		bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(enemies[i].rigidBody);

		// Remove projectile rigid body
		bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(enemies[i].fireBall.projectileRigidBody);
	}

	// Remove platform rigid bodies
	for(UINT i = 0; i < bHandler.nrOfCubes; i++)
	{

		bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(bHandler.cubeObjects[i].rigidBody);

	}

	// Remove lava plane rigid body
	bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(bHandler.lavaPitRigidBody);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// CLEAR THE RIGID BODIES FROM THE DYNAMICS WORLD
	//----------------------------------------------------------------------------------------------------------------------------------//

	bulletPhysicsHandler.rigidBodies.clear();
	bulletPhysicsHandler.enemyRigidBodies.clear();

}

void SceneContainer::ReInitialize() {

	//----------------------------------------------------------------------------------------------------------------------------------//
	// RE-CREATE RIGID BODIES
	//----------------------------------------------------------------------------------------------------------------------------------//
	
	// Recreate player
	character.setPos(XMFLOAT3(2, 2, 5));
	character.setHealth(5);

	XMFLOAT3 initSpawnPos;

	for (UINT i = 0; i < nrOfEnemies; i++) {

		initSpawnPos.x = RandomNumber(-15, 15);
		initSpawnPos.y = 2;
		initSpawnPos.z = RandomNumber(-15, 15);

		enemies[i] = Enemy(0, { initSpawnPos.x, initSpawnPos.y, initSpawnPos.z });

	}

	character.CreatePlayerBoundingBox(0.10, character.getPos(), XMFLOAT3(0.3, 0.8f, 0.3), bulletPhysicsHandler);
	this->character.rigidBody->setIslandTag(characterRigid);//This is for checking intersection ONLY between the projectile of the player and any possible enemy, not with platforms or other rigid bodies

	// Recreate enemy
	for (UINT i = 0; i < nrOfEnemies; i++) {

		//enemies[i].setHealth
		enemies[i].setAlive(true);
		enemies[i].Spawn(gHandler.gDevice, bulletPhysicsHandler, i);
		enemies[i].createProjectile(bulletPhysicsHandler);

	}

	// Recreate the platforms
	bHandler.nrOfCubes = 0;
	bHandler.CreatePlatforms(gHandler.gDevice, bulletPhysicsHandler);

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

	if (!PlatformFile.readFormat("Format//Platform_binary.txt"))
	{
		return false;
	}

	if (!lavaEnemyFile.readFormat("Format//lavaEnemy_binary.txt"))
	{
		return false;
	}

	return true;
}

void SceneContainer::update(HWND &windowHandle)
{
	
	bHandler.CreateRigidBodyTags();
	character.meleeAttack(windowHandle, this->nrOfEnemies, this->enemies, bulletPhysicsHandler.bulletDynamicsWorld);
	character.rangeAttack(windowHandle, this->nrOfEnemies, this->enemies, bulletPhysicsHandler.bulletDynamicsWorld, gHandler, bHandler);

	for (UINT i = 0; i < this->nrOfEnemies; i++){
	

		if (i < nrOfIceEnemies){

			this->useAI(character, enemies[i]);

		}

		else if (i >= nrOfIceEnemies) {

			this->useAI(character, enemies[i]);
			enemies[i].updateProjectile();

		}
	}

	//cout << "Side Center: " << sides[2].Center.x << ", " << sides[2].Center.y << ", " << sides[2].Center.z << endl;
	//cout << "Enemy Center: " << enemies[0].getBoundingBox().Center.x << ", " << enemies[0].getBoundingBox().Center.y << ", " << enemies[0].getBoundingBox().Center.z << endl;
	
	render();
}

void SceneContainer::useAI(MainCharacter &player, Enemy &enemy)
{
	btVector3 edge = ai.collisionEdge(sides, enemy);

	enemy.rigidBody->applyCentralForce(edge);

	if (enemy.getType() == 0)
	{
		this->ai.iceAI(player, enemy);
	}
	else if (enemy.getType() == 1)
	{
		this->ai.fireAI(player, enemy, this->bulletPhysicsHandler);
	}
}

void SceneContainer::incrementLevels()
{
	level++;

	HUD.setElementPos(gHandler.gDevice);
	HUD.CreateIndexBuffer(gHandler.gDevice);
	HUD.loadBitMap();
	HUD.setText(level);
	HUD.setFont(gHandler.gDevice);
	HUD.CreateFontIndexBuffer(gHandler.gDevice);

}

void SceneContainer::drawFortress() {

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

void SceneContainer::drawPlatforms() {

	tHandler.texArr[0] = tHandler.platformResource;
	tHandler.texArr[1] = tHandler.shadowSRV;
	tHandler.samplerArr[0] = tHandler.texSampler;
	tHandler.samplerArr[1] = tHandler.shadowSampler;

	ID3D11ShaderResourceView* nullResouce[2] = { nullptr };

	gHandler.gDeviceContext->VSSetShader(gHandler.gPlatformVertexShader, nullptr, 0);
	gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);
	gHandler.gDeviceContext->GSSetConstantBuffers(1, 1, &bHandler.gInstanceBuffer);
	gHandler.gDeviceContext->GSSetShader(gHandler.gPlatformGeometryShader, nullptr, 0);

	gHandler.gDeviceContext->PSSetShader(gHandler.gPlatformPixelShader, nullptr, 0);
	//gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.platformResource);
	//gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);
	gHandler.gDeviceContext->PSSetShaderResources(0, 2, tHandler.texArr);
	gHandler.gDeviceContext->PSSetSamplers(0, 2, tHandler.samplerArr);

	UINT32 vertexSize = sizeof(StandardVertex);
	UINT32 offset = 0;

	gHandler.gDeviceContext->IASetInputLayout(gHandler.gPlatformLayout);
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &bHandler.gCubeVertexBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	gHandler.gDeviceContext->DrawInstanced(PlatformFile.standardMeshes[0].vertices.size(), bHandler.nrOfCubes, 0, 0);

	gHandler.gDeviceContext->PSSetShaderResources(0, 2, nullResouce);


}

void SceneContainer::drawDebugCubes() {

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

	renderShadowMap();
	//renderDeferred();
	renderLava(); 
	//drawDebugCubes();
	renderCharacters();
	renderIceEnemies();
	renderLavaEnemies();
	renderScene();
	drawHUD();
	renderProjectile();
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
	
	deferredShaders.Render(gHandler.gDeviceContext, tHandler.texSampler, tHandler.platformResource, indexCounter);

	return true;
	
}

void SceneContainer::renderScene() {

	drawPlatforms();
	drawFortress();
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
	gHandler.gDeviceContext->VSSetShader(gHandler.gIceEnemyVertexShader, nullptr, 0);
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(1, 1, &bHandler.gIceEnemyTransformBuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(2, 1, &animHandler.gIceEnemyBoneBuffer);

	ID3D11GeometryShader* nullShader = nullptr;
	gHandler.gDeviceContext->GSSetShader(nullShader, nullptr, 0);

	gHandler.gDeviceContext->PSSetShader(gHandler.gIceEnemyPixelShader, nullptr, 0);
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.defaultResource);
	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	UINT32 vertexSize = sizeof(Vertex_Bone);
	UINT32 offset = 0;

	ID3D11Buffer* nullBuffer = { nullptr };
	gHandler.gDeviceContext->IASetIndexBuffer(nullBuffer, DXGI_FORMAT_R32_UINT, 0);

	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &enemyIceVertexBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gIceEnemyVertexLayout);

	gHandler.gDeviceContext->DrawInstanced(this->iceEnemyVertices.size(), this->nrOfIceEnemies, 0, 0);
	
}

void SceneContainer::renderLavaEnemies()
{
	tHandler.texArr[0] = tHandler.LavaResource;
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

	gHandler.gDeviceContext->DrawInstanced(this->lavaEnemyVertices.size(), 10, 0, 0);
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

	UINT32 vertexSize = sizeof(TriangleVertex);
	UINT32 offset = 0;

	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &gProjectileBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetIndexBuffer(gProjectileIndexBuffer, DXGI_FORMAT_R32_UINT, offset);

	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	gHandler.gDeviceContext->DrawIndexedInstanced(36, this->nrOfEnemies, 0, 0, 0);

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

	//--------------------------------------------------------------------------------------------------------------//

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

void SceneContainer::drawHUD()
{

	gHandler.gDeviceContext->OMSetBlendState(tHandler.blendState, 0, 0xffffffff);
	gHandler.gDeviceContext->VSSetShader(gHandler.gHUDVertexShader, nullptr, 0);	//vs
	
	gHandler.gDeviceContext->PSSetShader(gHandler.gHUDPixelShader, nullptr, 0); //ps

																				 //texture
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.HUDResource);
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



	//set vertex buffer
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &HUD.gFontVertexBuffer, &vertexSize, &offset);
	//Set index buffer
	gHandler.gDeviceContext->IASetIndexBuffer(HUD.gFontIndexBuffer, DXGI_FORMAT_R32_UINT, offset);


	gHandler.gDeviceContext->DrawIndexed(HUD.foo, 0, 0);


	gHandler.gDeviceContext->OMSetBlendState(nullptr, 0, 0xffffffff);




}
