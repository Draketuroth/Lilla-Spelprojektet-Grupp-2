#ifndef LAVA_H
#define LAVA_H
#include <vector>
#include <string>
#include <d3d11.h>
#include "Timer.h"
#include <fstream>
#include <iostream>
#include "VertexType.h"

#define DEPTH 100
#define WIDTH 100 
#define MAXHEIGHT 10
#define QUADSIZE 1
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
	void IBuffer(ID3D11Device* device);

	float GetWidth()const;
	float GetDepth()const;

	void ReleaseAll(); 
	vector<float> hightmap; 

private:
	Timer lavaTime;
	Map map; 
	vector<float> heightMap;

};


#endif
