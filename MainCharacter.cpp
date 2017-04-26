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

	this->attacking = false;
	this->attackTimer = 0.0f;
	this->attackCd = 0.5f;

	this->shooting = false;
	this->shootCD = 0.8f;
	this->shootTimer = 0.0f;

	camera.SetPosition(this->getPos().x, cameraDistanceY, this->getPos().z - cameraDistanceZ);

	this->test = 0;
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
	CreateBoundingBox(0.10, this->getPos(), XMFLOAT3(0.6, 0.8f, 0.6), bulletPhysicsHandle);
	this->rigidBody->setIslandTag(characterRigid);//This is for checking intersection ONLY between the projectile of the player and any possible enemy, not with platforms or other rigid bodies
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
	if (GetAsyncKeyState(MK_LBUTTON) && ! attacking && attackTimer <= 0)
	{
		attacking = true;
		attackTimer = attackCd;
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
				test++;
				cout << "HIT!" << test << endl;
				enemyArray[0].setHealth(enemyArray[0].getHealth() - 1);


				if (enemyArray[0].getHealth() <= 0)
				{
					enemyArray[0].setAlive(false);
					cout << "ENEMY DEAD" << endl;
				}
			}
		}
	}

	if (attacking)
	{
		if (attackTimer > 0)
			attackTimer -= timer.getDeltaTime();
		else
		{
			attacking = false;
		}
	}

}

void MainCharacter::rangeAttack(HWND windowHandle, int nrOfEnemies, Enemy enemies[], btDynamicsWorld* world)
{
	this->rigidBody->setIslandTag(characterRigid);
	for (size_t i = 0; i < nrOfEnemies; i++)
	{
		enemies[i].rigidBody->setIslandTag(characterRigid);
	}

	if (GetAsyncKeyState(MK_RBUTTON) && !this->shooting && this->shootTimer <= 0)
	{
		
		float angle = this->characterLookAt(windowHandle);
		cout << "RANGED ATTACK" << endl;
		this->shooting = true;
		this->shootTimer = this->shootCD;

		cout << "Player Tag: " << this->rigidBody->getIslandTag() << endl << endl;

		XMFLOAT3 characterPos = this->getPos();
		XMVECTOR RayOrigin = XMLoadFloat3(&characterPos);


		//Used to rotate the direction vector, so that we always shoot in the direction of where the player is facing
		XMVECTOR directionVec = this->getForwardVector();

		XMVECTOR rotQuat = XMQuaternionRotationAxis(XMVECTOR{ 0, 1, 0 }, angle);
		directionVec = XMVector3Rotate(directionVec, rotQuat);
		directionVec = XMVector3Normalize(directionVec);

		//The offset starting point for the ray
		XMFLOAT3 newOrigin;
		RayOrigin += directionVec * 1.4f;
		XMStoreFloat3(&newOrigin, RayOrigin);
		
		XMFLOAT3 direction;
		XMStoreFloat3(&direction, directionVec);
	

		//cout << "Direction X: " << direction.x << "Direction Y: " << direction.y << "Direction Z: " << direction.z << endl;
		
		btCollisionWorld::ClosestRayResultCallback rayCallBack(btVector3(newOrigin.x, 1.2f, newOrigin.z), btVector3(direction.x * 50, 1.2f, direction.z * 50));
		world->rayTest(btVector3(newOrigin.x, 1.2f, newOrigin.z), btVector3(direction.x * 50, 1.5f, direction.z * 50), rayCallBack);
		if (rayCallBack.hasHit())
		{
			
			btVector3 pos = rayCallBack.m_collisionObject->getWorldTransform().getOrigin();
			XMFLOAT3 floatPos;
			cout << "RayHit Tag: " << rayCallBack.m_collisionObject->getIslandTag() << endl;
			cout << "Hit pos X: " << rayCallBack.m_collisionObject->getWorldTransform().getOrigin().getX() << "  Hit Pos Y: " << rayCallBack.m_collisionObject->getWorldTransform().getOrigin().getY() << endl << endl;
				
		
			for (size_t i = 0; i < nrOfEnemies; i++)
			{
				cout << "Enemy Tag: " << enemies[i].rigidBody->getIslandTag() << endl << endl;
				if (enemies[i].rigidBody->getIslandTag() == rayCallBack.m_collisionObject->getIslandTag())
				{
					cout << "Enemy Shot!! -1 health\n";
					enemies[i].setHealth(enemies[i].getHealth() - 1);
				}
				if (enemies[i].getHealth() == 0)
				{
					enemies[i].setAlive(false);
					cout << "Enemy Deleted\n";
				}
			}
		}
	}

	if (this->shooting)
	{
		if (this->shootTimer > 0)
		{
			this->shootTimer -= timer.getDeltaTime();
		}
		else
		{
			this->shooting = false;
		}
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



