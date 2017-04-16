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

	/*plane = getPlane();*/

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
	/*meleeAttack();
	rangeAttack();*/
}

//--------- Changing the character's position --------------
void MainCharacter::CharacterMove(HWND windowhandle)
{
	
	direction = { 0, 0, 0 };

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

	XMMATRIX transform = XMMatrixIdentity();
	XMFLOAT4X4 data;

	// Gather the rigid body matrix
	btTransform btRigidTransform;
	this->rigidBody->getMotionState()->getWorldTransform(btRigidTransform);

	// Load it into an XMFLOAT4x4
	btRigidTransform.getOpenGLMatrix((float*)&data);

	// Load it into an XMMATRIX
	transform = XMLoadFloat4x4(&data);
	XMVECTOR t;
	XMVECTOR s;
	XMVECTOR r;
	XMMatrixDecompose(&s, &r, &t, transform);
	XMFLOAT3 rigidPos;
	XMStoreFloat3(&rigidPos, t);

	this->setPos(rigidPos);

	newCameraPos = { floatPos.x, floatPos.y + cameraDistanceY, floatPos.z - cameraDistanceZ };
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
		this->rigidBody->applyForce(btVector3(0, 0, 1), btVector3(direction.x, direction.y, direction.z));
	}
	if (GetAsyncKeyState('S'))
	{
		direction.z = -1.0;
		this->rigidBody->applyForce(btVector3(0, 0, -1), btVector3(direction.x, direction.y, direction.z));
	}
	if (GetAsyncKeyState('A'))
	{
		direction.x = -1.0;
		this->rigidBody->applyForce(btVector3(-1, 0, 0), btVector3(direction.x, direction.y, direction.z));
	}
	if (GetAsyncKeyState('D'))
	{
		direction.x = 1.0;
		this->rigidBody->applyForce(btVector3(1, 0, 0), btVector3(direction.x, direction.y, direction.z));
	}

	return negativePosVec;
}

float MainCharacter::characterLookAt(HWND windowHandle)
{
	POINT p;
	float angle;
	GetCursorPos(&p);
	ScreenToClient(windowHandle, &p);

	float mouseX = p.x;
	float mouseY = p.y;

	float mouseXNDC = (2 * mouseX) / WIDTH - 1;
	float mouseYNDC = (2 * mouseY) / HEIGHT - 1;
	mouseYNDC *= -1;

	angle = atan2(mouseXNDC, mouseYNDC);

	return angle;
}

//Rotate character
XMMATRIX MainCharacter::rotate(HWND windowhandle)
{
	XMMATRIX R;

	float angle = characterLookAt(windowhandle);
	R = XMMatrixRotationY(angle);

	return R;
}
void MainCharacter::meleeAttack(HWND windowHandle)
{
	if (GetAsyncKeyState(MK_LBUTTON))
	{
		//Need an attack duration.
		//attackDuration = 0.5f;
		//if attackdur > 0 
		// arratckDur -= deltaTime;
		//When a time has passed and the attack is ongoing damage enemies within range.
		//(When attack != done or smth)
		//else... attack = done reset attack duration?

		cout << "MELEE ATTACK" << endl;
		//this is the way the character will be facing
		float angle = characterLookAt(windowHandle);
		
		//create boundingBox within the angle of the attack that
		//covers the range between the character and the attack range
		XMFLOAT3 characterPos = getPos();
		float centerX = 0.5 * cos(angle * (2 * PI)); //0.5 eftersom 1 boxRange
		float centerZ = 0.5 * sin(angle * (2 * PI));
		XMFLOAT3 boxCenter = {centerX, 0, centerZ };  
		XMFLOAT3 boxRange = { 1, 1, 1 };

		BoundingBox meleeBox = BoundingBox(boxCenter, boxRange);
		

		//We need a way to browse through the enemies
		//Array? Handler?

		//filter down the list of enemies, 
		//first if they're within range
		//then if they're within the angle of the attack

		//we need to get the number of enemies
		//ALT. enemyArray.length?
		//int nrOfEnemies = 10;

		//for (int i = 0; i < nrOfEnemies, i++;)
		//{
		//	float enemyDistance = 2;
		//	if (enemyDistance <= 1)
		//	{
		//		//check if that enemy boundingBox intersects meleeBox
		//			//if YES 
		//				//drain that enemy's health
		//			//else
		//				//do nothing and continue on
		//	}
		//}

		//attack done 
		//reset attack duration
		
	}
}
void MainCharacter::rangeAttack(HWND windowHandle)
{
	if (GetAsyncKeyState(MK_RBUTTON))
	{
		cout << "RANGED ATTACK" << endl;
	}

}



