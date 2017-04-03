
#include "Render.h"

void clear(SceneContainer &sceneContainer) {

	// clear the back buffer to a deep blue
	float clearColor[] = { 0, 0, 1, 1 };	// Back buffer clear color as an array of floats (rgba)
	sceneContainer.gHandler.gDeviceContext->ClearRenderTargetView(sceneContainer.gHandler.gBackbufferRTV, clearColor);	// Clear the render target view using the specified color
	sceneContainer.gHandler.gDeviceContext->ClearDepthStencilView(sceneContainer.gHandler.depthView, D3D11_CLEAR_DEPTH, 1.0f, 0);	// Clear the depth stencil view
}

void render(SceneContainer &sceneContainer) {

	clear(sceneContainer);

	sceneContainer.drawPlane();

	sceneContainer.character.draw(sceneContainer.gHandler.gDeviceContext);

}