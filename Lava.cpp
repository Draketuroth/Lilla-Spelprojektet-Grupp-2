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
	vector<unsigned char> in(DEPTH * WIDTH);

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

	heightMap.resize(DEPTH * WIDTH, 0);
	for (int i = 0; i < (DEPTH * WIDTH); i++)
	{
		heightMap[i] = (in[i] / 255.0f)*MAXHEIGHT; 
	}
}

void Lava::ReleaseAll()
{

}

float Lava::GetWidth()const
{
	return (WIDTH - 1)*QUADSIZE; 
}

float Lava::GetDepth()const
{
	return (DEPTH - 1)*QUADSIZE;
}

void Lava::VBuffer(ID3D11Device* device)
{
	vector<LavaVertex> verticis(DEPTH*WIDTH);

	float halfWidth = 0.5*GetWidth(); 
	float halfDepth = 0.5f*GetDepth(); 

	float patchWidth = GetWidth() / (WIDTH - 1);
	float patchDepth = GetDepth() / (DEPTH - 1);

	float u = 1.0f / (WIDTH - 1); 
	float v = 1.0f / (DEPTH - 1);
	int k = 0;

	for (UINT i = 0; i < WIDTH; ++i)
	{
		float z = 0;
		for (UINT j = 0; j < DEPTH; ++j)
		{
			float x = 0;
			float y = 0;

			x = -halfDepth + j;
			y = heightMap[i*WIDTH + j];
			z = halfDepth - i;

			//cout << heightMap[i] << endl;
			verticis[i*WIDTH + j].pos = XMFLOAT3(x, y, z);

			//sträcka texturen över griden
			verticis[i*WIDTH + j].pos.y = j*u;
			verticis[i*WIDTH + j].pos.x = i*v;

			k++;
		}
	}
}