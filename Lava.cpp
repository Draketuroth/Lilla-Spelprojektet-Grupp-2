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
	vector<unsigned char> in(LAVADEPTH * LAVAWIDTH);

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

	heightMap.resize(LAVADEPTH * LAVAWIDTH, 0);
	for (int i = 0; i < (LAVADEPTH * LAVAWIDTH); i++)
	{
		heightMap[i] = (in[i] / 255.0f)*LAVAMAXHEIGHT;
	}
}

void Lava::ReleaseAll()
{
	SAFE_RELEASE(LavaVB);
	SAFE_RELEASE(LavaIB);
}

float Lava::GetWidth()const
{
	return (LAVAWIDTH - 1)*LAVAQUADSIZE;
}

float Lava::GetDepth()const
{
	return (LAVADEPTH - 1)*LAVAQUADSIZE;
}

void Lava::VBuffer(ID3D11Device* device)
{
	vector<LavaVertex> verticis(LAVADEPTH*LAVAWIDTH);

	float halfWidth = 0.5*GetWidth(); 
	float halfDepth = 0.5f*GetDepth(); 

	float patchWidth = GetWidth() / (LAVAWIDTH - 1);
	float patchDepth = GetDepth() / (LAVADEPTH - 1);

	float u = 1.0f / (LAVAWIDTH - 1);
	float v = 1.0f / (LAVADEPTH - 1);
	int k = 0;

	for (UINT i = 0; i < LAVAWIDTH; ++i)
	{
		float z = 0;
		for (UINT j = 0; j < LAVADEPTH; ++j)
		{
			float x = 0;
			float y = 0;

			x = -halfDepth + j;
			y = heightMap[i*LAVAWIDTH + j];
			z = halfDepth - i;

			//cout << heightMap[i] << endl;
			verticis[i*LAVAWIDTH + j].pos = XMFLOAT3(x, y, z);

			//sträcka texturen över griden
			verticis[i*LAVAWIDTH + j].pos.y = j*u;
			verticis[i*LAVAWIDTH + j].pos.x = i*v;

			k++;
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(OBJStruct) * verticis.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &verticis[0];
	HRESULT(device->CreateBuffer(&vbd, &vinitData, &LavaVB));
}

void Lava::IBuffer(ID3D11Device* device)
{
	HRESULT hr;
	int k = 0;

	index.resize((LAVADEPTH * LAVAWIDTH) * 6);

	for (unsigned int i = 0; i < LAVAWIDTH - 1; i++)
	{
		for (unsigned int j = 0; j < LAVAWIDTH - 1; j++)
		{
			index[k + 5] = (i + 1) * LAVAWIDTH + j + 1;
			index[k + 4] = i * LAVAWIDTH + j + 1;
			index[k + 3] = (i + 1) * LAVAWIDTH + j;

			index[k + 2] = (i + 1) * LAVAWIDTH + j;
			index[k + 1] = i * LAVAWIDTH + j + 1;
			index[k] = i * LAVAWIDTH + j;

			//next quad
			k += 6;
			indexCounter += 6;

		}
	}

	D3D11_BUFFER_DESC ibd;
	memset(&ibd, 0, sizeof(ibd));
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.ByteWidth = index.size() * sizeof(int);
	
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = index.data();
	hr = device->CreateBuffer(&ibd, &iinitData, &LavaIB);

	if (hr != S_OK)
	{
		cout << "Error Index buffer" << endl;
	}
}