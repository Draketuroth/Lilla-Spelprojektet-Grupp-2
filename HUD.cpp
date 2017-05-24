#include "HUD.h"

using namespace std;
HUDClass::HUDClass()
{
	

}

HUDClass::~HUDClass()
{

}

bool HUDClass::setElementPos(ID3D11Device* &gDevice, int playerHP)
{
	HRESULT hr;
	
	

	HUDElements Elements[4] =
	{
		// POS				// UV
		-1.0f, 0.85f, 0.8f,	0.0f, 1.0f, //Bot left
		-1.0f, 1.0f, 0.8f,	0.0f, 0.0f, //Top left
		-0.7f, 1.0f, 0.8f,	1.0f, 0.0f, //Top right

		-0.7f, 0.85f, 0.8f,	1.0f ,1.0f	//Bot right	

		
		


	};


	D3D11_BUFFER_DESC ElementBufferDesc;
	ZeroMemory(&ElementBufferDesc, sizeof(ElementBufferDesc));

	ElementBufferDesc.ByteWidth = sizeof(HUDElements) * 4;
	ElementBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ElementBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ElementBufferDesc.MiscFlags = 0;
	ElementBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = Elements;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&ElementBufferDesc, &vertexData, &gElementVertexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	float xLength = 0.216;
	float hpLoss = xLength / 10;

	float right = -0.704;
	float hpMul = 0.0f;
	if (playerHP < 10)
	{
		hpMul = 10.0f - playerHP;
	}
	

	HUDElements Health[4] =
	{
		// POS					// UV
		-0.92f, 0.86f, 0.0f,	0.0f, 1.0f, //Bot left
		-0.92f, 0.99f, 0.0f,		0.0f, 0.0f, //Top left
		right - hpLoss * hpMul, 0.99f, 0.0f,		1.0f, 0.0f, //Top right

		right - hpLoss * hpMul, 0.86f, 0.0f,		1.0f ,1.0f	//Bot right	





	};


	D3D11_BUFFER_DESC ElementBufferDesc2;
	ZeroMemory(&ElementBufferDesc2, sizeof(ElementBufferDesc2));

	ElementBufferDesc2.ByteWidth = sizeof(HUDElements) * 4;
	ElementBufferDesc2.Usage = D3D11_USAGE_DEFAULT;
	ElementBufferDesc2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ElementBufferDesc2.MiscFlags = 0;
	ElementBufferDesc2.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData2;
	vertexData2.pSysMem = Health;
	vertexData2.SysMemPitch = 0;
	vertexData2.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&ElementBufferDesc2, &vertexData2, &gHPBarVtxBuffer);

	if (FAILED(hr)) {

		return false;
	}





	return true;
}
void HUDClass::ReleaseAll()
{
	SAFE_RELEASE(gElementVertexBuffer);
	SAFE_RELEASE(gElementIndexBuffer);
	SAFE_RELEASE(gFontVertexBuffer);
	SAFE_RELEASE(gFontIndexBuffer);
	SAFE_RELEASE(gHPBarVtxBuffer);
}

bool HUDClass::CreateIndexBuffer(ID3D11Device* &gDevice)
{
	HRESULT hr;

	int indexBuffer[6] = { 0, 1, 2, 0, 2, 3 };


	D3D11_BUFFER_DESC ElementBufferDesc;
	ZeroMemory(&ElementBufferDesc, sizeof(ElementBufferDesc));

	ElementBufferDesc.ByteWidth = sizeof(int) * 6;
	ElementBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ElementBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ElementBufferDesc.MiscFlags = 0;
	ElementBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = indexBuffer;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&ElementBufferDesc, &vertexData, &gElementIndexBuffer);

	if (FAILED(hr)) {

		return false;
	}
	return true;
}

