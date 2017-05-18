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
#include <iostream>
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
	sceneContainer.sceneTimer.initialize();
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
				sceneContainer.character.currentAnimIndex = 0;
				break;
			case QUIT_GAME:
				windowMessage.message = WM_QUIT;
				break;
			case START_GAME:
				//sceneContainer.character.setAlive(true);
				menuState.checkGameState();
				updateCharacter(windowHandle);
				updateEnemies();
				updateBuffers();
				lavamovmentUpdate();
				sceneContainer.bulletPhysicsHandler.bulletDynamicsWorld->stepSimulation(deltaTime);

				

				//----------------------------------------------------------------------------------------------------------------------------------//
				// PLAYER LAVA HIT DETECTION
				//----------------------------------------------------------------------------------------------------------------------------------//
				
				MyCharacterContactResultCallback characterCallBack(&sceneContainer.character);

				if (!sceneContainer.character.getAlive())
				{
					if (sceneContainer.character.getHealth() <= 0)
					{
						sceneContainer.character.DeathTimer();						
						if (sceneContainer.character.deathCountdown >= 2.0f)
						{
							sceneContainer.character.deathCountdown = 0.0f;
							menuState.state = GAME_OVER;	
						}	
					}	
					else
					{
						menuState.state = GAME_OVER;
					}
				}

				
				sceneContainer.bulletPhysicsHandler.bulletDynamicsWorld->contactPairTest(sceneContainer.bHandler.lavaPitRigidBody, sceneContainer.character.rigidBody, characterCallBack);

				//----------------------------------------------------------------------------------------------------------------------------------//
				// ENEMY LAVA HIT DETECTION
				//----------------------------------------------------------------------------------------------------------------------------------//

				for(UINT i = 0; i < sceneContainer.nrOfEnemies; i++){

					MyEnemyContactResultCallback enemyCallBack(sceneContainer.enemies[i]);
					sceneContainer.bulletPhysicsHandler.bulletDynamicsWorld->contactPairTest(sceneContainer.bHandler.lavaPitRigidBody, sceneContainer.enemies[i]->rigidBody, enemyCallBack);

					if (sceneContainer.enemies[i]->getAlive() == false) {

						sceneContainer.enemies[i]->releaseAll(sceneContainer.bulletPhysicsHandler.bulletDynamicsWorld);
					}

				}

				//----------------------------------------------------------------------------------------------------------------------------------//
				// RENDER
				//----------------------------------------------------------------------------------------------------------------------------------//
			
				sceneContainer.update(windowHandle, sceneContainer.animHandler.enemyTimePos, timer);

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
	float currentPlayerTimePos;
	int currentAnimIndex;
	int currentAnimationLength;

	if(sceneContainer.character.getAlive() == true){

		sceneContainer.character.update(windowhandle);

	}
	
	sceneContainer.character.camera.UpdateViewMatrix();	// Update Camera View and Projection Matrix for each frame

	if (sceneContainer.character.getAlive() == true){

		sceneContainer.character.playerAnimTimePos += timer.getDeltaTime() * 30;
		currentPlayerTimePos = sceneContainer.character.playerAnimTimePos;
		currentAnimIndex = sceneContainer.character.currentAnimIndex;
		currentAnimationLength = sceneContainer.mainCharacterFile.skinnedMeshes[0].hierarchy[0].Animations[currentAnimIndex].Length;

		if (currentPlayerTimePos >= currentAnimationLength) {

			if (sceneContainer.character.currentAnimIndex == 4) {

				sceneContainer.character.currentAnimIndex = 0;
				sceneContainer.character.shotFlag = false;
			}

			else if (sceneContainer.character.currentAnimIndex == 3) {

				sceneContainer.character.currentAnimIndex = 0;
				sceneContainer.character.attackFlag = false;
			}

			sceneContainer.character.playerAnimTimePos = 0.0f;
		}

	}

	else {

		sceneContainer.character.currentAnimIndex = 2;

		sceneContainer.character.playerAnimTimePos += timer.getDeltaTime() * 30;
		currentPlayerTimePos = sceneContainer.character.playerAnimTimePos;
		currentAnimIndex = sceneContainer.character.currentAnimIndex;
		currentAnimationLength = sceneContainer.mainCharacterFile.skinnedMeshes[0].hierarchy[0].Animations[currentAnimIndex].Length;

		if (currentPlayerTimePos >= currentAnimationLength) {

			sceneContainer.character.playerAnimTimePos = currentAnimationLength;
		}

	}

	sceneContainer.animHandler.UpdatePlayerAnimation(sceneContainer.gHandler.gDeviceContext, currentAnimIndex, sceneContainer.mainCharacterFile, currentPlayerTimePos);
}

