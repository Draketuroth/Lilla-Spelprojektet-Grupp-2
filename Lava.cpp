#include "Lava.h"
#include "MacroDefinitions.h"

Lava::Lava()
{
	map[0].filename = L"Textures\\HMap.raw";
	map[1].filename = L"Textures\\island-tut4.raw";
	map[2].filename = L"Textures\\island-tut4.raw";
	map[3].filename = L"Textures\\HMap3.raw";

	weightScalar = 1;
	lastMap = 1;
	currentMap = 0;

	rows = LAVADEPTH; 
	cols = LAVAWIDTH;
	NrOfVert = LAVADEPTH*LAVAWIDTH; 
}

Lava::~Lava()
{
}

void Lava::LoadRawFile()
{
	//vector<unsigned char> in(LAVADEPTH * LAVAWIDTH);
	

	ifstream inFile; 
	for (int j = 0; j < 4; j++)
	{
		map[j].in.resize(LAVADEPTH * LAVAWIDTH); 
		inFile.open(map[j].filename.c_str(), std::ios_base::binary);

		if (!inFile)
		{
			cout << "height map not found" << endl;
		}

		if (inFile)
		{
			inFile.read((char*)&map[j].in[0], (std::streamsize)map[j].in.size());
			inFile.close();
		}
		
		map[j].heightMap.resize(LAVADEPTH * LAVAWIDTH, 0);
		for (int i = 0; i < (LAVADEPTH * LAVAWIDTH); i++)
		{
			map[j].heightMap[i] = ((map[j].in[i] / 255.0f)*LAVAMAXHEIGHT)* -4;
		}
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

void Lava::VBuffer(ID3D11Device* device, int current, float weightScalar)
{
	vector<LavaVertex> verticis(rows*cols);

	float halfWidth = 0.5*GetWidth(); 
	float halfDepth = 0.5f*GetDepth(); 

	float patchWidth = GetWidth() / (cols - 1);
	float patchDepth = GetDepth() / (rows - 1);

	float u = 1.0f / (rows -1);
	float v = 1.0f / (cols -1);

	for (UINT i = 0; i < rows; ++i)
	{
		float z = halfDepth - i * patchDepth;
		for (UINT j = 0; j < cols; ++j)
		{
			float x = -halfDepth + j * patchWidth;
			float y = map[2].heightMap[i*cols + j] * weightScalar;
			y += map[3].heightMap[i*cols + j] * secondWeightScalar;
			if (secondWeightScalar > 0)
			{
				int hablababbla = 0;
			}
			verticis[i*cols + j].pos = XMFLOAT3(x, y, z);

			//sträcka texturen över griden
			verticis[i*cols + j].uv.y = j*u;
			verticis[i*cols + j].uv.x = i*v;

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


	//index.resize(NrOfVert* 4);

	//for (unsigned int i = 0; i < rows - 1; ++i)
	//{
	//	for (unsigned int j = 0; j < cols - 1; ++j)
	//	{
	//		//top row 2x2 quad
	//		index[k] = i*cols + j; 
	//		index[k + 1] = i*cols + j + 1; 
	//		//bottom row 2x2 quad
	//		index[k + 2] = (i + 1)*cols + j; 
	//		index[k + 3] = (i + 1)*cols + j + 1;
	//		//next quad
	//		k += 4; 
	//		indexCounter += 4; 
	//	}
	//}


	index.resize(NrOfVert * 6);

	for (unsigned int i = 0; i < rows - 1; i++)
	{
		for (unsigned int j = 0; j < cols - 1; j++)
		{
			index[k + 5] = (i + 1) * cols + j + 1;
			index[k + 4] = i * cols + j + 1;
			index[k + 3] = (i + 1) * cols + j;

			index[k + 2] = (i + 1) * cols + j;
			index[k + 1] = i * cols + j + 1;
			index[k] = i * cols + j;

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

int Lava::swap(int frameCounter, ID3D11Device* device)
{
	
	if (frameCounter <= 1000)
	{
		currentMap = 0;
		
		weightSwap = true;
	}
	else
	{
		lastMap = 1;
		weightSwap = false;
		
	}

	if (frameCounter >= 2000 && frameCounter <= 3000)
	{
		currentMap = 2;
		weightSwap = true;
	}
	else if (frameCounter > 3000)
	{
		lastMap = 3;
		weightSwap = false;
	}


	if (weightSwap)
	{
		if (weightScalar < 1)
		{
			weightScalar += 0.001f;
		}
		
		if (secondWeightScalar > 0)
		{
			secondWeightScalar -= 0.001f;
		}
		
		
	}
	else
	{
		if (secondWeightScalar < 1)
		{
			secondWeightScalar += 0.001f;
		}

		if (weightScalar > 0)
		{
			weightScalar -= 0.001f;
		}
	}

	this->VBuffer(device, currentMap, weightScalar);
	return currentMap; 
}
