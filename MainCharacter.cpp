#include "MainCharacter.h"

MainCharacter::MainCharacter()
	:CharacterBase(true, 10, 5.0f, 1, {2, 2, 5}, XMMatrixIdentity())
{
	cameraDistanceY = 6.0f;
	cameraDistanceZ = 3.0f;
	playerHeight = 2.0f;

	camera.SetPosition(this->getPos().x, cameraDistanceY, this->getPos().z - cameraDistanceZ);

	plane = getPlane();

}

MainCharacter::~MainCharacter()
{
}

void MainCharacter::update()
{
	CharacterMove();
}

//--------- Changing the character's position --------------
void MainCharacter::CharacterMove()
{

	float time = timer.getDeltaTime();

	XMFLOAT3 direction = { 0, 0, 0 };
	XMFLOAT3 newCameraPos = { 0, 0, 0 };
	XMVECTOR positionVec = XMLoadFloat3(&this->getPos());
	XMVECTOR directionVec;
	XMFLOAT3 floatPos = { 0, 0, 0 };
	XMFLOAT3 oldpos = this->getPos();

	XMMATRIX R = rotate();
	updateWorldMatrix(oldpos, R);

	if(CheckInput(direction) == false){
	
		directionVec = XMLoadFloat3(&direction);
		positionVec += directionVec * time * this->getMovementSpeed();

	}

	else {
		
		directionVec = XMLoadFloat3(&direction);
		positionVec -= directionVec * time * this->getMovementSpeed();
	}

	XMStoreFloat3(&floatPos, positionVec);
	this->setPos(floatPos);

	newCameraPos = { floatPos.x, cameraDistanceY, floatPos.z - cameraDistanceZ };
	camera.SetPosition(newCameraPos);

#if defined (DEBUG) || defined(_DEBUG)

	if (oldpos.z != this->getPos().z)
	{
		cout << "Position Z: " << this->getPos().z << endl;
	}
	if (oldpos.x != this->getPos().x)
	{
		cout << "Position X: " << this->getPos().x << endl;
	}

#endif

	timer.updateCurrentTime();
}

bool MainCharacter::CheckInput(XMFLOAT3 &direction) {

	bool negativePosVec = false;

	if ((GetAsyncKeyState('W') & 0x8000) && (GetAsyncKeyState('D') & 0x8000)) {

		// negativePosVec == false
		direction.z = 1.0;
		direction.x = 1.0;
	}

	if ((GetAsyncKeyState('W') & 0x8000) && (GetAsyncKeyState('A') & 0x8000)) {

		// negativePosVec == false
		direction.z = 1.0;
		direction.x = -1.0;
	}

	if ((GetAsyncKeyState('S') & 0x8000) && (GetAsyncKeyState('D') & 0x8000)) {

		// negativePosVec == true
		direction.z = 1.0;
		direction.x = -1.0;

		negativePosVec = true;
	}

	if ((GetAsyncKeyState('S') & 0x8000) && (GetAsyncKeyState('A') & 0x8000)) {

		// negativePosVec == true
		direction.z = 1.0;
		direction.x = 1.0;

		negativePosVec = true;
	}

	if ((GetAsyncKeyState('D') & 0x8000) && (!GetAsyncKeyState('W')) && (!GetAsyncKeyState('S'))) {

		// negativePosVec == false
		direction.x = 1.0;
	}

	if ((GetAsyncKeyState('A') & 0x8000) && (!GetAsyncKeyState('W')) && (!GetAsyncKeyState('S'))) {

		// negativePosVec == true
		direction.x = 1.0;

		negativePosVec = true;
	}

	if ((GetAsyncKeyState('W') & 0x8000) && (!GetAsyncKeyState('A')) && (!GetAsyncKeyState('D')) && (!GetAsyncKeyState('S'))) {

		// negativePosVec == false
		direction.z = 1.0;
	}

	if ((GetAsyncKeyState('S') & 0x8000) && (!GetAsyncKeyState('A')) && (!GetAsyncKeyState('D')) && (!GetAsyncKeyState('D'))) {

		// negativePosVec == true
		direction.z = 1.0;

		negativePosVec = true;
	}

	return negativePosVec;
}

//Rotate character
XMMATRIX MainCharacter::rotate()
{
	XMMATRIX R;
	POINT p;

	XMFLOAT3 characterPosition = getPos();
	float angle;
	GetCursorPos(&p);

	if (MK_LBUTTON) {

		float dx = XMConvertToRadians(0.25f * static_cast<float>(p.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(p.y));
		
		angle = atan2(dx, dy) * (2 * PI);
		R = XMMatrixRotationY(angle);
	}

	camera.mLastMousePos.x = p.x;
	camera.mLastMousePos.y = p.y;

	return R;
}

XMVECTOR MainCharacter::getPlane()
{
	XMVECTOR point = { 0, 0, 0 };
	XMVECTOR normal = { 0, 1, 0 };

	return XMPlaneFromPointNormal(point, normal);
}

