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

// Default 20
#define ASCENSION_LIMIT 20

// Default 20
#define DESCENSION_LIMIT 20

// Default 5
#define BREAK_LIMIT 5

class Platform {

public:

	Platform();
	~Platform();

	void platformDecension();
	int platformAcension();
	void platformBreaking();
	bool checkState();

	float lerpScalar;

	btRigidBody* rigidBody;
	btTransform originMatrix;
	XMMATRIX worldMatrix;
	bool renderCheck;
	int textureFlag;
	int ID;
	bool Hit;
	bool Damaged;
	bool Restored;
	btVector3 startPos;
	float platformDelay;
	float breakTimer;
	float ascensionTimer;
	float descensionTimer;

private:
};

#endif

