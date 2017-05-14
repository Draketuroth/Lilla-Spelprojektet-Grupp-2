//----------------------------------------------------------------------------------------------------------------------------------//
// main.cpp
// DV1541 3D PROGMRAMMING: PROJECT
//
// THIS APPLICATION IS CREATED BY THE FOLLOWING GROUP MEMBERS FROM THE TA15 CLASS:
// Philip Velandria, Jonathan Sundberg, Linnea Vajda, Fredrik Linde
//----------------------------------------------------------------------------------------------------------------------------------//

#include "SceneContainer.h"
#include "Timer.h"
#include "GameState.h"
#include <thread>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace std;

//----------------------------------------------------------------------------------------------------------------------------------//
// WINDOW HANDLE AND CAMERA
//----------------------------------------------------------------------------------------------------------------------------------//
HWND windowHandle;

//----------------------------------------------------------------------------------------------------------------------------------//
// SCENE COMPONENTS
//----------------------------------------------------------------------------------------------------------------------------------//
SceneContainer sceneContainer;
GameState menuState;
Timer timer;

//----------------------------------------------------------------------------------------------------------------------------------//
// FORWARD DECLARATIONS
//----------------------------------------------------------------------------------------------------------------------------------//
int RunApplication();
void updateCharacter(HWND windowhandle);
void updateEnemies();
void updateBuffers();
void updateLava();
void lavamovmentUpdate(); 

//----------------------------------------------------------------------------------------------------------------------------------//
// MEMORY LEAK DETECTION
//----------------------------------------------------------------------------------------------------------------------------------//
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

int main() {

	// Memory leak detection

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(65);
#endif

	// We always want to keep our eyes open for terminal errors, which mainly occur when the window isn't created
	
	sceneContainer.initialize(windowHandle);

	return RunApplication();
}

int RunApplication() 
{

	//----------------------------------------------------------------------------------------------------------------------------------//
	// INITIALIZE
	//----------------------------------------------------------------------------------------------------------------------------------//

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// Memory leak detection flag
	
	MSG windowMessage = { 0 };

	SetCapture(windowHandle);

	SetCursorPos(WIDTH / 2, HEIGHT / 2);

	timer.initialize();
	sceneContainer.character.timer.initialize();
	updateLava();

	int gameOverTimer;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// GAME LOOP
	//----------------------------------------------------------------------------------------------------------------------------------//
	while (windowMessage.message != WM_QUIT) {

		if (PeekMessage(&windowMessage, NULL, NULL, NULL, PM_REMOVE)) {

			TranslateMessage(&windowMessage);
			DispatchMessage(&windowMessage);
		}

		// If there are no messages to handle, the application will continue by running a frame
		else {

			//----------------------------------------------------------------------------------------------------------------------------------//
			// UPDATE
			//----------------------------------------------------------------------------------------------------------------------------------//

			float deltaTime = timer.getDeltaTime();


			switch (menuState.state)
			{
			case MAIN_MENU:
				sceneContainer.character.attackSound.stop();
				menuState.menuHandler(windowHandle, sceneContainer, windowMessage);
				break;
			case PAUSE_MENU:
				sceneContainer.character.attackSound.stop();
				menuState.menuHandler(windowHandle, sceneContainer, windowMessage);
				break;
			case GAME_OVER:
				sceneContainer.character.attackSound.stop();
				menuState.menuHandler(windowHandle, sceneContainer, windowMessage);
				sceneContainer.character.setAlive(true);
				break;
			case QUIT_GAME:
				windowMessage.message = WM_QUIT;
				break;
			case START_GAME:
				//sceneContainer.character.setAlive(true);
				menuState.checkGameState();
				updateCharacter(windowHandle);
				updateEnemies();
				/*thread enemyThread(updateEnemies);
				enemyThread.join();*/
				updateBuffers();
				lavamovmentUpdate();
				sceneContainer.bulletPhysicsHandler.bulletDynamicsWorld->stepSimulation(deltaTime);

				if (GetAsyncKeyState('N')) {

					sceneContainer.RespawnEnemies();
					Sleep(200);
				}

				//----------------------------------------------------------------------------------------------------------------------------------//
				// PLAYER LAVA HIT DETECTION
				//----------------------------------------------------------------------------------------------------------------------------------//
				
				MyCharacterContactResultCallback characterCallBack(&sceneContainer.character);

				if (!sceneContainer.character.getAlive())
				{
					menuState.state = GAME_OVER;
				}
				
				sceneContainer.bulletPhysicsHandler.bulletDynamicsWorld->contactPairTest(sceneContainer.bHandler.lavaPitRigidBody, sceneContainer.character.rigidBody, characterCallBack);

				//----------------------------------------------------------------------------------------------------------------------------------//
				// ENEMY LAVA HIT DETECTION
				//----------------------------------------------------------------------------------------------------------------------------------//

				for(UINT i = 0; i < sceneContainer.nrOfEnemies; i++){

					MyEnemyContactResultCallback enemyCallBack(&sceneContainer.enemies[i]);
					sceneContainer.bulletPhysicsHandler.bulletDynamicsWorld->contactPairTest(sceneContainer.bHandler.lavaPitRigidBody, sceneContainer.enemies[i].rigidBody, enemyCallBack);

					if (sceneContainer.enemies[i].getAlive() == false) {

						sceneContainer.enemies[i].releaseAll(sceneContainer.bulletPhysicsHandler.bulletDynamicsWorld);
					}

				}

				//----------------------------------------------------------------------------------------------------------------------------------//
				// RENDER
				//----------------------------------------------------------------------------------------------------------------------------------//

				sceneContainer.update(windowHandle);

				showFPS(windowHandle, deltaTime);

				sceneContainer.gHandler.gSwapChain->Present(0, 0);

				timer.updateCurrentTime();

				break;
			}
			
			
			
		}

	}

	sceneContainer.releaseAll();

	DestroyWindow(windowHandle);

	return 0;
}