void updateEnemies() {

	float currentEnemyTimePos;
	int currentAnimIndex;
	int currentAnimationLength;

	for (UINT i = 0; i < sceneContainer.nrOfEnemies; i++) {

			if (i < sceneContainer.nrOfIceEnemies){

				if (sceneContainer.enemies[i]->getAlive() == true) {
				// Update ice enemy physics
				XMMATRIX scaling = XMMatrixScaling(0.1, 0.1, 0.1);
				sceneContainer.enemies[i]->EnemyPhysics(sceneContainer.character.getPos(), scaling);

				// Update enemy animation time pose
				sceneContainer.animHandler.enemyTimePos[i] += timer.getDeltaTime() * 30;
				currentEnemyTimePos = sceneContainer.animHandler.enemyTimePos[i];
				currentAnimIndex = sceneContainer.enemies[i]->currentAnimIndex;
				currentAnimationLength = sceneContainer.iceEnemyFile.skinnedMeshes[0].hierarchy[0].Animations[currentAnimIndex].Length;

				if (currentEnemyTimePos >= currentAnimationLength) {

					if (sceneContainer.enemies[i]->currentAnimIndex == 3){

						sceneContainer.enemies[i]->currentAnimIndex = 0;
						sceneContainer.enemies[i]->attackFlag = false;
					}

					sceneContainer.animHandler.enemyTimePos[i] = 0.0f;
				}

				}

				else {

					sceneContainer.enemies[i]->currentAnimIndex = 2;

					// Update enemy animation time pose
					sceneContainer.animHandler.enemyTimePos[i] += timer.getDeltaTime() * 30;
					currentEnemyTimePos = sceneContainer.animHandler.enemyTimePos[i];
					currentAnimIndex = sceneContainer.enemies[i]->currentAnimIndex;
					currentAnimationLength = sceneContainer.iceEnemyFile.skinnedMeshes[0].hierarchy[0].Animations[currentAnimIndex].Length;

					if (currentEnemyTimePos >= currentAnimationLength) {

						sceneContainer.animHandler.enemyTimePos[i] = currentAnimationLength;
						sceneContainer.enemies[i]->tPlayerTranslation = XMMatrixTranslation(0.0f, -100.0f, 0.0f);

					}
				}

				sceneContainer.animHandler.UpdateEnemyAnimation(sceneContainer.gHandler.gDeviceContext, sceneContainer.iceEnemyFile, i, currentAnimIndex, sceneContainer.animHandler.enemyTimePos[i]);

			}

			else if (i >= sceneContainer.nrOfIceEnemies) {

				if (sceneContainer.enemies[i]->getAlive() == true) {

					// Update lava enemy physics
					XMMATRIX scaling = XMMatrixScaling(0.3, 0.3, 0.3);
					sceneContainer.enemies[i]->EnemyPhysics(sceneContainer.character.getPos(), scaling);

					// Update enemy animation time pose
					sceneContainer.animHandler.enemyTimePos[i] += timer.getDeltaTime() * 80;
					currentEnemyTimePos = sceneContainer.animHandler.enemyTimePos[i];
					currentAnimIndex = sceneContainer.enemies[i]->currentAnimIndex;
					currentAnimationLength = sceneContainer.lavaEnemyFile.skinnedMeshes[0].hierarchy[0].Animations[currentAnimIndex].Length;

					if (currentEnemyTimePos >= currentAnimationLength) {

						sceneContainer.animHandler.enemyTimePos[i] = 0.0f;
					}

				}

				else {

					sceneContainer.enemies[i]->currentAnimIndex = 2;

					// Update lava enemy physics
					XMMATRIX scaling = XMMatrixScaling(0.3, 0.3, 0.3);
					sceneContainer.enemies[i]->EnemyPhysics(sceneContainer.character.getPos(), scaling);

					// Update enemy animation time pose
					sceneContainer.animHandler.enemyTimePos[i] += timer.getDeltaTime() * 80;
					currentEnemyTimePos = sceneContainer.animHandler.enemyTimePos[i];
					currentAnimIndex = sceneContainer.enemies[i]->currentAnimIndex;
					currentAnimationLength = sceneContainer.lavaEnemyFile.skinnedMeshes[0].hierarchy[0].Animations[currentAnimIndex].Length;

					if (currentEnemyTimePos >= currentAnimationLength) {

						sceneContainer.animHandler.enemyTimePos[i] = currentAnimationLength;
						sceneContainer.enemies[i]->tPlayerTranslation = XMMatrixTranslation(0.0f, -100.0f, 0.0f);
					}

				}

				sceneContainer.animHandler.UpdateEnemyAnimation(sceneContainer.gHandler.gDeviceContext, sceneContainer.lavaEnemyFile, i, currentAnimIndex, sceneContainer.animHandler.enemyTimePos[i]);

		}

	}

	sceneContainer.animHandler.MapIceEnemyAnimations(sceneContainer.gHandler.gDeviceContext, sceneContainer.nrOfIceEnemies);
	sceneContainer.animHandler.MapLavaEnemyAnimations(sceneContainer.gHandler.gDeviceContext, sceneContainer.nrOfIceEnemies, sceneContainer.nrOfEnemies);

}

