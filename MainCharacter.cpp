#include "MainCharacter.h"
#include "MacroDefinitions.h"

MainCharacter::MainCharacter()
	:CharacterBase(true, 10, 5.0f, 1, {2, 2, 5}, XMMatrixIdentity())
{
	cameraDistanceY = 6.0f;
	cameraDistanceZ = 5.0f;
	playerHeight = 2.0f;

	camera.SetPosition(this->getPos().x, cameraDistanceY, this->getPos().z - cameraDistanceZ);


	direction = { 0, 0, 0 };
	newCameraPos = { 0, 0, 0 };
	directionVec;
	floatPos = { 0, 0, 0 };
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

	
	XMVECTOR positionVec = XMLoadFloat3(&this->getPos());
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

	//if (oldpos.z != this->getPos().z)
	//{
	//	cout << "Position Z: " << this->getPos().z << endl;
	//}
	//if (oldpos.x != this->getPos().x)
	//{
	//	cout << "Position X: " << this->getPos().x << endl;
	//}

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
	GetCursorPos(&p);
	XMFLOAT3 right = camera.GetRight();
	XMFLOAT3 forward = camera.GetLook();
	XMFLOAT3 up = camera.GetUp();

	XMFLOAT3 currentRay;
	
	//float mouseX = camera.mLastMousePos.x;
	//float mouseY = camera.mLastMousePos.y;

	float mouseX = p.x;
	float mouseY = p.y;

	float mouseXNDC = (2 * mouseX) / WIDTH - 1;
	float mouseYNDC = (2 * mouseY) / HEIGHT - 1;
	mouseYNDC = (mouseYNDC*-1);

	XMVECTOR clipcoords = {mouseXNDC, mouseYNDC, 1.0f, 1.0f};
	
	

	XMVECTOR mouseWorldPos = XMVector4Transform(clipcoords, camera.InvViewPoj);
	
	XMFLOAT4 MWP; 
	XMStoreFloat4(&MWP, mouseWorldPos);
	float MWPLength = sqrt(MWP.x*MWP.x + MWP.y*MWP.y + MWP.z*MWP.z);
	XMFLOAT4 normalizedMWP;
	normalizedMWP.x = MWP.x / MWPLength; 
	normalizedMWP.y = MWP.y / MWPLength;
	normalizedMWP.z = MWP.z / MWPLength;
	cout << normalizedMWP.x << " " << normalizedMWP.y << " " << normalizedMWP.z << endl;
	

	if (MK_LBUTTON) {

		//Where the mouse is, is where to character will face. How to solve?
		//We have z(forward), x (right) and y (which is up). 
		//The character will rotate around y.

		float dx = XMConvertToRadians(0.25f * static_cast<float>(p.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(p.y - camera.mLastMousePos.y));

		R = XMMatrixRotationY(dx);

		//-------What does this do?------------
		/*XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), R));
		XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
		XMStoreFloat3(&forward, XMVector3TransformNormal(XMLoadFloat3(&forward), R));*/
	}

	//The mouse position is the new Point?
	camera.mLastMousePos.x = p.x;
	camera.mLastMousePos.y = p.y;
	//cout << camera.mLastMousePos.x << " " << camera.mLastMousePos.y << endl;

	return R;
}

