#pragma once
#ifndef BULLETSCOMPONENTS_H
#define BULLETSCOMPONENTS_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include <vector>
#include "MacroDefinitions.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

using namespace std;

class BulletComponents {

private:

public:

	BulletComponents();
	~BulletComponents();

	//------------------------------------------------------------//
	// BULLET PHYSICS GLOBAL VARIABLES
	//------------------------------------------------------------//

	btDynamicsWorld* bulletDynamicsWorld;
	btDispatcher* bulletDispatcher;
	btCollisionConfiguration* bulletCollisionConfig;
	btBroadphaseInterface* bulletBroadPhase;
	btConstraintSolver* bulletConstraintSolver;

	vector<btRigidBody*>rigidBodies;

	//------------------------------------------------------------//
	// BULLET PHYSICS FUNCTIONS
	//------------------------------------------------------------//

	void InitializeBulletPhysics();
	void ReleaseAll();
};


#endif BULLETSCOMPONENTS_H

