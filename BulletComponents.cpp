
#include "BulletComponents.h"

BulletComponents::BulletComponents(){


}

BulletComponents::~BulletComponents() {


}

void BulletComponents::InitializeBulletPhysics() {

	// Initialize main Bullet Physics components

	// btCollisionConfiguration allows to configure Bullet collision detection stack allocator, pool memory allocators
	bulletCollisionConfig = new btDefaultCollisionConfiguration();

	// btCollisionDispatcher supports algorithms that handle ConvexConvex and ConvexConcave collision pairs
	bulletDispatcher = new btCollisionDispatcher(bulletCollisionConfig);

	// The btDbvtBroadphase implements a broadphase using two dynamic AABB bounding volume hierarchies/trees
	// Broad phase detection is typically a computationally low cost operation that quickly answers the question, “Which objects have a strong possibility of colliding?”
	bulletBroadPhase = new btDbvtBroadphase();

	// The btSequentialImpulseConstraintSolver is a fast SIMD implementation of the Projected Gauss Seidel (iterative LCP) method
	// In numerical linear algebra, the Gauss–Seidel method is an iterative method used to solve a linear system of equations
	bulletConstraintSolver = new btSequentialImpulseConstraintSolver();

	// btDiscreteDynamicsWorld provides discrete rigid body simulation those classes replace the obsolete CcdPhysicsEnvironment/CcdPhysicsController
	bulletDynamicsWorld = new btDiscreteDynamicsWorld(bulletDispatcher, bulletBroadPhase, bulletConstraintSolver, bulletCollisionConfig);

	// Immediately set the gravity after initializing main Bullet Physics components
	bulletDynamicsWorld->setGravity(btVector3(0.0f, -10, 0));

}

void BulletComponents::ReleaseAll() {

	for (UINT i = 0; i < rigidBodies.size(); i++) {

		bulletDynamicsWorld->removeCollisionObject(rigidBodies[i]);
		btMotionState* motion = rigidBodies[i]->getMotionState();
		btCollisionShape* shape = rigidBodies[i]->getCollisionShape();
		delete rigidBodies[i];
		delete motion;
		delete shape;
	}

	for (UINT i = 0; i < enemyRigidBodies.size(); i++) {

		bulletDynamicsWorld->removeCollisionObject(enemyRigidBodies[i]);
		btMotionState* motion = enemyRigidBodies[i]->getMotionState();
		btCollisionShape* shape = enemyRigidBodies[i]->getCollisionShape();
		delete enemyRigidBodies[i];
		delete motion;
		delete shape;
	}

	delete bulletDispatcher;
	delete bulletCollisionConfig;
	delete bulletConstraintSolver;
	delete bulletDynamicsWorld;
	delete bulletBroadPhase;
}