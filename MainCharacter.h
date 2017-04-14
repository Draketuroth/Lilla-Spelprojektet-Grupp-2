#pragma once
#include "CharacterBase.h"


class MainCharacter: public CharacterBase
{
private:


public:
	MainCharacter();
	~MainCharacter();

	void update(HWND windowhandle);

	void CharacterMove(HWND windowhandle);
	bool CheckInput(XMFLOAT3 &direction);

	XMMATRIX rotate(HWND windowhandle);
	
	Camera camera;
	XMVECTOR plane;


	float cameraDistanceY;
	float cameraDistanceZ;
	float playerHeight;




	XMFLOAT3 direction;
	XMFLOAT3 newCameraPos;
	XMVECTOR directionVec;
	XMFLOAT3 floatPos;
	XMVECTOR getPlane();
	
	void FillVertexBuffer();




	bool IntersectionInRange(XMFLOAT3 MousePos);

};