void updateCharacter(HWND windowhandle) 
{
	
	sceneContainer.character.update(windowhandle);
	
	sceneContainer.character.camera.UpdateViewMatrix();	// Update Camera View and Projection Matrix for each frame

	sceneContainer.character.playerAnimTimePos += timer.getDeltaTime() * 30;
	float currentPlayerTimePos = sceneContainer.character.playerAnimTimePos;
	int currentAnimIndex = sceneContainer.character.currentAnimIndex;
	int currentAnimationLength = sceneContainer.mainCharacterFile.skinnedMeshes[0].hierarchy[0].Animations[currentAnimIndex].Length;

	if (currentPlayerTimePos >= currentAnimationLength) {

		sceneContainer.character.playerAnimTimePos = 0.0f;
	}

	sceneContainer.animHandler.UpdatePlayerAnimation(sceneContainer.gHandler.gDeviceContext, currentAnimIndex, sceneContainer.mainCharacterFile, currentPlayerTimePos);
}

void updateEnemies() {

	for (UINT i = 0; i < sceneContainer.nrOfEnemies; i++) {

		if (sceneContainer.enemies[i].getAlive() == true) {

			// Update enemy physics
			sceneContainer.enemies[i].EnemyPhysics(sceneContainer.character.getPos());

			if(i < sceneContainer.nrOfIceEnemies){

				// Update enemy animation time pose
				sceneContainer.animHandler.enemyTimePos[i] += timer.getDeltaTime() * 30;
				float currentEnemyTimePos = sceneContainer.animHandler.enemyTimePos[i];
				int currentAnimIndex = sceneContainer.enemies[i].currentAnimIndex;
				int currentAnimationLength = sceneContainer.iceEnemyFile.skinnedMeshes[0].hierarchy[0].Animations[currentAnimIndex].Length;

				if (currentEnemyTimePos >= currentAnimationLength) {

					sceneContainer.animHandler.enemyTimePos[i] = 0.0f;
				}

				sceneContainer.animHandler.UpdateEnemyAnimation(sceneContainer.gHandler.gDeviceContext, sceneContainer.iceEnemyFile, i, currentAnimIndex, sceneContainer.animHandler.enemyTimePos[i]);

			}

			else if (i >= sceneContainer.nrOfIceEnemies) {

				// Update enemy animation time pose
				sceneContainer.animHandler.enemyTimePos[i] += timer.getDeltaTime() * 30;
				float currentEnemyTimePos = sceneContainer.animHandler.enemyTimePos[i];
				int currentAnimIndex = sceneContainer.enemies[i].currentAnimIndex;
				int currentAnimationLength = sceneContainer.lavaEnemyFile.skinnedMeshes[0].hierarchy[0].Animations[currentAnimIndex].Length;

				if (currentEnemyTimePos >= currentAnimationLength) {

					sceneContainer.animHandler.enemyTimePos[i] = 0.0f;
				}

				sceneContainer.animHandler.UpdateEnemyAnimation(sceneContainer.gHandler.gDeviceContext, sceneContainer.lavaEnemyFile, i, currentAnimIndex, sceneContainer.animHandler.enemyTimePos[i]);

			}

		}

	}

	sceneContainer.animHandler.MapEnemyAnimations(sceneContainer.gHandler.gDeviceContext, sceneContainer.nrOfEnemies);

}

void lavamovmentUpdate()
{
	SAFE_RELEASE(sceneContainer.lava.LavaVB);
	sceneContainer.lava.swap(timer.getFrameCount(), sceneContainer.gHandler.gDevice);
}

void updateLava()
{
	sceneContainer.lava.LoadRawFile();
	//sceneContainer.lava.VBuffer(sceneContainer.gHandler.gDevice, sceneContainer.lava.swap(timer.getFrameCount()));
	sceneContainer.lava.IBuffer(sceneContainer.gHandler.gDevice);
}

