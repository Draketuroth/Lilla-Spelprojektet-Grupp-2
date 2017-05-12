#ifndef VERTEXTYPE_H
#define VERTEXTYPE_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <iostream>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

struct TriangleVertex{

	float x, y, z;
	float u, v;
};

struct StandardVertex {

	float x, y, z;
	float u, v;
	float nx, ny, nz;
};


struct Vertex_Bone { // Stores the attributes of a vertex such as position, uv coordinates and normals

	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
	float weights[4];
	UINT boneIndices[4];

};

struct OBJStruct
{
	XMFLOAT3 Varr;
	XMFLOAT2 VTarr;
	XMFLOAT3 VNarr;

};

struct LavaVertex
{
	XMFLOAT3 pos; 
	XMFLOAT2 uv; 
	XMFLOAT3 normal; 
};
struct RayVertex
{
	XMFLOAT3 pos;
};
// collisionGroup
#define BIT(x) (1<<(x))
enum collisiontypes {
	COL_NOTHING = 0,        //<Collide with nothing
	COL_PLAYER = BIT(1),    //<Collide with player
	COL_LEVEL = BIT(2),      //<Collide with arena
	COL_ENEMY = BIT(3),     //<Collide with enemies
	COL_PROJ = BIT(4),      //<Collide with projectiles
	COL_RAY = BIT(5)        //<Collide with ray
};
#endif
