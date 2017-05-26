#ifndef SCENECONTAINER_H
#define SCENECONTAINER_H

#include <windows.h>

#include <d3d11.h>	
#include <d3dcompiler.h>	

#include <iostream>
#include <Windows.h>
#include "Camera.h"
#include "WICTextureLoader.h"
#include "Window.h"

#include "DeferredBuffersClass.h"
#include "DeferredShaderClass.h"
#include "LightShaderClass.h"

#include "GraphicComponents.h"
#include "BufferComponents.h"
#include "TextureComponents.h"
#include "BulletComponents.h"

#include "MainCharacter.h"
#include "Enemies.h"

#include "AI.h"

#include "AnimationHandler.h"
#include "FileImporter.h"

#include "Lava.h"
#include <SFML\Audio.hpp>

#include "HUD.h"

#include <chrono>

struct MyCharacterContactResultCallback : public btCollisionWorld::ContactResultCallback
{
	MyCharacterContactResultCallback(CharacterBase* ptr) : character(ptr) {}

	btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap,
		int partId0,
		int index0,
		const btCollisionObjectWrapper* colObj1Wrap,
		int partId1,
		int index1)
	{
		btTransform t;
		t.setIdentity();
		t.setOrigin(btVector3(2, 2, 5));
		character->rigidBody->setWorldTransform(t);
		character->setAlive(false);
		return 0;
	}

	CharacterBase* character;
};

struct MyEnemyContactResultCallback : public btCollisionWorld::ContactResultCallback
{
	MyEnemyContactResultCallback(Enemy* ptr) : enemy(ptr) {}

	btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap,
		int partId0,
		int index0,
		const btCollisionObjectWrapper* colObj1Wrap,
		int partId1,
		int index1)
	{
		enemy->setAlive(false);
		return 0;
	}

	Enemy* enemy;
};

struct MyPlatformContactResultCallback : public btCollisionWorld::ContactResultCallback
{
	MyPlatformContactResultCallback(Platform* ptr) : platform(ptr) {}

	btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap,
		int partId0,
		int index0,
		const btCollisionObjectWrapper* colObj1Wrap,
		int partId1,
		int index1)
	{
		
		platform->Hit = true;

		return 0;
	}

	Platform* platform;
};



class SceneContainer {

private:
	AI ai;
	

public:

	//------------------------------------------------------------//
	// FUNCTIONS
	//------------------------------------------------------------//

	SceneContainer();
	~SceneContainer();

	void releaseAll();

	bool initialize(HWND &windowHandle);
	void update(HWND &windowHandle, float enemyTimePoses[30], Timer timer);
	bool readFiles();

	void useAI(MainCharacter &player, Enemy* &enemy, float enemyTimePos);

	void InitializeEnemies(ID3D11Device* graphicDevice, BulletComponents &bulletPhysicsHandle);
	void RespawnEnemies();
	bool createProjectileBox(ID3D11Device* gDevice);

	void loadIceEnemyVertices(FileImporter &importer, ID3D11Device* &graphicDevice);
	void loadLavaEnemyVertices(FileImporter &importer, ID3D11Device* &graphicDevice);

	bool createIceEnemyBuffer(ID3D11Device* &graphicDevice);
	bool createLavaEnemyBuffer(ID3D11Device* &graphicDevice);

	bool createIceEnemyBoneBuffer(ID3D11Device* &graphicDevice, ICE_ENEMY_SKINNED_DATA &skinData);
	bool createLavaEnemyBoneBuffer(ID3D11Device* &graphicDevice, LAVA_ENEMY_SKINNED_DATA &skinData);

	void reportLiveObjects();

	float RandomNumber(float Minimum, float Maximum);

	//Bounding "box" values

	//main Character
	float getRadiusCharacter();
	float getHeightCharacter();

	//lava enemy
	float getRadiusLava();
	float getHeightLava();

	//ice enemy
	float getRadiusIce();
	float getHeightIce();

	//------------------------------------------------------------//
	// RE-INTIALIZE
	//------------------------------------------------------------//
	
	void ReRelease();
	void ReInitialize();

	//------------------------------------------------------------//
	// FILES
	//------------------------------------------------------------//

	FileImporter mainCharacterFile;
	FileImporter iceEnemyFile;
	FileImporter FortressFile;
	FileImporter PlatformFile;
	FileImporter lavaEnemyFile;
	FileImporter projectileFile;

	//------------------------------------------------------------//
	// COMPONENTS
	//------------------------------------------------------------//

	GraphicComponents gHandler;
	BufferComponents bHandler;
	TextureComponents tHandler;

	DeferredBuffersClass deferredObject;
	DeferredShaderClass deferredShaders;
	LightShaderClass lightShaders;

	MainCharacter character;
	AnimationHandler animHandler;
	Lava lava; 
	HUDClass HUD;

	BoundingBox sides[4];

	int level;
	Timer sceneTimer;

	BulletComponents bulletPhysicsHandler;

	//------------------------------------------------------------//
	// INSTANCE DATA
	//------------------------------------------------------------//

	ID3D11Buffer* gProjectileBuffer;

	int nrOfEnemies;
	int nrOfIceEnemies;
	int nrOfLavaEnemies;
	vector<Enemy*> enemies;

	ICE_ENEMY_SKINNED_DATA iceEnemySkinData;
	LAVA_ENEMY_SKINNED_DATA lavaEnemySkinData;
	
	ID3D11Buffer* enemyIceVertexBuffer;
	vector<Vertex_Bone>iceEnemyVertices;

	vector<StandardVertex>ExplosionVertices;
	ID3D11Buffer* ExplosionVertexBuffer;

	vector<StandardVertex>projectileVertices;
	ID3D11Buffer* projectileVertexBuffer;

	ID3D11Buffer* enemyLavaVertexBuffer;
	vector<Vertex_Bone>lavaEnemyVertices;

	float waveDelay;
	int restoredCounter;
	bool respawnDelay;

	/*btTransform projectileTransform;
	projectileTransform.setIdentity();
	projectileTransform.setOrigin(btVector3(0, -20, 0));
	fireBall.projectileRigidBody->setWorldTransform(projectileTransform);*/

	//------------------------------------------------------------//
	// RENDER FUNCTIONS
	//------------------------------------------------------------//

	void drawFortress();
	void drawPlatforms();
	void drawDebugCubes();
	void drawHUD();
	
	void clear();
	void resetRenderTarget(GraphicComponents &gHandler);

	void render();

	bool renderDeferred();

	bool renderSceneToTexture();

	void renderScene();
	void renderCharacters();
	void renderIceEnemies();
	void renderLavaEnemies();
	void renderShadowMap();
	void renderRay();
	void renderLava();

	void renderProjectile();

	void createSideBoundingBoxes();
	

	

	void delayWave(Timer timer);
	void incrementLevels();

	void spawnEnemies();
};




#endif //SCENECONTAINER_H

