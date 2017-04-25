#ifndef LAVA_H
#define LAVA_H
#include <vector>
#include <string>
#include <d3d11.h>
#include "Timer.h"
#include <fstream>
#include <iostream>
#include "VertexType.h"


using namespace std; 

class Lava
{
public:
	struct HMap
	{
		wstring	filename;  
		vector<float> heightMap;
		vector<unsigned char> in; 
	};

	Lava();
	~Lava();

	//hämta raw filerna
	void LoadRawFile(); 

	void VBuffer(ID3D11Device* device, int current, float weightScalar);
	void IBuffer(ID3D11Device* device);

	float GetWidth()const;
	float GetDepth()const;

	void ReleaseAll(); 

	int swap(int frameCounter, ID3D11Device* device);


	ID3D11Buffer* LavaVB;
	ID3D11Buffer* LavaIB;

	vector<int> index;

	unsigned int indexCounter = 0;
	Timer time;
	

private:
	
	HMap map[4]; 

	int lastMap;
	int currentMap;

	float value;
	bool weightSwap;
	float weightScalar;
	float secondWeightScalar;

	int rows; 
	int cols; 
	int NrOfVert;
	int lastFrame;
};

#endif
