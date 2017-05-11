//----------------------------------------------------------------------------------------------------------------------------------//
// FBX Loader.h DirectX11
//
// BTH - Fredrik Linde TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

#ifndef ANIMATIONHANDLER_H
#define ANIMATIONHANDLER_H

#include <windows.h>

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "VertexType.h"
#include "MacroDefinitions.h"
#include "FileImporter.h"

// Necessary lib files kan be linked here, but also be added by going to:
// Properties->Linker->Input->Additional Dependencies
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace std;

//----------------------------------------------------------------------------------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------------------------------------------------------------------------------//

struct CHARACTER_SKINNED_DATA { // Struct to hold the Inverse Global Bind Pose matrices on the GPU

	XMFLOAT4X4 gBoneTransform[30];
};

struct ENEMY_SKINNED_DATA { // Struct to hold the Inverse Global Bind Pose matrices on the GPU

	XMFLOAT4X4 gBoneTransform[24];
};

struct VertexBlendInfo {

	unsigned int BlendingIndex;
	double BlendingWeight;

	VertexBlendInfo():

		BlendingIndex(0),
		BlendingWeight(0.0)

	{}

	bool operator < (const VertexBlendInfo& other) {

		return (BlendingWeight > other.BlendingWeight);
	}
};

struct Keyframe { // Stores the attributes of a keyframe in an animation

	float TimePos;
	XMFLOAT3 Translation;
	XMFLOAT3 Scale;
	XMFLOAT4 RotationQuat;

};

struct Animation {

	vector<Keyframe> Sequence;
	int Length;
};

struct Joint { // Stores the attributes of a joint node

	string Name;
	int ParentIndex;

	Animation Animations[3];

};

struct BlendingIndexWeightPair { // Middle hand container to help with passing Vertex/Skinning pair data

	int BlendIndex;
	double BlendWeight;

	BlendingIndexWeightPair() :
		BlendIndex(0),
		BlendWeight(0)
	{}
};

struct ControlPoint { // Resembles a physical vertex point in the FBX SDK

	XMFLOAT3 Position;
	vector<BlendingIndexWeightPair> BlendingInfo;

	ControlPoint() {

		BlendingInfo.reserve(4);
	}

};

struct Skeleton { // Stores every joint in the skeleton hierarchy from the loaded FBX file

	vector<Joint> hierarchy;
	int hierarchyDepth;

};

struct Blend { // Temporary struct containing a VertexBlendInfo vector for debugging purposes

	vector<VertexBlendInfo> VertexBlendingInfoList;

	void SortBlendingInfoByWeight() {

		sort(VertexBlendingInfoList.begin(), VertexBlendingInfoList.end());
	}

	bool operator==(const Blend& other) {

		bool sameBlendingInfo = true;

		// We only have to compare the blending information of another vertex if there is any blending information in that vertex
		if (!(VertexBlendingInfoList.empty() && other.VertexBlendingInfoList.empty())) {

			// Each vertex should only hold 4 index-weight blending info pairs
			for (unsigned int i = 0; i < 4; i++) {

				if (VertexBlendingInfoList[i].BlendingIndex != other.VertexBlendingInfoList[i].BlendingIndex ||
					abs(VertexBlendingInfoList[i].BlendingWeight - other.VertexBlendingInfoList[i].BlendingWeight) > 0.001) {

					sameBlendingInfo = false;
					break;
				}
			}
		}

	}
};

class AnimationHandler { // Handler Class to store FBX data and manage the Skeletal Animation System

public:

	//----------------------------------------------------------------------------------------------------------------------------------//
	// CONSTRUCTOR AND DESTRUCTOR
	//----------------------------------------------------------------------------------------------------------------------------------//

	AnimationHandler();
	~AnimationHandler();
	void ReleaseAll();

	//----------------------------------------------------------------------------------------------------------------------------------//
	// BUFFERS
	//----------------------------------------------------------------------------------------------------------------------------------//

	ID3D11Buffer* gCharacterBoneBuffer;
	ID3D11Buffer* gEnemyBoneBUffer;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// PRIMARY FUNCTIONS AND VARIABLES
	//----------------------------------------------------------------------------------------------------------------------------------//

	void UpdatePlayerAnimation(ID3D11DeviceContext* gDevice, int animIndex, FileImporter &importer);
	XMFLOAT4X4 Interpolate(int jointIndex, ID3D11DeviceContext* gDevice, int animIndex, FileImporter &importer);
	
	D3D11_MAPPED_SUBRESOURCE boneMappedResource;

	vector<Vertex_Bone>vertices;	// Extra copy of vertices

	float animTimePos;
	float playerAnimTimePos;
	float enemyAnimTimePos;

private:

	Blend vertexBlend;
	unordered_map<unsigned int, ControlPoint*>controlPoints;
	vector<int>indices;
	string animationName;

};

#endif

