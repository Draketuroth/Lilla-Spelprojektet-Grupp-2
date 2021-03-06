
#include "Camera.h"

// Constructor and destructor
Camera::Camera() {
	
	XMMATRIX worldMatrix = XMMatrixIdentity();
	//----------------------------------------------------------------------------------------------------------------------------------//

	// The view matrix is used to to transform an object's vertices from world space to view space, written in Row major.
	// Using the following method, the matrix can be computed from the world position of the camera (eye), a global up vector, and a 
	// target point.

	eyePosF = { 0, 6, 0 };
	lookAtF = { 0, 0, 1 };
	upF = { 0, 1, 0 };

	eyePos = DirectX::XMLoadFloat3(&eyePosF);
	lookAt = DirectX::XMLoadFloat3(&lookAtF);
	up = DirectX::XMLoadFloat3(&upF);

	viewMatrix = XMMatrixLookAtLH(eyePos, lookAt, up);
	LookAt(eyePos, lookAt, up);

	
	//----------------------------------------------------------------------------------------------------------------------------------//

	// The projection matrix is the actual camera we are looking through when viewing the world with its far and near clipping plane.
	// It's important to create this since we need it to link between view space and clip space

	float fov = PI * 0.45f;		// We recieve the field of view in radians by multiplying with PI

	float aspectRatio = float(WIDTH) / HEIGHT;		// Using the already defined macros for the width and height of the viewport

	float nearPlane = NEARPLANE;

	float farPlane = FARPLANE;

	projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
	SetLens(fov, aspectRatio, nearPlane, farPlane);

	viewProj = viewMatrix * projectionMatrix;
	projDet = XMMatrixDeterminant(projectionMatrix);

	ProjInv = XMMatrixInverse(nullptr, projectionMatrix);
	viewDet = XMMatrixDeterminant(viewMatrix);
	ViewInv = XMMatrixInverse(nullptr, viewMatrix);
	detViewProj = XMMatrixDeterminant(viewProj);
	InvViewPoj = XMMatrixInverse(nullptr, viewProj);
	
	Pitch(45);
}

Camera::~Camera() {

	
}

void Camera::cameraUpdate(float delta)
{

	//--------MOVEMENT MOUSE--------------
	POINT p;
	GetCursorPos(&p);

	if (MK_LBUTTON) {

		// //Make each pixel to correspond to a quarter of a degree

		//float dx = XMConvertToRadians(0.25f * static_cast<float>(p.x - mLastMousePos.x));

		//float dy = XMConvertToRadians(0.25f * static_cast<float>(p.y - mLastMousePos.y));

		//Pitch(dy);
		//RotateY(dx);
	}

	mLastMousePos.x = p.x;
	mLastMousePos.y = p.y;

	//_--------MOVEMENT KEYS-----------------------

	float moveSpeed = 5.0f;
	moveEvent(delta, moveSpeed);
}