bool HUDClass::setFont(ID3D11Device* &gDevice)
{
	HRESULT hr;

	float width = 0, height = 0;
	width = 25.0f / 256;
	height = 32.0f / 256;
	Vtxs = 4 * nrOfChars;
	float posXleft = 0.0f, posYtop = 0.95f, posXright = 0.025f, posYbot = 0.9f;
	float uStart = 0, vStart = 0, uEnd = 0, vEnd = 0;
	int index = 0;
	
	vector<HUDElements> Elements;
	Elements.resize(Vtxs);

	for (unsigned int i = 0; i < nrOfChars; i++)
	{
		for (int k = 0; k < 69; k++)
		{
			if (ascii[i] == indexArr[k][0])
			{
				uStart = indexArr[k][1];
				vStart = indexArr[k][2];
				uEnd = uStart + indexArr[k][3];
				vEnd = vStart + indexArr[k][4];
				uStart /= 256;
				vStart /= 256;
				uEnd /= 256;
				vEnd /= 256;
			}
		}
			
			Elements[index] =
			{
				// POS				// UV
				posXleft, posYbot, 0.0f,	uStart, vEnd, //Bot left

			};
			index++;
			Elements[index] =
			{
				// POS				// UV
				posXleft, posYtop, 0.0f,	uStart, vStart, //top left

			};
			index++;
			Elements[index] =
			{
				// POS				// UV
				posXright, posYtop, 0.0f,	uEnd, vStart, //top right

			};
			index++;
			Elements[index] =
			{
				// POS				// UV
				posXright, posYbot, 0.0f,	uEnd, vEnd, //Bot right

			};
			index++;
			posXleft += 0.03f;
			posXright += 0.03f;
			

	}



	ascii.clear();
	

	D3D11_BUFFER_DESC ElementBufferDesc;
	ZeroMemory(&ElementBufferDesc, sizeof(ElementBufferDesc));

	ElementBufferDesc.ByteWidth = sizeof(HUDElements)* Vtxs;
	ElementBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ElementBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ElementBufferDesc.MiscFlags = 0;
	ElementBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = &Elements[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&ElementBufferDesc, &vertexData, &gFontVertexBuffer);

	if (FAILED(hr)) {

		return false;
	}
	return true;
}

bool HUDClass::CreateFontIndexBuffer(ID3D11Device* &gDevice)
{


	HRESULT hr;
	foo = nrOfChars * 6;
	vector<int> indexBuffer;
	indexBuffer.resize(foo);
	int index = 0;
	int vtxIndex = 0;
	for (int i = 0; i < nrOfChars; i++)
	{
	
		indexBuffer[index] = vtxIndex; // 0	//4
		vtxIndex++;
		index++;

		indexBuffer[index] = vtxIndex; // 1 //5
		vtxIndex++;
		index++;

		indexBuffer[index] = vtxIndex; // 2 //6
		vtxIndex-=2;
		index++;

		indexBuffer[index] = vtxIndex; // 0 //4
		vtxIndex+=2;
		index++;

		indexBuffer[index] = vtxIndex; // 2 //6
		vtxIndex++;
		index++;

		indexBuffer[index] = vtxIndex; // 3 //7
		vtxIndex++;
		index++;
	}
	


	D3D11_BUFFER_DESC ElementBufferDesc;
	ZeroMemory(&ElementBufferDesc, sizeof(ElementBufferDesc));

	ElementBufferDesc.ByteWidth = sizeof(int) * foo;
	ElementBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ElementBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ElementBufferDesc.MiscFlags = 0;
	ElementBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = &indexBuffer[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&ElementBufferDesc, &vertexData, &gFontIndexBuffer);
	
	if (FAILED(hr)) {

		return false;
	}
	return true;
}

void HUDClass::setText(int wave)
{
	stringstream ss;
	ss << wave;
	string waveStr = ss.str();

	waveText = "Wave " + waveStr;

	text.clear();
	nrOfChars = waveText.length();
	for (int i = 0; i < nrOfChars; i++)
	{
		text.push_back(waveText[i]);
		ascii.push_back((int)text[i]);
		
	}
	


}

void HUDClass::loadBitMap()
{
	fstream file("Fonts//HUDFont.txt", ios::in | ios::ate);
	string line;
	string subline;
	
	string::size_type sizeT;
	int indexloop = 0;

	// ID,X,Y,Width,Height

	if (!file.is_open())
	{
		cout << "Could not find bitmap file" << endl;
		return;
	}
	file.seekg(0, file.beg);

	while (!file.eof())
	{
		line.clear();

		getline(file, line);

		istringstream lineparse(line);
		for (unsigned int i = 0; i < 5; i++)
		{
			lineparse >> subline;
			indexArr[indexloop][i] = stoi(subline, &sizeT);
		}
		
		indexloop++;
	}
}