#ifndef LAVA_H
#define LAVA_H
#include <vector>
#include <string>
#include <d3d11.h>
#include "Timer.h"
#include <fstream>
#include <iostream>
#include "VertexType.h"
#include "MacroDefinitions.h"

#define LAVADEPTH 100
#define LAVAWIDTH 100 
#define LAVAMAXHEIGHT 10
#define LAVAQUADSIZE 1

using namespace std; 

class Lava
{
public:
	struct Map
	{
		wstring	filename;  
	};

	Lava();
	~Lava();

	//hämta raw filerna
	void LoadRawFile(); 

	void VBuffer(ID3D11Device* device); 
	void IBBuffer(ID3D11Device* device);

	float GetWidth()const;
	float GetDepth()const;

	void ReleaseAll(); 
	vector<float> hightmap; 

	ID3D11Buffer* LavaVB;
	ID3D11Buffer* LavaIB;

	vector<int> index;

private:
	Timer lavaTime;
	Map map; 
	vector<float> heightMap;
	unsigned int indexCounter = 0;
};


#endif
