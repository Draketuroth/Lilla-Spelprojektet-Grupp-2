
#include "Platform.h"

Platform::Platform() {

	lerpScalar = 0;
}

Platform::~Platform() {


}

void Platform::platformDecension()
{

	btTransform rigidCube;
	XMFLOAT4X4 data;
	XMMATRIX transform;
	XMFLOAT3 pos;

	XMVECTOR t;
	XMVECTOR s;
	XMVECTOR r;

	rigidBody->getMotionState()->getWorldTransform(rigidCube);
	rigidCube.getOpenGLMatrix((float*)&data);

	transform = XMLoadFloat4x4(&data);
	XMMatrixDecompose(&s, &r, &t, transform);

	XMStoreFloat3(&pos, t);

	btScalar rigidXvalue = pos.x;
	btScalar rigidYvalue = pos.y;
	btScalar rigidZvalue = pos.z;

	btVector3 startPos(startPos.x(), startPos.y(), startPos.z());
	btVector3 endPos = { startPos.x(),-11 ,startPos.z() };

	if (pos.y > -11)
	{
		btVector3 lerpResult = lerp(startPos, endPos, lerpScalar);
		lerpScalar += 0.001f;
		btMatrix3x3 movementMatrix;
		movementMatrix.setIdentity();
		btTransform plat{ movementMatrix,lerpResult };


		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		rigidBody->setActivationState(DISABLE_DEACTIVATION);

		//cube.rigidBody->translate(btVector3(0, 1, 0));
		rigidBody->getMotionState()->setWorldTransform(plat);
	}
	else
	{
		lerpScalar = 0;
	}


}

void Platform::platformAcension()
{

	btTransform rigidCube;
	XMFLOAT4X4 data;
	XMMATRIX transform;
	XMFLOAT3 pos;

	XMVECTOR t;
	XMVECTOR s;
	XMVECTOR r;

	rigidBody->getMotionState()->getWorldTransform(rigidCube);
	rigidCube.getOpenGLMatrix((float*)&data);

	transform = XMLoadFloat4x4(&data);
	XMMatrixDecompose(&s, &r, &t, transform);

	XMStoreFloat3(&pos, t);

	btScalar rigidXvalue = pos.x;
	btScalar rigidYvalue = pos.y;
	btScalar rigidZvalue = pos.z;

	btVector3 startPos(startPos.x(), -11, startPos.z());
	btVector3 endPos = { this->startPos.x(),this->startPos.y(), this->startPos.z() };

	if (pos.y < this->startPos.y())
	{
		btVector3 lerpResult = lerp(startPos, endPos, lerpScalar);
		lerpScalar += 0.002f;
		btMatrix3x3 movementMatrix;
		movementMatrix.setIdentity();
		btTransform plat{ movementMatrix,lerpResult };


		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		rigidBody->setActivationState(DISABLE_DEACTIVATION);


		rigidBody->getMotionState()->setWorldTransform(plat);
	}
	else
	{
		lerpScalar = 0;
	}
}

void Platform::platformBreaking()
{
	btTransform rigidCube;
	XMFLOAT4X4 data;
	XMMATRIX transform;
	XMFLOAT3 pos;

	XMVECTOR t;
	XMVECTOR s;
	XMVECTOR r;

	rigidBody->getMotionState()->getWorldTransform(rigidCube);
	rigidCube.getOpenGLMatrix((float*)&data);

	transform = XMLoadFloat4x4(&data);
	XMMatrixDecompose(&s, &r, &t, transform);

	XMStoreFloat3(&pos, t);

	btScalar rigidXvalue = pos.x;
	btScalar rigidYvalue = pos.y;
	btScalar rigidZvalue = pos.z;

	
	btVector3 endPos;

	if (pos.x > startPos.x() + 0.05f)
	{

		endPos = { pos.x - 0.1f,pos.y,pos.z };
	}
	else
	{

		endPos = { pos.x + 0.1f,pos.y,pos.z };
	}



	btVector3 lerpResult = lerp(startPos, endPos, 1);

	btMatrix3x3 movementMatrix;
	movementMatrix.setIdentity();
	btTransform plat{ movementMatrix,lerpResult };


	rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	rigidBody->setActivationState(DISABLE_DEACTIVATION);

	//cube.rigidBody->translate(btVector3(0, 1, 0));
	rigidBody->getMotionState()->setWorldTransform(plat);


}