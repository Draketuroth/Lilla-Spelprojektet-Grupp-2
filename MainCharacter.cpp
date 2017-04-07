#include "MainCharacter.h"



MainCharacter::MainCharacter()
	:CharacterBase(true, 10, 1.0f, 1, {0, 0, 0}, XMMatrixIdentity())
{
	
}


MainCharacter::~MainCharacter()
{
}


void MainCharacter::update()
{
	CharacterMove();
}
void MainCharacter::CharacterMove()
{
	float time = timer.getDeltaTime();

	XMFLOAT3 direction = { 0, 0, 0 };
	XMVECTOR positionVec = XMLoadFloat3(&this->getPos());
	XMFLOAT3 floatPos = { 0, 0, 0 };
	XMFLOAT3 oldpos = this->getPos();

	updateWorldMatrix(oldpos);

	if (GetAsyncKeyState('W') & 0x8000) {

		direction.z = 1.0;
		XMVECTOR directionVec = XMLoadFloat3(&direction);
		positionVec += directionVec * time * this->getMovementSpeed();
		XMStoreFloat3(&floatPos, positionVec);
		this->setPos(floatPos);
	}

	if (GetAsyncKeyState('S') & 0x8000) {

		direction.z = 1.0;
		XMVECTOR directionVec = XMLoadFloat3(&direction);
		positionVec -= directionVec * time  * this->getMovementSpeed();
		XMStoreFloat3(&floatPos, positionVec);
		this->setPos(floatPos);
	}

	if (GetAsyncKeyState('A') & 0x8000) {

		direction.x = 1.0;
		XMVECTOR directionVec = XMLoadFloat3(&direction);
		positionVec -= directionVec * time * this->getMovementSpeed();
		XMStoreFloat3(&floatPos, positionVec);
		this->setPos(floatPos);
	}

	if (GetAsyncKeyState('D') & 0x8000) {

		direction.x = 1.0;
		XMVECTOR directionVec = XMLoadFloat3(&direction);
		positionVec += directionVec * time * this->getMovementSpeed();
		XMStoreFloat3(&floatPos, positionVec);
		this->setPos(floatPos);
	}
	if (oldpos.z != this->getPos().z)
	{
		cout << "Position Z: " << this->getPos().z << endl;
	}
	if (oldpos.x != this->getPos().x)
	{
		cout << "Position X: " << this->getPos().x << endl;
	}


	timer.updateCurrentTime();
}


//--------- Changing the character's position --------------


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

