#include "MainCharacter.h"
#include "MacroDefinitions.h"
#include"Window.h"

MainCharacter::MainCharacter()
	:CharacterBase(true, 10, 5.0f, 1, {2, 20, 5}, XMMatrixIdentity())
{
	cameraDistanceY = 6.0f;
	cameraDistanceZ = 3.0f;
	playerHeight = 2.0f;
	currentAnimIndex = 0;

	camera.SetPosition(this->getPos().x, cameraDistanceY, this->getPos().z - cameraDistanceZ);

}

MainCharacter::~MainCharacter()
{

}

void MainCharacter::initialize(ID3D11Device* &graphicDevice, XMFLOAT3 spawnPosition, BulletComponents &bulletPhysicsHandle, FbxImport &fbxImporter) {

	currentAnimIndex = 0;

	// Main character function
	loadVertices(fbxImporter, graphicDevice);

	// Base character functions
	createBuffers(graphicDevice, fbxVector, fbxImporter, skinData);
	CreateBoundingBox(0.10, this->getPos(), XMFLOAT3(1, 1, 1), bulletPhysicsHandle);
}

void MainCharacter::update(HWND windowhandle)
{
	CharacterMove(windowhandle);
}

//--------- Changing the character's position --------------
void MainCharacter::CharacterMove(HWND windowhandle)
{
	
	direction = { 0, 0, 0 };

	float time = timer.getDeltaTime();

	
	XMVECTOR positionVec = XMLoadFloat3(&this->getPos());
	XMFLOAT3 oldpos = this->getPos();

	XMMATRIX R = rotate(windowhandle);
	XMMATRIX scale = XMMatrixScaling(0.2, 0.2, 0.2);
	updateWorldMatrix(R, scale);
	
	CheckInput();

	XMMATRIX transform;
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

	newCameraPos = { rigidPos.x, rigidPos.y + cameraDistanceY, rigidPos.z - cameraDistanceZ };
	camera.SetPosition(newCameraPos);

	timer.updateCurrentTime();
}

void MainCharacter::CheckInput() {

	bool isIdle = true;
	this->rigidBody->setFriction(4);

	if (GetAsyncKeyState('W'))
	{
		currentAnimIndex = 0;
		isIdle = false;

		this->rigidBody->setFriction(3);
		this->rigidBody->applyCentralForce(btVector3(0, 0, 7));
	}
	if (GetAsyncKeyState('S'))
	{
		currentAnimIndex = 0;
		isIdle = false;

		this->rigidBody->setFriction(3);
		this->rigidBody->applyCentralForce(btVector3(0, 0, -7));	
	}
	if (GetAsyncKeyState('A'))
	{
		currentAnimIndex = 0;
		isIdle = false;

		this->rigidBody->setFriction(3);
		this->rigidBody->applyCentralForce(btVector3(-7, 0, 0));
	}
	if (GetAsyncKeyState('D'))
	{
		currentAnimIndex = 0;
		isIdle = false;

		this->rigidBody->setFriction(3);
		this->rigidBody->applyCentralForce(btVector3(7, 0, 0));	
	}

	float maxSpeed = 4;
	float minSpeed = -4;
	btVector3 speed = this->rigidBody->getLinearVelocity();
	
	//X-led
	if (speed.getX() > maxSpeed)
	{
		speed.setX(maxSpeed);
	}
	if (speed.getX() < minSpeed)
	{
		speed.setX(minSpeed);
	}
	//Z-led
	if (speed.getZ() > maxSpeed)
	{
		speed.setZ(maxSpeed);
	}
	if (speed.getZ() < minSpeed)
	{
		speed.setZ(minSpeed);
	}

	if (isIdle == true){

		currentAnimIndex = 1;

	}

	this->rigidBody->setLinearVelocity(speed);
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

	//returns in radians?
	return angle;
}

void MainCharacter::loadVertices(FbxImport &fbxImporter, ID3D11Device* &graphicDevice) {

	HRESULT hr;

	fbxImporter.LoadFBX(&fbxVector); //load mesh vertices

	for (unsigned int i = 0; i < fbxImporter.meshSkeleton.hierarchy.size(); i++) {

		XMMATRIX inversedBindPose = fbxImporter.Load4X4JointTransformations(fbxImporter.meshSkeleton.hierarchy[i], i); // converts from float4x4 too xmmatrix

		skinData.gBoneTransform[i] = inversedBindPose;
		fbxImporter.invertedBindPose[i] = inversedBindPose; // copy on the cpu

	}
}

//Rotate character
XMMATRIX MainCharacter::rotate(HWND windowhandle)
{
	XMMATRIX R;
	float angle = characterLookAt(windowhandle);

	R = XMMatrixRotationY(angle);

	return R;
}




void MainCharacter::meleeAttack(HWND windowHandle, int nrOfEnemies, Enemy enemyArray[], btDynamicsWorld* bulletDynamicsWorld)
{
	float attackDuration;

	if (GetAsyncKeyState(MK_LBUTTON))
	{
		attackDuration = 0.5f;

		while (attackDuration > 0)
		{
			attackDuration -= timer.getDeltaTime();
		}

		//-----------------Calculate the hit area-----------------------
		float angle = characterLookAt(windowHandle);

		XMFLOAT3 characterPos = this->getBoundingBox().Center;
		XMMATRIX playerTranslation = getPlayerTanslationMatrix();

		float centerX = characterPos.x + sin(angle);
		float centerZ = characterPos.z + cos(angle);

		XMFLOAT3 boxCenter = { centerX, 0, centerZ };
		XMFLOAT3 boxRange = { 2, 2, 2 };

		BoundingBox meleeBox = BoundingBox(boxCenter, boxRange);
		meleeBox.Transform(meleeBox, playerTranslation);
		//---------------------------------------------------------------
		//---------Attack------------------------------------------------

		for (int i = 0; i < nrOfEnemies; i++)
		{
			BoundingBox enemyBox = enemyArray[0].getBoundingBox();
			if (enemyBox.Intersects(meleeBox))
			{
				cout << "HIT!" << endl;
				enemyArray[0].setHealth(enemyArray[0].getHealth() - 1);

				XMFLOAT3 playerForward;
				XMStoreFloat3(&playerForward, forwardVector);
				enemyArray[0].rigidBody->applyCentralForce(btVector3(playerForward.x, playerForward.y, playerForward.z));

				if (enemyArray[0].getHealth() <= 0 && enemyArray[0].getAlive() == true)
				{
					enemyArray[0].setAlive(false);
					cout << "ENEMY DOWN" << endl;

					enemyArray[0].releaseAll(bulletDynamicsWorld);

				}
			}
			/*}*/

		}
	}

}

void MainCharacter::rangeAttack(HWND windowHandle)
{
	if (GetAsyncKeyState(MK_RBUTTON))
	{
		cout << "RANGED ATTACK" << endl;
	}

	//check which way the charater is looking
	float angle = characterLookAt(windowHandle);

	//fireProjectile(angle);

	//fireProjectile have to create and follow up the projectile and see it it hits anything.
	//it needs to check every frame until the projectile is a certain distance away from the character
	//or until it hits an enemy

	//The projectile should move straight forward into its direction at a constant speed (no real need for physics)
	//The projectile needs to be able to sort through the list of enemies.
}

//void MainCharacter::initiateBB(float mass,BulletComponents& bulletPhysicsHandle)
//{
//
//}

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



