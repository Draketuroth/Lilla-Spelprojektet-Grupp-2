#include "GameState.h"

GameState::GameState()
{
	this->state = MAIN_MENU;

	soundBuffer.loadFromFile("Sounds//click.wav");
	arenaMusic.openFromFile("Sounds//music.wav");
	clickSound.setBuffer(soundBuffer);

}

GameState::~GameState()
{

}

int GameState::menuHandler(HWND windowHandle, SceneContainer scene, MSG windowMessage)
{
	if (this->state == MAIN_MENU)
	{
		mainMenu(windowHandle, scene);
	}
	else if (this->state == PAUSE_MENU)
	{
		pauseMenu(windowHandle, scene, windowMessage);
	}
	else if (this->state == GAME_OVER)
	{
		gameOver(windowHandle, scene);
	}
	return state;
}

int GameState::mainMenu(HWND windowHandle, SceneContainer scene)
{
	if (this->state == MAIN_MENU)
	{
		getMousePos(windowHandle, scene);
		if (this->floatMouse.x <= 0.35f && this->floatMouse.x >= -0.3f && this->floatMouse.y <= 0.65f && this->floatMouse.y >= 0.13f)
		{
			scene.gHandler.gDeviceContext->PSSetShaderResources(0, 1, &scene.tHandler.menuResources[1]);
		}
		else if (this->floatMouse.x <= 0.35f && this->floatMouse.x >= -0.3f && this->floatMouse.y <= 0.03f && this->floatMouse.y >= -0.47f)
		{
			scene.gHandler.gDeviceContext->PSSetShaderResources(0, 1, &scene.tHandler.menuResources[2]);
		}
		else
		{
			scene.gHandler.gDeviceContext->PSSetShaderResources(0, 1, &scene.tHandler.menuResources[0]);
		}

		renderMenus(scene);
		if (GetAsyncKeyState(VK_LBUTTON))
		{			
			getMousePos(windowHandle, scene);
			cout << "Mouse X: " << this->floatMouse.x << "  Mouse Y: " << this->floatMouse.y << endl;
			if (this->floatMouse.x <= 0.35f && this->floatMouse.x >= -0.3f && this->floatMouse.y <= 0.65f && this->floatMouse.y >= 0.13f)
			{
				//clickSound.play();
				//arenaMusic.play();
				this->state = START_GAME;
			}
			if (this->floatMouse.x <= 0.35f && this->floatMouse.x >= -0.3f && this->floatMouse.y <= 0.03f && this->floatMouse.y >= -0.47f)
			{
				this->state = QUIT_GAME;
			}
		}
	}
	return state;
}

int GameState::pauseMenu(HWND windowHandle, SceneContainer scene, MSG windowMessage)
{
	if (this->state == PAUSE_MENU)
	{
		//arenaMusic.pause();
		getMousePos(windowHandle, scene);
		if (this->floatMouse.x <= 0.35f && this->floatMouse.x >= -0.3f && this->floatMouse.y <= 0.65f && this->floatMouse.y >= 0.13f)
		{
			scene.gHandler.gDeviceContext->PSSetShaderResources(0, 1, &scene.tHandler.menuResources[4]);

		}
		else if (this->floatMouse.x <= 0.35f && this->floatMouse.x >= -0.3f && this->floatMouse.y <= 0.03f && this->floatMouse.y >= -0.47f)
		{
			scene.gHandler.gDeviceContext->PSSetShaderResources(0, 1, &scene.tHandler.menuResources[5]);
		}
		else
		{
			scene.gHandler.gDeviceContext->PSSetShaderResources(0, 1, &scene.tHandler.menuResources[3]);
		}
		renderMenus(scene);
		if (GetAsyncKeyState(VK_LBUTTON))
		{
			getMousePos(windowHandle, scene);
			if (this->floatMouse.x <= 0.35f && this->floatMouse.x >= -0.3f && this->floatMouse.y <= 0.65f && this->floatMouse.y >= 0.13f)
			{
				//clickSound.play();
				//arenaMusic.play();
				this->state = START_GAME;
			}
			if (this->floatMouse.x <= 0.35f && this->floatMouse.x >= -0.3f && this->floatMouse.y <= 0.03f && this->floatMouse.y >= -0.47f)
			{
				this->state = QUIT_GAME;
			}
		}
	}

	return state;
}

