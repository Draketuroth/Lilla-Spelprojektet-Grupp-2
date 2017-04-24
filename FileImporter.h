
#ifndef FILEIMPORTER_H
#define FILEIMPORTER_H

#include <windows.h>

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include <fbxsdk.h>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "VertexType.h"
#include "MacroDefinitions.h"

struct Header {

	uint32_t nrOfMeshes;
	uint32_t nrOfLights;
	uint32_t nrOfCameras;
};

class FileImporter {

public:

	FileImporter();
	~FileImporter();

	void readFormat();

private:
};

#endif