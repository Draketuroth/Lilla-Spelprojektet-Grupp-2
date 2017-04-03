
#include "SceneContainer.h"

SceneContainer::SceneContainer() {

	gHandler = GraphicComponents();
	bHandler = BufferComponents();
	tHandler = TextureComponents();
}

SceneContainer::~SceneContainer() {


}

void SceneContainer::releaseAll() {

	gHandler.ReleaseAll();
	bHandler.ReleaseAll();
	tHandler.ReleaseAll();
}

bool SceneContainer::initialize(HWND &windowHandle, Camera &mCam) {

	if (!gHandler.InitalizeDirect3DContext(windowHandle, bHandler)) {

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

	return true;

}