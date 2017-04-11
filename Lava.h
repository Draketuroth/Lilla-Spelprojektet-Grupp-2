#ifndef LAVA_H
#define LAVA_H
#include <vector>
#include <string>
#include <d3d11.h>
#include "Timer.h"
#include <fstream>
#include <iostream>

#define HEIGHT 100
#define WIDTH 100 
#define MAXHEIGHT 10

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


	void ReleaseAll(); 
	vector<float> hightmap; 

private:
	Timer lavaTime;
	Map map; 
	vector<float> heightMap;

};


#endif
