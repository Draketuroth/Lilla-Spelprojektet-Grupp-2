
#include "SceneContainer.h"

SceneContainer::SceneContainer() {

	gHandler = GraphicComponents();
	bHandler = BufferComponents();
	tHandler = TextureComponents();

	character = MainCharacter();
}

SceneContainer::~SceneContainer() {


}

void SceneContainer::releaseAll() {

	gHandler.ReleaseAll();
	bHandler.ReleaseAll();
	tHandler.ReleaseAll();

	character.releaseAll();
}

bool SceneContainer::initialize(HWND &windowHandle, Camera &mCam) {

	if (!gHandler.InitalizeDirect3DContext(windowHandle)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: DirectX couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	bHandler.SetupScene(gHandler.gDevice, mCam);

	if (!tHandler.CreateTexture(gHandler.gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Textures couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	character.createBuffers(gHandler.gDevice);

	return true;

}

void SceneContainer::drawPlane() {

	//----------------------------------------------------------------------------------------------------------------------------------//
	// STANDARD PIPELINE
	//----------------------------------------------------------------------------------------------------------------------------------//

	gHandler.gDeviceContext->VSSetShader(gHandler.gVertexShader, nullptr, 0);	// Setting the Vertex Shader 
	gHandler.gDeviceContext->GSSetShader(gHandler.gGeometryShader, nullptr, 0); // Setting the Geometry Shader 
	gHandler.gDeviceContext->PSSetShader(gHandler.gPixelShader, nullptr, 0); // Setting the Pixel Shader 
	gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer); // Setting the Constant Buffer for the Vertex Shader
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.standardResource);

	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	// The stride and offset must be stored in variables as we need to provide pointers to these when setting the vertex buffer
	UINT32 vertexSize = sizeof(TriangleVertex);
	UINT32 offset = 0;
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &bHandler.planeVertexBuffer, &vertexSize, &offset);

	// The input assembler will now recieve the vertices and the vertex layout

	// The vertices should be interpreted as parts of a triangle in the input assembler
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gVertexLayout);

	gHandler.gDeviceContext->Draw(6, 0);

}

void SceneContainer::clear()
{
	// clear the back buffer to a deep blue
	float clearColor[] = { 0, 0, 1, 1 };	// Back buffer clear color as an array of floats (rgba)
	gHandler.gDeviceContext->ClearRenderTargetView(gHandler.gBackbufferRTV, clearColor);	// Clear the render target view using the specified color
	gHandler.gDeviceContext->ClearDepthStencilView(gHandler.depthView, D3D11_CLEAR_DEPTH, 1.0f, 0);	// Clear the depth stencil view
}
void SceneContainer::renderScene()
{
	drawPlane();
}
void SceneContainer::renderCharacters()
{
	//we clear in here since characters are rendered before the scene
	//Characters need to be rendered first since they will be moving
	clear();
	character.draw(gHandler.gDeviceContext);
}