void Camera::moveEvent(float deltaTime, float speed)
{
	if (GetAsyncKeyState(VK_UP) & 0x8000) {

		Walk(speed * deltaTime);
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {

		Walk(-speed * deltaTime);
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {

		Strafe(-speed * deltaTime);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {

		Strafe(speed * deltaTime);
	}
}


//---------FLYTTA DETTA--------------------
bool Camera::Collision()
{
	bool collision = false; 
	float CAMx; 
	float CAMz;

	CAMx = GetX(); 
	CAMz = GetZ(); 

	if (CAMx >= -31.5 && CAMx <= 31.5)
	{
		if (CAMz >= -31.5&& CAMz <= 31.5)
		{
			collision = true; 
		}
	}
	
	return collision; 
}
//------------------------------------------



// Get/Set Camera Properties
XMVECTOR Camera::GetPositionXM()const {

	return XMLoadFloat3(&mPosition);
}

XMFLOAT3 Camera::GetPosition()const {

	return mPosition;
}

float Camera::GetX()const
{
	float HX = mPosition.x; 
	return HX; 
}

float Camera::GetZ()const
{
	float HZ = mPosition.z;
	return HZ;
}

void Camera::SetPosition(float x, float y, float z) {

	mPosition = XMFLOAT3(x, y, z);
}

void Camera::SetPosition(const XMFLOAT3& v) {

	mPosition = v;
}

// Get Camera Vectors
XMVECTOR Camera::GetRightXM()const {

	return XMLoadFloat3(&mRight);
}

XMFLOAT3 Camera::GetRight()const {

	return mRight;
}

XMVECTOR Camera::GetUpXM()const {

	return XMLoadFloat3(&mUp);
}

XMFLOAT3 Camera::GetUp()const {

	return mUp;
}

XMVECTOR Camera::GetLookXM()const {

	return XMLoadFloat3(&mLook);
}

XMFLOAT3 Camera::GetLook()const {

	return mLook;
}

// Get Frustum Properties
float Camera::GetNearZ()const {

	return mNearZ;
}

float Camera::GetFarZ()const {

	return mFarZ;
}

float Camera::GetAspect()const {

	return mAspect;
}

float Camera::GetFovY()const {

	return mFovY;
}

float Camera::GetFovX()const {

	float halfWidth = 0.5f * GetNearWindowWidth();

	return 2.0f*atan(halfWidth / mNearZ);
}

// Get Near and Far Plane dimensions in View Space coordinates
float Camera::GetNearWindowWidth()const {

	return mAspect * mNearWindowHeight;
}

float Camera::GetNearWindowHeight()const {

	return mNearWindowHeight;
}

float Camera::GetFarWindowWidth()const {

	return mAspect * mFarWindowHeight;
}

float Camera::GetFarWindowHeight()const {

	return mFarWindowHeight;
}

// Set frustum
void Camera::SetLens(float fovY, float aspect, float zn, float zf) {

	// Cache properties
	mFovY = fovY;
	mAspect = aspect;

	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f*mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f*mFovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
}

// Define Camera Space via Look At parameters
void Camera::LookAt(XMVECTOR pos, XMVECTOR target, XMVECTOR worldUp) {

	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mLook, target);
	XMStoreFloat3(&mUp, worldUp);
	XMStoreFloat3(&mRight, XMVector3Cross(pos - target, worldUp));
	
	XMMATRIX viewMatrix = XMMatrixLookAtLH(pos, target, worldUp);
	XMStoreFloat4x4(&mView, viewMatrix);

}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up) {

	mPosition = pos;
	mLook = target;
	mUp = up;
	
	DirectX::XMVECTOR eyePos = DirectX::XMLoadFloat3(&pos);
	DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR upDirection = DirectX::XMLoadFloat3(&up);

	XMMATRIX viewMatrix = XMMatrixLookAtLH(eyePos, lookAt, upDirection);
	XMStoreFloat4x4(&mView, viewMatrix);
}

// Get View / Proj matrices
XMMATRIX Camera::View()const {

	return XMLoadFloat4x4(&mView);
}

XMMATRIX Camera::Proj()const {

	return XMLoadFloat4x4(&mProj);
}

XMMATRIX Camera::ViewProj()const {

	XMMATRIX aView = XMLoadFloat4x4(&mView);
	XMMATRIX aProj = XMLoadFloat4x4(&mProj);

	return aView * aProj;
}

// Strafe / Walk the camera a distance d
void Camera::Strafe(float d) {

	// mPosition += deltaTime * mRight
	// If creating a vector from a single scalar variable, we use VectorReplicate
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));
}

void Camera::Walk(float d) {

	XMFLOAT3 y = { 0, 0, 1 };

	// mPosition += deltaTime * mLook
	// If creating a vector from a single scalar variable, we use VectorReplicate
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&y);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));
}

// Rotate the camera
void Camera::Pitch(float angle) {

	// Rotate up and look vector about the right vector

	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
}

void Camera::RotateY(float angle) {

	// Rotate the basic vectors about the world y-axis

	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

}

// Update View Matrix after every frame 
void Camera::UpdateViewMatrix() {

	XMVECTOR R = XMLoadFloat3(&mRight);
	XMVECTOR U = XMLoadFloat3(&mUp);
	XMVECTOR L = XMLoadFloat3(&mLook);
	XMVECTOR P = XMLoadFloat3(&mPosition);

	// Orthonormalize the right, up and look vectors

	// Make look vector unit length

	L = XMVector3Normalize(L);

	// Compute a new corrected "up" vector and normalize it

	U = XMVector3Normalize(XMVector3Cross(L, R));

	// Compute a new corrected "right" vector. U and L are already ortho-normal, so no need to normalize cross product
	// || up x look || = || up || || look || sin90 = l
	R = XMVector3Cross(U, L);

	// Fill in the view matrix entries

	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);
	XMStoreFloat3(&mLook, L);

	mView(0, 0) = mRight.x;
	mView(1, 0) = mRight.y;
	mView(2, 0) = mRight.z;
	mView(3, 0) = x;

	mView(0, 1) = mUp.x;
	mView(1, 1) = mUp.y;
	mView(2, 1) = mUp.z;
	mView(3, 1) = y;

	mView(0, 2) = mLook.x;
	mView(1, 2) = mLook.y;
	mView(2, 2) = mLook.z;
	mView(3, 2) = z;

	mView(0, 3) = 0.0f;
	mView(1, 3) = 0.0f;
	mView(2, 3) = 0.0f;
	mView(3, 3) = 1.0f;
}


