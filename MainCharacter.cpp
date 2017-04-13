#include "MainCharacter.h"
#include "MacroDefinitions.h"
#include"Window.h"

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

void MainCharacter::update(HWND windowhandle)
{
	CharacterMove(windowhandle);
}

//--------- Changing the character's position --------------
void MainCharacter::CharacterMove(HWND windowhandle)
{

	float time = timer.getDeltaTime();

	
	XMVECTOR positionVec = XMLoadFloat3(&this->getPos());
	XMFLOAT3 oldpos = this->getPos();

	XMMATRIX R = rotate(windowhandle);
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
XMMATRIX MainCharacter::rotate(HWND windowhandle)
{
	basicMath math;
	XMMATRIX R;
	POINT p;
	const float RayRange = 600;

	XMFLOAT3 characterPosition = getPos();
	float angle;
	GetCursorPos(&p);
	ScreenToClient(windowhandle, &p);

	float mouseX = p.x;
	float mouseY = p.y;

	float mouseXNDC = (2 * mouseX) / WIDTH - 1;
	float mouseYNDC = (2 * mouseY) / HEIGHT - 1;
	mouseYNDC *= -1;


	//https://www.braynzarsoft.net/viewtutorial/q16390-24-picking
	float viewSpaceZ = 1 / tan((0.45*PI) / 2);

	//cout << mouseXNDC << " " << mouseYNDC << endl;

	XMVECTOR clipcoords = {mouseXNDC, mouseYNDC, viewSpaceZ};    
	
	XMVECTOR mouseViewCoords = XMVector4Transform(clipcoords, camera.ProjInv);


	XMFLOAT4 MVP;
	XMStoreFloat4(&MVP, mouseViewCoords);
	XMFLOAT4 filter = { MVP.x, MVP.y, 1.0f, 0.0f};
	XMVECTOR mouseWorldPos = XMLoadFloat4(&filter);
	//cout << filter.x << " " << filter.y << " " << filter.z << endl;
	//mouseWorldPos = XMVector4Transform(mouseWorldPos,camera.ViewInv);
	
	XMFLOAT3 MWPosFloat3;
	XMVECTOR MWPos = XMVector4Transform(mouseWorldPos, camera.ViewInv);
	XMStoreFloat4(&filter, MWPos);
	MWPosFloat3.x = filter.x;
	MWPosFloat3.y = filter.y;
	MWPosFloat3.z = filter.z;
	math.Normalize(MWPosFloat3);
	cout << MWPosFloat3.x << " " << MWPosFloat3.y << " " << MWPosFloat3.z << endl;

	XMFLOAT4 holder; 
	XMStoreFloat4(&holder, mouseWorldPos);
	XMFLOAT3 MWP = { holder.x, holder.y, holder.z };
	MWP = math.Normalize(MWP);
	//cout << MWP.x << " " << MWP.y << " " << MWP.z << endl;
	

	if (MK_LBUTTON) {

		float dx = XMConvertToRadians(0.25f * static_cast<float>(p.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(p.y));
		
		angle = atan2(dx, dy) * (2 * PI);
		R = XMMatrixRotationY(angle);
	}


	
	bool PlaneHit = IntersectionInRange(MWP);
	//cout << PlaneHit << endl;

	

	return R;
}

XMVECTOR MainCharacter::getPlane()
{
	XMVECTOR point = { 0, 0, 0 };
	XMVECTOR normal = { 0, 1, 0 };

	return XMPlaneFromPointNormal(point, normal);
}
XMFLOAT3 MainCharacter::getPointOnRay(XMFLOAT3 ray, float distance)
{
	
	XMFLOAT3 scaledRay = { ray.x * distance, ray.y * distance, ray.z * distance };

	return scaledRay;
}
bool MainCharacter::IntersectionInRange(XMFLOAT3 MousePos)
{
	basicMath math;
	XMFLOAT3 normal = { 0,1,0 };
	XMFLOAT3 camPos = camera.GetPosition();
	XMFLOAT3 rayDirection;
	XMFLOAT3 point = { 0,0,0 };
	float T;

	XMVECTOR vecCamPos = XMLoadFloat3(&camPos);
	XMVECTOR vecMousePos = XMLoadFloat3(&MousePos);
	XMVECTOR vecRayDirection = vecMousePos - vecCamPos;
	
	XMStoreFloat3(&rayDirection, vecRayDirection);
	rayDirection = math.Normalize(rayDirection);

	// a point in the plane dotted with the plane normal
	float dp = math.dot(normal, point);
	//dp *= -1;

	T = (dp - math.dot(normal,camPos)) / math.dot(normal,rayDirection);

	if (T < 0)
	{
		return false;
	}
	else
	{
		return true;
	}

	
	
	

	return false;
}


