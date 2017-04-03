
#include "Render.h"

void Render(SceneContainer &sceneContainer) {

	// clear the back buffer to a deep blue
	float clearColor[] = { 0, 0, 1, 1 };	// Back buffer clear color as an array of floats (rgba)
	sceneContainer.gHandler.gDeviceContext->ClearRenderTargetView(sceneContainer.gHandler.gBackbufferRTV, clearColor);	// Clear the render target view using the specified color
	sceneContainer.gHandler.gDeviceContext->ClearDepthStencilView(sceneContainer.bHandler.depthView, D3D11_CLEAR_DEPTH, 1.0f, 0);	// Clear the depth stencil view

	RenderStandard(sceneContainer);

}

void RenderStandard(SceneContainer &sceneContainer) {

	//----------------------------------------------------------------------------------------------------------------------------------//
	// STANDARD PIPELINE
	//----------------------------------------------------------------------------------------------------------------------------------//
	
	sceneContainer.gHandler.gDeviceContext->VSSetShader(sceneContainer.gHandler.gVertexShader, nullptr, 0);	// Setting the Vertex Shader 
	sceneContainer.gHandler.gDeviceContext->GSSetShader(sceneContainer.gHandler.gGeometryShader, nullptr, 0); // Setting the Geometry Shader 
	sceneContainer.gHandler.gDeviceContext->PSSetShader(sceneContainer.gHandler.gPixelShader, nullptr, 0); // Setting the Pixel Shader 
	sceneContainer.gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &sceneContainer.bHandler.gConstantBuffer); // Setting the Constant Buffer for the Vertex Shader
	sceneContainer.gHandler.gDeviceContext->PSSetShaderResources(0, 1, &sceneContainer.tHandler.standardResource);

	sceneContainer.gHandler.gDeviceContext->PSSetSamplers(0, 1, &sceneContainer.tHandler.texSampler);

	// The stride and offset must be stored in variables as we need to provide pointers to these when setting the vertex buffer
	UINT32 vertexSize = sizeof(TriangleVertex);
	UINT32 offset = 0;
	sceneContainer.gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &sceneContainer.bHandler.gVertexBuffer, &vertexSize, &offset);

	// The input assembler will now recieve the vertices and the vertex layout

	// The vertices should be interpreted as parts of a triangle in the input assembler
	sceneContainer.gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	sceneContainer.gHandler.gDeviceContext->IASetInputLayout(sceneContainer.gHandler.gVertexLayout);

	sceneContainer.gHandler.gDeviceContext->Draw(6, 0);

	ID3D11ShaderResourceView* nullSRV = nullptr;

	sceneContainer.gHandler.gDeviceContext->PSSetShaderResources(0, 1, &nullSRV);

	/*gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &bHandler.vertexBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetIndexBuffer(bHandler.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	gHandler.gDeviceContext->DrawIndexed(36, 0, 0);*/


}