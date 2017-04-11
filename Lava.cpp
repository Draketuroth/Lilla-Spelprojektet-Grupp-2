#include "Lava.h"

Lava::Lava()
{
	map.filename = L"Textures\\HMap.raw";
}

Lava::~Lava()
{
}

void Lava::LoadRawFile()
{
	vector<unsigned char> in(HEIGHT * WIDTH); 

	ifstream inFile; 
	inFile.open(map.filename.c_str(), std::ios_base::binary); 

	if (!inFile)
	{
		cout << "height map not found" << endl; 
	}

	if (inFile)
	{
		inFile.read((char*)&in[0], (std::streamsize)in.size()); 
		inFile.close(); 
	}

	heightMap.resize(HEIGHT * WIDTH, 0);
	for (int i = 0; i < (HEIGHT * WIDTH); i++)
	{
		heightMap[i] = (in[i] / 255.0f)*MAXHEIGHT; 
	}
}

void Lava::ReleaseAll()
{

}