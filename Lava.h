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

	//h�mta raw filerna
	void LoadRawFile(); 

	void VBuffer(ID3D11Device* device); 
	void IBuffer(ID3D11Device* device);

	float GetWidth()const;
	float GetDepth()const;

	void ReleaseAll(); 

	void swap();

	ID3D11Buffer* LavaVB;
	ID3D11Buffer* LavaIB;

	vector<int> index;

	unsigned int indexCounter = 0;
private:
	
	HMap map[2]; 
	
	int rows; 
	int cols; 
	int NrOfVert;

};

#endif