void updateBuffers() 
{

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_MAPPED_SUBRESOURCE playerMappedResource;
	D3D11_MAPPED_SUBRESOURCE EnemyMappedResource;
	D3D11_MAPPED_SUBRESOURCE platformMappedResource;
	D3D11_MAPPED_SUBRESOURCE ProjectileMappedResource;

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	ZeroMemory(&playerMappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	ZeroMemory(&EnemyMappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	ZeroMemory(&platformMappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	ZeroMemory(&ProjectileMappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	XMMATRIX tCameraViewProj = XMMatrixTranspose(sceneContainer.character.camera.ViewProj());
	XMMATRIX tCameraInverseViewProj = XMMatrixTranspose(XMMatrixInverse(nullptr, sceneContainer.character.camera.ViewProj()));
	XMMATRIX tCameraProjection = XMMatrixTranspose(sceneContainer.character.camera.Proj());
	XMMATRIX tCameraView = XMMatrixTranspose(sceneContainer.character.camera.View());

	//----------------------------------------------------------------------------------------------------------------------------------//
	// CONSTANT BUFFER UPDATE
	//----------------------------------------------------------------------------------------------------------------------------------//

	sceneContainer.gHandler.gDeviceContext->Map(sceneContainer.bHandler.gConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	GS_CONSTANT_BUFFER* cBufferPointer = (GS_CONSTANT_BUFFER*)mappedResource.pData;

	cBufferPointer->worldViewProj = (sceneContainer.bHandler.tWorldMatrix  * tCameraViewProj);
	cBufferPointer->matrixWorld = sceneContainer.bHandler.tWorldMatrix;
	cBufferPointer->matrixView = sceneContainer.bHandler.tWorldMatrix * tCameraView;
	cBufferPointer->matrixProjection = tCameraProjection;
	cBufferPointer->fortressWorldMatrix = XMMatrixTranspose(sceneContainer.bHandler.fortressWorld);
	cBufferPointer->lightViewProj = sceneContainer.bHandler.tLightViewProj;
	cBufferPointer->cameraPos = sceneContainer.character.camera.GetPosition();

	sceneContainer.gHandler.gDeviceContext->Unmap(sceneContainer.bHandler.gConstantBuffer, 0);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// CHARACTER TRANSFORM BUFFER UPDATE
	//----------------------------------------------------------------------------------------------------------------------------------//

	sceneContainer.gHandler.gDeviceContext->Map(sceneContainer.bHandler.gPlayerTransformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &playerMappedResource);

	PLAYER_TRANSFORM* playerTransformPointer = (PLAYER_TRANSFORM*)playerMappedResource.pData;

	XMMATRIX tCharacterTranslation = XMMatrixTranspose(sceneContainer.character.tPlayerTranslation);

	playerTransformPointer->matrixW = tCharacterTranslation;

	playerTransformPointer->matrixWVP = tCameraViewProj * tCharacterTranslation;

	sceneContainer.gHandler.gDeviceContext->Unmap(sceneContainer.bHandler.gPlayerTransformBuffer, 0);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// ENEMY TRANSFORM BUFFER UPDATE
	//----------------------------------------------------------------------------------------------------------------------------------//

	sceneContainer.gHandler.gDeviceContext->Map(sceneContainer.bHandler.gEnemyTransformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &EnemyMappedResource);

	ENEMY_TRANSFORM* EnemyTransformPointer = (ENEMY_TRANSFORM*)EnemyMappedResource.pData;

	for(UINT i = 0; i < sceneContainer.nrOfEnemies; i++){

		EnemyTransformPointer->matrixW[i] = XMMatrixTranspose(sceneContainer.enemies[i].tPlayerTranslation);

	}

	sceneContainer.gHandler.gDeviceContext->Unmap(sceneContainer.bHandler.gEnemyTransformBuffer, 0);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// PLATFORM INSTANCE BUFFER UPDATE
	//----------------------------------------------------------------------------------------------------------------------------------//

	sceneContainer.bHandler.updatePlatformWorldMatrices();

	sceneContainer.gHandler.gDeviceContext->Map(sceneContainer.bHandler.gInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &platformMappedResource);

	PLATFORM_INSTANCE_BUFFER* platformTransformPointer = (PLATFORM_INSTANCE_BUFFER*)platformMappedResource.pData;

	for (UINT i = 0; i < sceneContainer.bHandler.nrOfCubes; i++) 
	{
		platformTransformPointer->worldMatrix[i] = XMMatrixTranspose(sceneContainer.bHandler.cubeObjects[i].worldMatrix);
	}

	sceneContainer.gHandler.gDeviceContext->Unmap(sceneContainer.bHandler.gInstanceBuffer, 0);

	// Projectile -----------

	sceneContainer.gHandler.gDeviceContext->Map(sceneContainer.bHandler.gProjectileTransformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ProjectileMappedResource);

	PROJECTILE_TRANSFORM* ProjectileTransformPointer = (PROJECTILE_TRANSFORM*)ProjectileMappedResource.pData;

	for (UINT i = 0; i < sceneContainer.nrOfEnemies; i++){

		ProjectileTransformPointer->worldMatrix[i] = XMMatrixTranspose(sceneContainer.enemies[i].fireBall.worldMatrix);

		}

	sceneContainer.gHandler.gDeviceContext->Unmap(sceneContainer.bHandler.gProjectileTransformBuffer, 0);

}


