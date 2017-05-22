#ifndef PLATFORM_H
#define PLATFORM_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include "MacroDefinitions.h"
#include "VertexType.h"
#include <vector>

#include "BulletComponents.h"
#include "FileImporter.h"

class Platform {

public:

	Platform();
	~Platform();

	void platformDecension();
	void platformAcension();
	void platformBreaking();

	float lerpScalar;

	btRigidBody* rigidBody;
	XMMATRIX originMatrix;
	XMMATRIX worldMatrix;
	bool renderCheck;
	int ID;
	bool Hit;
	btVector3 startPos;
	float platformDelay;
	float breakTimer;
	float ascensionTimer;
	float descensionTimer;

private:
};

#endif

