#include "MainCharacter.h"
#include "MacroDefinitions.h"

MainCharacter::MainCharacter()
	:CharacterBase(true, 10, 5.0f, 1, {2, 2, 5}, XMMatrixIdentity())
{
	cameraDistanceY = 6.0f;
	cameraDistanceZ = 3.0f;
	playerHeight = 2.0f;

	camera.SetPosition(this->getPos().x, cameraDistanceY, this->getPos().z - cameraDistanceZ);

	plane = getPlane();

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

	direction = { 0, 0, 0 };

	float time = timer.getDeltaTime();

	
	XMVECTOR positionVec = XMLoadFloat3(&this->getPos());
	XMFLOAT3 oldpos = this->getPos();

	XMMATRIX R = rotate();
	updateWorldMatrix(oldpos, R);

	//Here
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

//#if defined (DEBUG) || defined(_DEBUG)
//
//	if (oldpos.z != this->getPos().z)
//	{
//		cout << "Position Z: " << this->getPos().z << endl;
//	}
//	if (oldpos.x != this->getPos().x)
//	{
//		cout << "Position X: " << this->getPos().x << endl;
//	}
//
//#endif

	timer.updateCurrentTime();
}

bool MainCharacter::CheckInput(XMFLOAT3 &direction) {

	bool negativePosVec = false;

	direction.x = 0;
	direction.z = 0;

	if (GetAsyncKeyState('W'))
	{
		direction.z = 1.0;
	}
	if (GetAsyncKeyState('S'))
	{
		direction.z = -1.0;
		//negativePosVec = true;
	}
	if (GetAsyncKeyState('A'))
	{
		direction.x = -1.0;
		//negativePosVec = true;
	}
	if (GetAsyncKeyState('D'))
	{
		direction.x = 1.0;
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

	//XMFLOAT3 currentRay;
	
	//float mouseX = camera.mLastMousePos.x;
	//float mouseY = camera.mLastMousePos.y;

	float mouseX = p.x;
	float mouseY = p.y;

	float mouseXNDC = (2 * mouseX) / WIDTH - 1;
	float mouseYNDC = (2 * mouseY) / HEIGHT - 1;
	mouseYNDC = (mouseYNDC*-1);

	XMVECTOR clipcoords = {mouseXNDC, mouseYNDC, 1.0f, 1.0f};
	
	XMVECTOR mouseViewCoords = XMVector4Transform(clipcoords, camera.ProjInv);
	XMFLOAT4 MVP;

	XMStoreFloat4(&MVP, mouseViewCoords);
	XMFLOAT4 filter = { MVP.x, MVP.y, 1.f, 0 };
	XMVECTOR mouseWorldPos = XMLoadFloat4(&filter);

	mouseWorldPos = XMVector4Transform(mouseWorldPos,camera.ViewInv);
	
	
	XMFLOAT4 holder; 
	XMStoreFloat4(&holder, mouseWorldPos);
	XMFLOAT3 MWP = { holder.x, holder.y, holder.z };
	float MWPLength = sqrt(MWP.x*MWP.x + MWP.y*MWP.y + MWP.z*MWP.z);
	XMFLOAT4 normalizedMWP;
	normalizedMWP.x = MWP.x / MWPLength; 
	normalizedMWP.y = MWP.y / MWPLength;
	normalizedMWP.z = MWP.z / MWPLength;
	cout << normalizedMWP.x << " " << normalizedMWP.y << " " << normalizedMWP.z << endl;
	

	if (MK_LBUTTON) {

		float dx = XMConvertToRadians(0.25f * static_cast<float>(p.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(p.y));
		
		angle = atan2(dx, dy) * (2 * PI);
		R = XMMatrixRotationY(angle);
	}

	camera.mLastMousePos.x = p.x;
	camera.mLastMousePos.y = p.y;
	//cout << camera.mLastMousePos.x << " " << camera.mLastMousePos.y << endl;

	return R;
}

XMVECTOR MainCharacter::getPlane()
{
	XMVECTOR point = { 0, 0, 0 };
	XMVECTOR normal = { 0, 1, 0 };

	return XMPlaneFromPointNormal(point, normal);
}

