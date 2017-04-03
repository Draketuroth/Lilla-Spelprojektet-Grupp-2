//----------------------------------------------------------------------------------------------------------------------------------//
// main.cpp
// DV1541 3D PROGMRAMMING: PROJECT
//
// THIS APPLICATION IS CREATED BY THE FOLLOWING GROUP MEMBERS FROM THE TA15 CLASS:
// Philip Velandria, Jonathan Sundberg, Linnea Vajda, Fredrik Linde
//----------------------------------------------------------------------------------------------------------------------------------//


#include "SceneContainer.h"

#include "Render.h" 

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace std;

//----------------------------------------------------------------------------------------------------------------------------------//
// WINDOW HANDLE AND CAMERA
//----------------------------------------------------------------------------------------------------------------------------------//
HWND windowHandle;
Camera mCam;

//----------------------------------------------------------------------------------------------------------------------------------//
// PIPELINE COMPONENTS
//----------------------------------------------------------------------------------------------------------------------------------//
SceneContainer sceneContainer;

Timer timer;

//----------------------------------------------------------------------------------------------------------------------------------//
// FORWARD DECLARATIONS
//----------------------------------------------------------------------------------------------------------------------------------//
int RunApplication();


int main() {

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// Memory leak detection flag

	// We always want to keep our eyes open for terminal errors, which mainly occur when the window isn't created

	if (!WindowInitialize(windowHandle)) {

		// If the window cannot be created during startup, it's more known as a terminal error
		// The MessageBox function will display a message and inform us of the problem
		MessageBox(
			NULL,
			L"CRITICAL ERROR: Window couldn't be initialized, investigate window initializr function\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	sceneContainer.initialize(windowHandle, mCam);

	return RunApplication();
}

int RunApplication() {

	//----------------------------------------------------------------------------------------------------------------------------------//
	// PREDEFINED VARIABLES
	//----------------------------------------------------------------------------------------------------------------------------------//

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// Memory leak detection flag

	MSG windowMessage = { 0 };

	SetCapture(windowHandle);

	SetCursorPos(WIDTH / 2, HEIGHT / 2);

	mCam.mLastMousePos.x = 0;
	mCam.mLastMousePos.y = 0;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	timer.initialize();

	//----------------GAME LOOP----------------------------
	while (windowMessage.message != WM_QUIT) {

		if (PeekMessage(&windowMessage, NULL, NULL, NULL, PM_REMOVE)) {

			TranslateMessage(&windowMessage);
			DispatchMessage(&windowMessage);
		}

		// If there are no messages to handle, the application will continue by running a frame
		else {

			float deltaTime = timer.getDeltaTime();

			//update camera
			mCam.cameraUpdate(deltaTime);
			mCam.UpdateViewMatrix();	// Update Camera View and Projection Matrix for each frame

			XMMATRIX tCameraViewProj = XMMatrixTranspose(mCam.ViewProj());	// Camera View Projection Matrix
			XMMATRIX tCameraProjection = XMMatrixTranspose(mCam.Proj());
			XMMATRIX tCameraView = XMMatrixTranspose(mCam.View());		// Camera View Matrix

			//----------------------------------------------------------------------------------------------------------------------------------//
			// CONSTANT BUFFER UPDATE
			//----------------------------------------------------------------------------------------------------------------------------------//

			// Here we disable GPU access to the vertex buffer data so I can change it on the CPU side and update it by sending it back when finished
			sceneContainer.gHandler.gDeviceContext->Map(sceneContainer.bHandler.gConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			// We create a pointer to the constant buffer containing the world matrix that requires to be multiplied with the rotation matrix

			GS_CONSTANT_BUFFER* cBufferPointer = (GS_CONSTANT_BUFFER*)mappedResource.pData;

			// Here We access the world matrix and update it. The angle of the rotation matrix is updated for every frame with a rotation matrix 
			// constructed to rotate the triangles around the y-axis

			// Both matrices must recieve the same treatment from the rotation matrix, no matter if we want to preserve its original space or not
			cBufferPointer->worldViewProj = (sceneContainer.bHandler.tWorldMatrix  * tCameraViewProj);
			cBufferPointer->matrixWorld = sceneContainer.bHandler.tWorldMatrix;
			cBufferPointer->matrixView = sceneContainer.bHandler.tWorldMatrix * tCameraView;
			cBufferPointer->matrixProjection = tCameraProjection;

			cBufferPointer->cameraPos = mCam.GetPosition();

			// At last we have to reenable GPU access to the vertex buffer data
			sceneContainer.gHandler.gDeviceContext->Unmap(sceneContainer.bHandler.gConstantBuffer, 0);

			//----------------------------------------------------------------------------------------------------------------------------------//
			// RENDER
			//----------------------------------------------------------------------------------------------------------------------------------//

			// Now we can render using the new updated buffers on the GPU
			render(sceneContainer);

			// When everythig has been drawn out, finish by presenting the final result on the screen by swapping between the back and front buffers
			sceneContainer.gHandler.gSwapChain->Present(0, 0); // Change front and back buffer

			timer.updateCurretTime();

		}

	}

	sceneContainer.releaseAll();

	DestroyWindow(windowHandle);

	return 0;
}


