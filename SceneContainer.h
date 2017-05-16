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
	void update(HWND &windowHandle);
	bool readFiles();

	void useAI(MainCharacter &player, Enemy* &enemy);

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

	BulletComponents bulletPhysicsHandler;

	//------------------------------------------------------------//
	// INSTANCE DATA
	//------------------------------------------------------------//

	ID3D11Buffer* gProjectileBuffer;
	ID3D11Buffer* gProjectileIndexBuffer;

	int nrOfEnemies;
	int nrOfIceEnemies;
	int nrOfLavaEnemies;
	vector<Enemy*> enemies;

	ICE_ENEMY_SKINNED_DATA iceEnemySkinData;
	LAVA_ENEMY_SKINNED_DATA lavaEnemySkinData;
	
	ID3D11Buffer* enemyIceVertexBuffer;
	vector<Vertex_Bone>iceEnemyVertices;

	ID3D11Buffer* enemyLavaVertexBuffer;
	vector<Vertex_Bone>lavaEnemyVertices;

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

	void renderLava();

	void renderProjectile();

	void createSideBoundingBoxes();
	void incrementLevels();

};




#endif //SCENECONTAINER_H

