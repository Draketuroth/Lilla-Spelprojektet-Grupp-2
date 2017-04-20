
#include "SceneContainer.h"

SceneContainer::SceneContainer() {

	// Initialize handler and main character

	gHandler = GraphicComponents();
	bHandler = BufferComponents();
	tHandler = TextureComponents();

	character = MainCharacter();
	enemy = Enemy(0, { 0.3f,20,5 });

	bulletPhysicsHandler = BulletComponents();

	this->nrOfEnemies = 0;
}

SceneContainer::~SceneContainer() {

}

void SceneContainer::releaseAll() {

	gHandler.ReleaseAll();
	bHandler.ReleaseAll();
	tHandler.ReleaseAll();

	//character.releaseAll();
	enemy.releaseAll();

	deferredObject.ReleaseAll();
	deferredShaders.ReleaseAll();
	lightShaders.ReleaseAll();

	bulletPhysicsHandler.ReleaseAll();
	fbxImporter.ReleaseAll();
}

bool SceneContainer::initialize(HWND &windowHandle) {

	if (!WindowInitialize(windowHandle)) {

		// If the window cannot be created during startup, it's more known as a terminal error
		// The MessageBox function will display a message and inform us of the problem
		MessageBox(
			NULL,
			L"CRITICAL ERROR: Window couldn't be initialized, investigate window initializr function\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	if (!gHandler.InitalizeDirect3DContext(windowHandle)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: DirectX couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	bulletPhysicsHandler.InitializeBulletPhysics();

	if (!bHandler.SetupScene(gHandler.gDevice, bulletPhysicsHandler)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Primitives couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	if (!tHandler.CreateTexture(gHandler.gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Textures couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	if (!deferredObject.Initialize(gHandler.gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Deferred buffer couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	if (!deferredShaders.InitializeShader(gHandler.gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Deferred shader couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	if (!lightShaders.Initialize(gHandler.gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Light shaders couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	character.initialize(gHandler.gDevice, XMFLOAT3(2, 2, 5), bulletPhysicsHandler, fbxImporter);
	enemy.Spawn(gHandler.gDevice,bulletPhysicsHandler);

	return true;

}

void SceneContainer::update(HWND &windowHandle)
{
	//FIX LATER PLZ
	enemies[0] = enemy;
	nrOfEnemies = 1;

	character.meleeAttack(windowHandle, this->nrOfEnemies, enemies);

	render();
}

void SceneContainer::drawPlatforms() {

	gHandler.gDeviceContext->VSSetShader(gHandler.gPlatformVertexShader, nullptr, 0);
	gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);
	gHandler.gDeviceContext->GSSetShader(gHandler.gPlatformGeometryShader, nullptr, 0);

	gHandler.gDeviceContext->PSSetShader(gHandler.gPlatformPixelShader, nullptr, 0);
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.standardResource);
	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	UINT32 vertexSize = sizeof(TriangleVertex);
	UINT32 offset = 0;
	gHandler.gDeviceContext->IASetIndexBuffer(bHandler.gCubeIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gPlatformLayout);

	for (int i = 0; i < bHandler.nrOfCubes; i++) {

		if (bHandler.cubeObjects[i].renderCheck == true) {

			gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &bHandler.cubeObjects[i].gCubeVertexBuffer, &vertexSize, &offset);

			gHandler.gDeviceContext->DrawIndexed(36, 0, 0);

		}

	}
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

	//renderDeferred();

	renderCharacters();
	renderEnemies();
	renderScene();
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

	XMFLOAT3 lightDirection = { 1.0f, 1.0f, 0.0f };
	lightShaders.SetShaderParameters(gHandler.gDeviceContext,
									deferredObject.d_shaderResourceViewArray[0],
									deferredObject.d_shaderResourceViewArray[1],
									deferredObject.d_shaderResourceViewArray[2],
									deferredObject.d_depthResourceView,
									lightDirection);

	gHandler.gDeviceContext->PSSetConstantBuffers(1, 1, &bHandler.gConstantBuffer);
									
	lightShaders.Render(gHandler.gDeviceContext, deferredObject.ImportStruct.size());

	return true;
}

bool SceneContainer::renderSceneToTexture() {

	// Set the render buffers to be the render target
	deferredObject.SetRenderTargets(gHandler.gDeviceContext);

	// Clear the render buffers
	deferredObject.ClearRenderTargets(gHandler.gDeviceContext, 0.0f, 0.0f, 1.0f, 1.0f);

	// Set the object vertex buffer to prepare it for drawing
	deferredObject.SetObjectBuffer(gHandler.gDeviceContext);

	// Render the object using the deferred shader
	int indexCounter = deferredObject.ImportStruct.size();

	// Don't forget to set the constant buffer to the geometry shader
	gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);
	
	deferredShaders.Render(gHandler.gDeviceContext, tHandler.texSampler, tHandler.standardResource, indexCounter);

	return true;
	
}

void SceneContainer::renderScene() {

	drawPlatforms();
}

void SceneContainer::renderCharacters()
{

	gHandler.gDeviceContext->VSSetShader(gHandler.gVertexShader, nullptr, 0);
	gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);
	gHandler.gDeviceContext->GSSetConstantBuffers(1, 1, &bHandler.gPlayerTransformBuffer);
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &fbxImporter.gBoneBuffer);
	gHandler.gDeviceContext->GSSetShader(gHandler.gGeometryShader, nullptr, 0);

	gHandler.gDeviceContext->PSSetShader(gHandler.gPixelShader, nullptr, 0);
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.standardResource);
	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	UINT32 vertexSize = sizeof(Vertex_Bone);
	UINT32 offset = 0;

	ID3D11Buffer* nullBuffer = { nullptr };
	gHandler.gDeviceContext->IASetIndexBuffer(nullBuffer, DXGI_FORMAT_R32_UINT, 0);
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &fbxImporter.gBoneVertexBuffer, &vertexSize, &offset);

	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gVertexLayout);

	character.draw(gHandler.gDeviceContext, fbxImporter.vertices.size());

	character.resetWorldMatrix();
	
}

void SceneContainer::renderEnemies()
{
	
	gHandler.gDeviceContext->VSSetShader(gHandler.gEnemyVertexShader, nullptr, 0);
	gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);
	gHandler.gDeviceContext->GSSetConstantBuffers(1, 1, &bHandler.gEnemyTransformBuffer);
	gHandler.gDeviceContext->GSSetShader(gHandler.gEnemyGeometryShader, nullptr, 0);

	gHandler.gDeviceContext->PSSetShader(gHandler.gEnemyPixelShader, nullptr, 0);
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.standardResource);
	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);


	ID3D11Buffer* nullBuffer = { nullptr };
	gHandler.gDeviceContext->IASetIndexBuffer(nullBuffer, DXGI_FORMAT_R32_UINT, 0);

	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gVertexLayout);
	enemy.draw(gHandler.gDeviceContext);

	
}




