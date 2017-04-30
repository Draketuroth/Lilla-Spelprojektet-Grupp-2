#pragma once
#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <d3d11.h>	
#include <d3dcompiler.h>	
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace std;

class SoundManager {

private:

public:

	SoundManager();
	~SoundManager();
	
	void ReleaseAll();

	bool Initialize();
	bool LoadSounds();

private:


};


#endif