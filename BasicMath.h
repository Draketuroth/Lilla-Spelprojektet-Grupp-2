#pragma once
#include<string>
#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include "MacroDefinitions.h"
using namespace DirectX;
class basicMath
{
public:
	basicMath();
	~basicMath();
	float dot(XMFLOAT3 a, XMFLOAT3 b);
	float VectorLength(XMFLOAT3 Vector);
	XMFLOAT3 Cross(XMFLOAT3 VectorA, XMFLOAT3 VectorB);
	XMFLOAT3 Normalize(XMFLOAT3 VectorA);
	
private:

};