//Don't need this
//XMVECTOR MainCharacter::getPlane()
//{
//	XMVECTOR point = { 0, 0, 0 };
//	XMVECTOR normal = { 0, 1, 0 };
//
//	return XMPlaneFromPointNormal(point, normal);
//}
//XMFLOAT3 MainCharacter::getPointOnRay(XMFLOAT3 ray, float distance)
//{
//	
//	XMFLOAT3 scaledRay = { ray.x * distance, ray.y * distance, ray.z * distance };
//
//	return scaledRay;
//}
//bool MainCharacter::IntersectionInRange(XMFLOAT3 MousePos)
//{
//
//	//XMVECTOR clipcoords = {mouseXNDC, mouseYNDC, 1.0f, 0.0f};    
//	//
//	//XMVECTOR mouseViewCoords = XMVector4Transform(clipcoords, camera.projectionMatrix);
//
//
//	//XMFLOAT4 MVP;
//	//XMStoreFloat4(&MVP, mouseViewCoords);
//	//XMFLOAT4 filter = { MVP.x, MVP.y, 1.0f, 0.0f };
//	//XMVECTOR mouseWorldPos = XMLoadFloat4(&filter);
//	////cout << filter.x << " " << filter.y << " " << filter.z << endl;
//	//mouseWorldPos = XMVector4Transform(mouseWorldPos,camera.ViewInv);
//	//
//	//
//	//XMFLOAT4 holder; 
//	//XMStoreFloat4(&holder, mouseWorldPos);
//	//XMFLOAT3 MWP = { holder.x, holder.y, holder.z };
//	//MWP = math.Normalize(MWP);
//	////cout << MWP.x << " " << MWP.y << " " << MWP.z << endl;
//	//
//
//
//	//XMMATRIX world = XMMatrixIdentity();
//
//	//XMVECTOR ray = XMVector3Unproject(clipcoords, 0.0f, 0.0f, WIDTH, HEIGHT, 0.0f, 1.0f, camera.projectionMatrix, camera.viewMatrix, world);
//
//	//
//	//bool PlaneHit = IntersectionInRange(MWP);
//	//cout << PlaneHit << endl;
//
//	//if (IntersectionInRange(MWP))
//	//{
//	//}	
//
//	//bool hit = false;
//
//	//basicMath math;
//	//XMFLOAT3 normal = { 0,1,0 };
//	//XMFLOAT3 camPos = camera.GetPosition();
//	//XMFLOAT3 rayDirection;
//	//XMFLOAT3 point = { 0,0,0 };
//	//float T;
//
//	//XMVECTOR vecCamPos = XMLoadFloat3(&camPos);
//	//XMVECTOR vecMousePos = XMLoadFloat3(&MousePos);
//	//XMVECTOR vecRayDirection = vecMousePos - vecCamPos;
//	//
//	//XMStoreFloat3(&rayDirection, vecRayDirection);
//	//rayDirection = math.Normalize(rayDirection);
//
//
//	//// a point in the plane dotted with the plane normal
//	//float dp = math.dot(normal, point);
//	////dp *= -1;
//
//	//T = (dp - math.dot(normal,camPos)) / math.dot(normal,rayDirection);
//
//	//if (T > 0)
//	//{
//	//	hit = true;
//	//}
//
//	return false;
//}



