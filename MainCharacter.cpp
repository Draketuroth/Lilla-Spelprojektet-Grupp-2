#include "MainCharacter.h"



MainCharacter::MainCharacter()
	:CharacterBase(true, 10, 3.0f, 1)
{

}


MainCharacter::~MainCharacter()
{
}


void MainCharacter::update()
{
	movement();

	//draw() happens in SceneContainer, 
	//characters are rendered before the rest of the scene
}
void MainCharacter::movement()
{
	float deltaTime = timer.getDeltaTime();
	XMFLOAT3 direction = getWalkDirection();

	direction.x *= deltaTime;
	direction.y *= deltaTime;
	direction.z *= deltaTime;

	move(direction);


}

XMFLOAT3 MainCharacter::getWalkDirection()
{
	XMFLOAT3 direction = { 0,0,0 };

	if (GetAsyncKeyState('W') & 0x8000) {

		direction.z += 1.0;
	}

	if (GetAsyncKeyState('S') & 0x8000) {

		direction.z -= 1.0;
	}

	if (GetAsyncKeyState('A') & 0x8000) {

		direction.x += 1.0;
	}

	if (GetAsyncKeyState('D') & 0x8000) {

		direction.x -= 1.0;
	}

	return direction;
}

//--------- Changing the character's position --------------

//Strafe
void MainCharacter::walkX(float deltaTime) {
	//the camera will be locked, and will not rotate so we can use the camera's right as x-axis
	XMFLOAT3 right = camera.GetRight();

	//Will be locked on the x-axis.
	//It will only allow the character to move left and right.

	XMFLOAT3 position = getPos();
	position.x += deltaTime * right.x; 
	position.y += deltaTime * right.y;
	position.z += deltaTime * right.z;

	//What does S stand for?
	XMVECTOR s = XMVectorReplicate(deltaTime);
	//Right
	XMVECTOR r = XMLoadFloat3(&right);
	//Position
	XMVECTOR p = XMLoadFloat3(&position);
	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, r, p));
	
	//set position?

}
//Walk
void MainCharacter::walkZ(float deltaTime) {

	//Instead of getLook, we should get the z-axis.
	XMFLOAT3 forward = { 0, 0, 1 };

	XMFLOAT3 position = getPos();
	position.x += deltaTime * forward.x;
	position.y += deltaTime * forward.y;
	position.z += deltaTime * forward.z;

	//What does S stand for?
	XMVECTOR s = XMVectorReplicate(deltaTime);
	//Forward
	XMVECTOR f = XMLoadFloat3(&forward);
	//Position
	XMVECTOR p = XMLoadFloat3(&position);
	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, f, p));

	//how to set the position?
}
//Rotate character
void MainCharacter::rotate()
{
	POINT p;
	GetCursorPos(&p);
	XMFLOAT3 right = camera.GetRight();
	XMFLOAT3 forward = camera.GetLook();
	XMFLOAT3 up = camera.GetUp();

	if (MK_LBUTTON) {

		//Where the mouse is, is where to character will face. How to solve?
		//We have z(forward), x (right) and y (which is up). 
		//The character will rotate around y.

		/*float dx = XMConvertToRadians(0.25f * static_cast<float>(p.x - camera.mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(p.y - mLastMousePos.y));
		XMMATRIX R = XMMatrixRotationY(dx); 

		//-------What does this do?------------
		XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), R));
		XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
		XMStoreFloat3(&forward, XMVector3TransformNormal(XMLoadFloat3(&forward), R));*/
	}

	//The mouse position is the new Point?
	camera.mLastMousePos.x = p.x;
	camera.mLastMousePos.y = p.y;
}