int GameState::gameOver(HWND windowHandle, SceneContainer scene)
{
	if (this->state == GAME_OVER)
	{
		//arenaMusic.stop();
		getMousePos(windowHandle, scene);
		if (this->floatMouse.x <= 0.35f && this->floatMouse.x >= -0.3f && this->floatMouse.y <= 0.65f && this->floatMouse.y >= 0.13f)
		{
			scene.gHandler.gDeviceContext->PSSetShaderResources(0, 1, &scene.tHandler.menuResources[7]);

		}
		else if (this->floatMouse.x <= 0.35f && this->floatMouse.x >= -0.3f && this->floatMouse.y <= 0.03f && this->floatMouse.y >= -0.47f)
		{
			scene.gHandler.gDeviceContext->PSSetShaderResources(0, 1, &scene.tHandler.menuResources[8]);
		}
		else
		{
			scene.gHandler.gDeviceContext->PSSetShaderResources(0, 1, &scene.tHandler.menuResources[6]);
		}

		renderMenus(scene);
		if (GetAsyncKeyState(VK_LBUTTON))
		{
			getMousePos(windowHandle, scene);
			if (this->floatMouse.x <= 0.3f && this->floatMouse.x >= -0.3f && this->floatMouse.y <= 0.65f && this->floatMouse.y >= 0.13f)
			{
				//clickSound.play();
				//arenaMusic.play();
				this->state = START_GAME;
			}
			if (this->floatMouse.x <= 0.35f && this->floatMouse.x >= -0.3f && this->floatMouse.y <= 0.03f && this->floatMouse.y >= -0.47f)
			{
				this->state = QUIT_GAME;
			}

		}
	}
	return state;
}

void GameState::renderMenus(SceneContainer scene)
{
	scene.clear();

	scene.gHandler.gDeviceContext->VSSetShader(scene.gHandler.gMenuVertex, nullptr, 0);
	scene.gHandler.gDeviceContext->PSSetShader(scene.gHandler.gMenuPixel, nullptr, 0);
	//scene.gHandler.gDeviceContext->PSSetShaderResources(0, 1, &scene.tHandler.menuResources[0]);
	scene.gHandler.gDeviceContext->PSSetSamplers(0, 1, &scene.tHandler.texSampler);
	scene.gHandler.gDeviceContext->VSSetConstantBuffers(0, 0, nullptr);
	scene.gHandler.gDeviceContext->GSSetShader(nullptr, nullptr, 0);


	scene.gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	scene.gHandler.gDeviceContext->IASetInputLayout(scene.gHandler.gMenuLayout);

	scene.gHandler.gDeviceContext->Draw(4, 0);
	//Don't forget the swapchain, måste ha för att byta mellan det som ritas och det som visas
	scene.gHandler.gSwapChain->Present(0, 0);


}

void GameState::getMousePos(HWND windowHandle, SceneContainer scene)
{
	GetCursorPos(&this->mousePos);
	ScreenToClient(windowHandle, &this->mousePos);
	this->floatMouse.x = mousePos.x;
	this->floatMouse.y = mousePos.y;
	this->floatMouse.x = (2 * this->floatMouse.x) / WIDTH - 1;
	this->floatMouse.y = -(2 * this->floatMouse.y) / HEIGHT + 1;
}

void GameState::checkGameState()
{
	if (this->state == START_GAME)
	{
		if (GetAsyncKeyState('P'))
		{
			this->state = PAUSE_MENU;
		}
	}
}