void lavamovmentUpdate()
{
	SAFE_RELEASE(sceneContainer.lava.LavaVB);
	sceneContainer.lava.swap(timer.getFrameCount(), sceneContainer.gHandler.gDevice);
}

void updateLava()
{
	sceneContainer.lava.LoadRawFile();

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
	// ICE ENEMY TRANSFORM BUFFER UPDATE
	//----------------------------------------------------------------------------------------------------------------------------------//

	sceneContainer.gHandler.gDeviceContext->Map(sceneContainer.bHandler.gIceEnemyTransformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &EnemyMappedResource);

	ICE_ENEMY_TRANSFORM* iceEnemyTransformPointer = (ICE_ENEMY_TRANSFORM*)EnemyMappedResource.pData;

	for(UINT i = 0; i < sceneContainer.nrOfIceEnemies; i++){

		iceEnemyTransformPointer->matrixW[i] = XMMatrixTranspose(sceneContainer.enemies[i]->tPlayerTranslation);

	}

	sceneContainer.gHandler.gDeviceContext->Unmap(sceneContainer.bHandler.gIceEnemyTransformBuffer, 0);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// LAVA ENEMY TRANSFORM BUFFER UPDATE
	//----------------------------------------------------------------------------------------------------------------------------------//
	ZeroMemory(&EnemyMappedResource, sizeof(EnemyMappedResource));

	sceneContainer.gHandler.gDeviceContext->Map(sceneContainer.bHandler.gLavaEnemyTransformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &EnemyMappedResource);

	LAVA_ENEMY_TRANSFORM* lavaEnemyTransformPointer = (LAVA_ENEMY_TRANSFORM*)EnemyMappedResource.pData;

	int lavaEnemyIndex = 0;
	int offsetStart = sceneContainer.nrOfIceEnemies;
	for (UINT i = offsetStart; i < sceneContainer.nrOfEnemies; i++) {

		lavaEnemyTransformPointer->matrixW[lavaEnemyIndex] = XMMatrixTranspose(sceneContainer.enemies[i]->tPlayerTranslation);
		lavaEnemyIndex++;

	}

	sceneContainer.gHandler.gDeviceContext->Unmap(sceneContainer.bHandler.gLavaEnemyTransformBuffer, 0);

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
	int index = 0;
	for (UINT i = sceneContainer.nrOfIceEnemies; i < sceneContainer.nrOfEnemies; i++)
	{

		ProjectileTransformPointer->worldMatrix[index] = XMMatrixTranspose(sceneContainer.enemies[i]->fireBall.worldMatrix);
		
		index++;
	}

	sceneContainer.gHandler.gDeviceContext->Unmap(sceneContainer.bHandler.gProjectileTransformBuffer, 0);

}

