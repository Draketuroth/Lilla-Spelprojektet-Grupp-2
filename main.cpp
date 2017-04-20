//----------------------------------------------------------------------------------------------------------------------------------//
// main.cpp
// DV1541 3D PROGMRAMMING: PROJECT
//
// THIS APPLICATION IS CREATED BY THE FOLLOWING GROUP MEMBERS FROM THE TA15 CLASS:
// Philip Velandria, Jonathan Sundberg, Linnea Vajda, Fredrik Linde
//----------------------------------------------------------------------------------------------------------------------------------//


#include "SceneContainer.h"
#include "GameState.h"
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
void updateBuffers();

int main() {

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// Memory leak detection flag

	// We always want to keep our eyes open for terminal errors, which mainly occur when the window isn't created
	
	sceneContainer.initialize(windowHandle);

	return RunApplication();
}

int RunApplication() {

	//----------------------------------------------------------------------------------------------------------------------------------//
	// INITIALIZE
	//----------------------------------------------------------------------------------------------------------------------------------//

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// Memory leak detection flag
	
	MSG windowMessage = { 0 };

	SetCapture(windowHandle);

	SetCursorPos(WIDTH / 2, HEIGHT / 2);

	timer.initialize();
	sceneContainer.character.timer.initialize();

	menuState.createBufferData(sceneContainer.gHandler.gDevice);
	menuState.createIndexBuffer(sceneContainer.gHandler.gDevice);
	menuState.createVertexBuffer(sceneContainer.gHandler.gDevice);

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
				menuState.menuHandler(windowHandle, sceneContainer, windowMessage);
				break;
			case PAUSE_MENU:
				menuState.menuHandler(windowHandle, sceneContainer, windowMessage);
				break;
			case START_GAME:
				menuState.checkGameState();
				updateCharacter(windowHandle);
				updateBuffers();

				sceneContainer.bulletPhysicsHandler.bulletDynamicsWorld->stepSimulation(deltaTime);

				if(GetAsyncKeyState('L')) {

					SAFE_RELEASE(sceneContainer.bHandler.cubeObjects[14].gCubeVertexBuffer);
					sceneContainer.bulletPhysicsHandler.bulletDynamicsWorld->removeCollisionObject(sceneContainer.bulletPhysicsHandler.rigidBodies[14]);
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
	menuState.releaseAll();
	DestroyWindow(windowHandle);

	return 0;
}

void updateCharacter(HWND windowhandle) 
{

	sceneContainer.character.update(windowhandle);
	sceneContainer.enemy.EnemyPhysics();
	
	sceneContainer.character.camera.UpdateViewMatrix();	// Update Camera View and Projection Matrix for each frame

	sceneContainer.fbxImporter.animTimePos += timer.getDeltaTime() * 50;

	if (sceneContainer.fbxImporter.animTimePos >= sceneContainer.fbxImporter.meshSkeleton.hierarchy[0].Animations[sceneContainer.character.currentAnimIndex].Length) {

		sceneContainer.fbxImporter.animTimePos = 0.0f;
	}

	sceneContainer.fbxImporter.UpdateAnimation(sceneContainer.gHandler.gDeviceContext, sceneContainer.character.currentAnimIndex);
}

void updateBuffers() 
{

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_MAPPED_SUBRESOURCE playerMappedResource;
	D3D11_MAPPED_SUBRESOURCE EnemyMappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	ZeroMemory(&playerMappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	ZeroMemory(&EnemyMappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));


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
	cBufferPointer->inverseViewProjection = tCameraInverseViewProj;

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

	PLAYER_TRANSFORM* EnemyTransformPointer = (PLAYER_TRANSFORM*)EnemyMappedResource.pData;

	XMMATRIX tEnemyTranslation = XMMatrixTranspose(sceneContainer.enemy.tPlayerTranslation);

	EnemyTransformPointer->matrixW = tEnemyTranslation;

	EnemyTransformPointer->matrixWVP = tCameraViewProj * tEnemyTranslation;

	sceneContainer.gHandler.gDeviceContext->Unmap(sceneContainer.bHandler.gEnemyTransformBuffer, 0);
}


