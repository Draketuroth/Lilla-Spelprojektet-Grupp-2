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
	//HÄR UPPDATERAS MOVEMENT, ANIMATION...
	//playIdleAnimation(); ?
	keyEvent();

	//draw(); ?
}
void MainCharacter::movement()
{
	//Check if character is going to move
	keyEvent();
	//check if the character should rotate
	rotate();
}

void MainCharacter::keyEvent()
{
	if (GetAsyncKeyState('W') & 0x8000) {

		walkZ(getMovementSpeed() * timer.getDeltaTime());
		//animation
	}

	if (GetAsyncKeyState('S') & 0x8000) {

		walkZ(-getMovementSpeed() * timer.getDeltaTime());
		//animation
	}

	if (GetAsyncKeyState('A') & 0x8000) {

		walkX(-getMovementSpeed() * timer.getDeltaTime());
		//animation
	}

	if (GetAsyncKeyState('D') & 0x8000) {

		walkX(getMovementSpeed() * timer.getDeltaTime());
		//animation
	}
}

//--------- Changing the character's position --------------
//Strafe
void MainCharacter::walkX(float deltaTime) {

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


	/*mPosition += deltaTime * mRight
	//If creating a vector from a single scalar variable, we use VectorReplicate
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));*/
}
//Walk
void MainCharacter::walkZ(float deltaTime) {

	XMFLOAT3 forward = camera.GetLook();

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