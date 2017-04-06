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

struct OBJStruct
{
	XMFLOAT3 Varr;
	XMFLOAT2 VTarr;
	XMFLOAT3 VNarr;

};


#endif
