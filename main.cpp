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
				//sceneContainer.character.rig
				//----------------------------------------------------------------------------------------------------------------------------------//
				// RENDER
				//----------------------------------------------------------------------------------------------------------------------------------//

				sceneContainer.render();

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

void updateCharacter(HWND windowhandle) {

	sceneContainer.character.update(windowhandle);
	
	sceneContainer.character.camera.UpdateViewMatrix();	// Update Camera View and Projection Matrix for each frame
}

void updateBuffers() {

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_MAPPED_SUBRESOURCE playerMappedResource;
	ZeroMemory(&playerMappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	ZeroMemory(&playerMappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	XMMATRIX tCameraViewProj = XMMatrixTranspose(sceneContainer.character.camera.ViewProj());
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
}


