
#ifndef FILEIMPORTER_H
#define FILEIMPORTER_H

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

//--------------------------------------------------------//
// IMPORTER VERTEX TYPES
//--------------------------------------------------------//

struct Vertex
{
	float pos[3];
	float uv[2];
	float normal[3];
	float binormal[3];
	float tangent[3];

};

struct Vertex_Deformer {

	float pos[3];
	float uv[2];
	float normal[3];
	float binormal[3];
	float tangent[3];
	float weights[4];
	uint32_t boneIndices[4];
};

//--------------------------------------------------------//
// HEADERS
//--------------------------------------------------------//

struct Header {

	uint32_t nrOfMeshes;
	uint32_t nrOfCameras;
	uint32_t nrOfLights;
};

struct Mesh_Header {

	uint32_t vertexLayout;
	uint32_t controlPoints;
	uint32_t hierarchySize;
	uint32_t animations;
	uint32_t hasTexture;
};

//--------------------------------------------------------//
// STRUCTS
//--------------------------------------------------------//

struct Mesh_Transform {

	XMFLOAT3 meshPosition;
	XMFLOAT3 meshRotation;
	XMFLOAT3 meshScale;
};

struct Material_Attributes {

	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
};

struct Mesh_Standard {

	Mesh_Transform meshTransformation;
	Material_Attributes materialAttributes;
	string textureName;
	vector<Vertex>vertices;
};

struct Joint_Keyframe {

	XMMATRIX GlobalTransform;
	float TimePos;
	XMFLOAT4 Translation;
	XMFLOAT4 Scale;
	XMFLOAT4 RotationQuat;
};

struct Joint_Animation {

	vector<Joint_Keyframe>Sequence;
	int Length;
};

struct Joint_Container {

	XMMATRIX inverseBindPoseMatrix;
	int parentIndex;
	Joint_Animation Animations[5];
};

struct Mesh_Skinned {

	Mesh_Transform meshTransformation;
	Material_Attributes materialAttributes;
	string textureName;
	vector<Vertex_Deformer> vertices;
	vector<Joint_Container>hierarchy;

};

struct Cameras {

	XMFLOAT3 position;
	XMFLOAT3 rotation;
};

struct Light {
	XMFLOAT3 position;
	XMFLOAT3 color;
};

class FileImporter {

public:

	FileImporter();
	~FileImporter();

	bool readFormat(string file);

	vector<Mesh_Header> meshHeader;
	vector<Mesh_Standard> standardMeshes;
	vector<Mesh_Skinned> skinnedMeshes;

	vector<Cameras> cameras;
	vector<Light> lights;

private:
};


#endif