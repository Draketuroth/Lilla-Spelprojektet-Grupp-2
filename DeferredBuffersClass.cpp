
#include "DeferredBuffersClass.h"

void importer(vector<OBJStruct> &ImportStruct, MTL_STRUCT &MTLConstandData, int ParserSwitch, bool &fileFound, wstring &OBJTexturePath)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	struct String3
	{
		string x, y, z;
	};

	String3 initialize;
	initialize.x = "";
	initialize.y = "";
	initialize.z = "";
	string* Farr;
	vector<XMFLOAT3> VertexArr;
	vector<XMFLOAT2> VertexTarr;
	vector<XMFLOAT3> VertexNarr;

	OBJStruct Filler;
	Filler.Varr.x = 0;
	Filler.Varr.y = 0;
	Filler.Varr.z = 0;

	Filler.VNarr.x = 0;
	Filler.VNarr.y = 0;
	Filler.VNarr.z = 0;

	Filler.VTarr.x = 0;
	Filler.VTarr.y = 0;


	int FarrSize = 50;

	int Vsize = 0;
	int VTsize = 0;
	int VNsize = 0;
	int Fsize = 0;



	XMFLOAT3 initializeFloat3;
	initializeFloat3.x = 0;
	initializeFloat3.y = 0;
	initializeFloat3.z = 0;


	string delimiter = "/";





	Farr = new string[FarrSize];

	XMFLOAT3 null;
	null.x = 0;
	null.y = 0;
	null.z = 0;

	XMFLOAT2 null1;
	null1.x = 0;
	null1.y = 0;



	string::size_type sz;
	string check;
	string check2;
	string check3;
	string check4;


	if (ParserSwitch == 0)
	{
		fstream file("OBJfiles//test//cube.obj", ios::in | ios::ate);
		if (!file.is_open())
		{
			fileFound = false;
			return;
		}
		fileFound = true;
		string line;


		file.seekg(0, file.end);
		int size = file.tellg();
		file.seekg(0, file.beg);





		//----------------------------------------------------------------------------------//
		while (!file.eof())
		{
			line.clear();


			getline(file, line);
			istringstream stringParse(line);


			stringParse >> check;

			if (check == "v")
			{
				stringParse >> check2;
				stringParse >> check3;
				stringParse >> check4;

				VertexArr.push_back(null);
				VertexArr[Vsize].x = stod(check2, &sz);
				VertexArr[Vsize].y = stod(check3, &sz);
				VertexArr[Vsize].z = stod(check4, &sz);
				Vsize++;
			}

			if (check == "vt")
			{
				stringParse >> check2;
				stringParse >> check3;
				VertexTarr.push_back(null1);
				VertexTarr[VTsize].x = stod(check2, &sz);
				VertexTarr[VTsize].y = stod(check3, &sz);
				VTsize++;
			}

			if (check == "vn")
			{
				stringParse >> check2;
				stringParse >> check3;
				stringParse >> check4;
				VertexNarr.push_back(null);
				VertexNarr[VNsize].x = stod(check2, &sz);
				VertexNarr[VNsize].y = stod(check3, &sz);
				VertexNarr[VNsize].z = stod(check4, &sz);
				VNsize++;
			}

			if (check == "f")
			{
				stringParse >> check;
				stringParse >> check2;
				stringParse >> check3;
				//stringParse >> check4;

				Farr[Fsize] = check;
				Fsize++;
				Farr[Fsize] = check2;
				Fsize++;
				Farr[Fsize] = check3;
				Fsize++;
				/*Farr[Fsize] = check4;
				Fsize++;*/
			}

			if (FarrSize - Fsize < 4)
			{
				FarrSize += 50;


				string* temp = new string[FarrSize];


				for (size_t i = 0; i < Fsize; i++)
				{
					temp[i] = Farr[i];
				}

				delete[] Farr;
				Farr = temp;
			}



		}



		size_t pos = 0;
		vector<String3> FaceList;
		vector<XMFLOAT3> converter;


		for (int i = 0; i < Fsize; i++)
		{
			FaceList.push_back(initialize);
			pos = Farr[i].find(delimiter);
			FaceList[i].x = Farr[i].substr(0, pos);
			Farr[i].erase(0, Farr[i].find(delimiter) + delimiter.length());

			pos = Farr[i].find(delimiter);
			FaceList[i].y = Farr[i].substr(0, pos);
			Farr[i].erase(0, Farr[i].find(delimiter) + delimiter.length());

			FaceList[i].z = Farr[i];


		}
		string::size_type typer;

		for (int u = 0; u < Fsize; u++)
		{
			converter.push_back(initializeFloat3);
			converter[u].x = stof(FaceList[u].x, &typer);
			converter[u].y = stof(FaceList[u].y, &typer);
			converter[u].z = stof(FaceList[u].z, &typer);
			converter[u].x -= 1;
			converter[u].y -= 1;
			converter[u].z -= 1;

			//cout << converter[u].x << " " << converter[u].y << " " << converter[u].z << endl;
		}


		for (size_t i = 0; i < Fsize; i++)
		{

			ImportStruct.push_back(Filler);

			ImportStruct[i].Varr.x = VertexArr[converter[i].x].x;
			ImportStruct[i].Varr.y = VertexArr[converter[i].x].y;
			ImportStruct[i].Varr.z = VertexArr[converter[i].x].z;

			ImportStruct[i].VTarr.x = VertexTarr[converter[i].y].x;
			ImportStruct[i].VTarr.y = VertexTarr[converter[i].y].y;

			ImportStruct[i].VNarr.x = VertexNarr[converter[i].z].x;
			ImportStruct[i].VNarr.y = VertexNarr[converter[i].z].y;
			ImportStruct[i].VNarr.z = VertexNarr[converter[i].z].z;





		}
	}

	if (ParserSwitch == 1)
	{
		fstream mtl_File("OBJfiles//test//cube.mtl", ios::in | ios::ate);


		string temp = "OBJfiles//test//";
		wstring temp2;
		OBJTexturePath = (L"OBJfiles//test//");
		if (!mtl_File.is_open())
		{
			fileFound = false;
			return;
		}
		fileFound = true;
		string mtl_Line;


		mtl_File.seekg(0, mtl_File.end);
		int mtl_Size = mtl_File.tellg();
		mtl_File.seekg(0, mtl_File.beg);



		//string material;
		float illum;
		XMFLOAT3 Kd;
		XMFLOAT3 Ka;
		XMFLOAT3 Tf;
		XMFLOAT3 Ks;
		float Ni;


		while (!mtl_File.eof())
		{
			mtl_Line.clear();


			getline(mtl_File, mtl_Line);
			istringstream mtl_StringParse(mtl_Line);

			mtl_StringParse >> check;
			/*if (check == "newmtl")
			{
			mtl_StringParse >> material;
			}*/
			if (check == "illum")
			{
				mtl_StringParse >> check;
				illum = stof(check, &sz);
			}
			else if (check == "Kd")
			{
				mtl_StringParse >> check2;
				mtl_StringParse >> check3;
				mtl_StringParse >> check4;

				Kd.x = stof(check2, &sz);
				Kd.y = stof(check3, &sz);
				Kd.z = stof(check4, &sz);

			}
			else if (check == "Ka")
			{
				mtl_StringParse >> check2;
				mtl_StringParse >> check3;
				mtl_StringParse >> check4;

				Ka.x = stof(check2, &sz);
				Ka.y = stof(check3, &sz);
				Ka.z = stof(check4, &sz);
			}
			else if (check == "Tf")
			{
				mtl_StringParse >> check2;
				mtl_StringParse >> check3;
				mtl_StringParse >> check4;

				Tf.x = stof(check2, &sz);
				Tf.y = stof(check3, &sz);
				Tf.z = stof(check4, &sz);
			}
			else if (check == "Ni")
			{
				mtl_StringParse >> check;

				Ni = stof(check, &sz);
			}
			else if (check == "Ks")
			{
				mtl_StringParse >> check;
				mtl_StringParse >> check2;
				mtl_StringParse >> check3;

				Ks.x = stof(check, &sz);
				Ks.y = stof(check2, &sz);
				Ks.z = stof(check3, &sz);
			}
			else if (check == "map_Kd")
			{
				mtl_StringParse >> check;
				temp2.assign(check.begin(), check.end());
				OBJTexturePath.append(temp2);

			}

		}

		//--------- MTLCONSTANT DATA ASSIGNMENTS ---------------//
		MTLConstandData.Illum = illum;
		MTLConstandData.Ka.x = Ka.x;
		MTLConstandData.Ka.y = Ka.y;
		MTLConstandData.Ka.z = Ka.z;
		MTLConstandData.Kd.x = Kd.x;
		MTLConstandData.Kd.y = Kd.y;
		MTLConstandData.Kd.z = Kd.z;
		MTLConstandData.Tf.x = Tf.x;
		MTLConstandData.Tf.y = Tf.y;
		MTLConstandData.Tf.z = Tf.z;
		MTLConstandData.Ni = Ni;
		MTLConstandData.Ks.x = Ks.x;
		MTLConstandData.Ks.y = Ks.y;
		MTLConstandData.Ks.z = Ks.z;

		//-----------------------------------------------------------------//
		//cout << "material: " << material << endl;
		/*cout << "illum: " << illum << endl;
		cout << "kd: " << Kd.x << " " << Kd.y << " " << Kd.z << endl;
		cout << "ka: " << Ka.x << " " << Ka.y << " " << Ka.z << endl;
		cout << "Tf: " << Tf.x << " " << Tf.y << " " << Tf.z << endl;
		cout << "Ni: " << Ni << endl;
		cout << "Ks: " << Ks.x << " " << Ks.y << " " << Ks.z << endl;*/
	}



	delete[] Farr;


	//----------------------------------------------------------------------------------//

}

DeferredBuffersClass::DeferredBuffersClass() {

	int i;

	for (i = 0; i < BUFFER_COUNT; i++) {

		d_renderTargetTextureArray[i] = nullptr;
		d_renderTargetViewArray[i] = nullptr;
		d_shaderResourceViewArray[i] = nullptr;
	}

	d_depthStencilBuffer = nullptr;
	d_depthStencilView = nullptr;
}

DeferredBuffersClass::~DeferredBuffersClass() {


}

void DeferredBuffersClass::ReleaseAll() {

	int i;

	SAFE_RELEASE(d_depthStencilView);
	SAFE_RELEASE(d_depthStencilBuffer);

	for (i = 0; i < BUFFER_COUNT; i++) {

		SAFE_RELEASE(d_shaderResourceViewArray[i]);
		SAFE_RELEASE(d_renderTargetViewArray[i]);
		SAFE_RELEASE(d_renderTargetTextureArray[i]);
	}

	SAFE_RELEASE(gDeferredBuffer);
}

bool DeferredBuffersClass::Initialize(ID3D11Device* gDevice) {

//#ifdef DEBUG

	if (!SetupResources(gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Deferred Resources couldn't be initialized, investigate SetupResources function\nClosing application...",
			L"ERROR",
			MB_OK);

			return false;
	
	}


	if (!CreateDepthStencil(gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Depth Stencil resources couldn't be initialized, investigate CreateDepthStencil function\nClosing application...",
			L"ERROR",
			MB_OK);

			return false;

	}

	if (!CreateDeferredBuffer(gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Deferred buffer resources couldn't be initialized, investigate CreateDepthStencil function\nClosing application...",
			L"ERROR",
			MB_OK);

		return false;

	}

	CreateViewport();

	return true;
}

bool DeferredBuffersClass::SetupResources(ID3D11Device* gDevice) {

	HRESULT hr;
	UINT i;

	d_textureWidth = WIDTH;
	d_textureHeight = HEIGHT;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// TEXTURE DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));

	texDesc.Width = WIDTH;
	texDesc.Height = HEIGHT;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	for (i = 0; i < BUFFER_COUNT; i++) {

		hr = gDevice->CreateTexture2D(&texDesc, NULL, &d_renderTargetTextureArray[i]);

		if (FAILED(hr)) {

			return false;
		}
	}

	//----------------------------------------------------------------------------------------------------------------------------------//
	// RENDER TARGET VIEW DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));

	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	for (i = 0; i < BUFFER_COUNT; i++) {

		hr = gDevice->CreateRenderTargetView(d_renderTargetTextureArray[i], &rtvDesc, &d_renderTargetViewArray[i]);

		if (FAILED(hr)) {

			return false;
		}
	}

	//----------------------------------------------------------------------------------------------------------------------------------//
	// SHADER RESOURCE VIEW DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	for (i = 0; i < BUFFER_COUNT; i++) {

		hr = gDevice->CreateShaderResourceView(d_renderTargetTextureArray[i], &srvDesc, &d_shaderResourceViewArray[i]);

		if (FAILED(hr)) {

			return false;
		}
	}

	return true;
}

bool DeferredBuffersClass::CreateDepthStencil(ID3D11Device* gDevice){
	
	HRESULT hr;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// DEPTH TEXTURE DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	// THe depth buffer texture is used to store the distance of each fragment to the camera

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));

	descDepth.Width = WIDTH;
	descDepth.Height = HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	hr = gDevice->CreateTexture2D(&descDepth, nullptr, &d_depthStencilBuffer);

	if (FAILED(hr)) {

		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------------------//
	// DEPTH VIEW DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_DEPTH_STENCIL_VIEW_DESC dViewDesc;
	ZeroMemory(&dViewDesc, sizeof(dViewDesc));

	dViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dViewDesc.Texture2D.MipSlice = 0;
	dViewDesc.Flags = 0;

	hr = gDevice->CreateDepthStencilView(d_depthStencilBuffer, &dViewDesc, &d_depthStencilView);

	if (FAILED(hr)) {

		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------------------//
	// DEPTH STATE DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_DEPTH_STENCIL_DESC stencilDesc;
	ZeroMemory(&stencilDesc, sizeof(stencilDesc));

	// Depth test
	stencilDesc.DepthEnable = true;
	stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test
	stencilDesc.StencilEnable = true;
	stencilDesc.StencilReadMask = 0xFF;
	stencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if the pixel is facing forward

	stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if the pixel is facing backward

	stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create Depth State
	hr = gDevice->CreateDepthStencilState(&stencilDesc, &d_depthStencilState);

	if (FAILED(hr)) {

		return false;
	}

	return true;

}

bool DeferredBuffersClass::CreateDeferredBuffer(ID3D11Device* &gDevice) {

	float maxX = 0;
	float minX = 0;
	float maxY = 0;
	float minY = 0;
	float maxZ = 0;
	float minZ = 0;

	importer(ImportStruct, MTLConstantData, 0, fileFound, OBJTexturePath);
	int arrayLength = ImportStruct.size();
	for (int i = 0; i < arrayLength; i++)
	{
		//cout << "X: " << ImportStruct[i].Varr.x << " Y: " << ImportStruct[i].Varr.y << " Z: " << ImportStruct[i].Varr.z << endl;

		if (maxX < ImportStruct[i].Varr.x)
		{
			maxX = ImportStruct[i].Varr.x;
		}
		if (minX > ImportStruct[i].Varr.x)
		{
			minX = ImportStruct[i].Varr.x;
		}
		if (maxY < ImportStruct[i].Varr.y)
		{
			maxY = ImportStruct[i].Varr.y;
		}
		if (minY > ImportStruct[i].Varr.y)
		{
			minY = ImportStruct[i].Varr.y;
		}
		if (maxZ < ImportStruct[i].Varr.z)
		{
			maxZ = ImportStruct[i].Varr.z;
		}
		if (minZ > ImportStruct[i].Varr.z)
		{
			minZ = ImportStruct[i].Varr.z;
		}
	}
	int extentX = 0;
	int extentY = 0;
	int extentZ = 0;
	XMFLOAT3 bbCenter;
	float XCenter = (maxX / 2) + (minX / 2);
	float YCenter = (maxY / 2) + (minY / 2);
	float ZCenter = (maxZ / 2) + (minZ / 2);
	bbCenter = { XCenter, YCenter, ZCenter };

	HRESULT hr;

	if (fileFound == true)
	{
		D3D11_BUFFER_DESC bufferDesc;
		memset(&bufferDesc, 0, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = ImportStruct.size() * sizeof(OBJStruct);

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = &ImportStruct[0];
		hr = gDevice->CreateBuffer(&bufferDesc, &data, &gDeferredBuffer);

		if (FAILED(hr)) {

			return false;
		}

	}

	return true;
}

void DeferredBuffersClass::CreateViewport() {

	d_viewPort.Width = (float)d_textureWidth;
	d_viewPort.Height = (float)d_textureHeight;
	d_viewPort.MinDepth = 0.0f;
	d_viewPort.MaxDepth = 1.0f;
	d_viewPort.TopLeftX = 0.0f;
	d_viewPort.TopLeftY = 0.0f;

}

void DeferredBuffersClass::SetRenderTargets(ID3D11DeviceContext* gDeviceContext) {

	// Bind the render target view array and depth stencil buffer to the output render pipeline
	gDeviceContext->OMSetRenderTargets(BUFFER_COUNT, d_renderTargetViewArray, d_depthStencilView);

	// Set the viewport
	gDeviceContext->RSSetViewports(1, &d_viewPort);
}

void DeferredBuffersClass::ClearRenderTargets(ID3D11DeviceContext* gDeviceContext, float red, float green, float blue, float alpha) {

	float color[4];
	int i;

	// Setup the color to clear the buffer to
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the render target buffers
	for (i = 0; i < BUFFER_COUNT; i++) {

		gDeviceContext->ClearRenderTargetView(d_renderTargetViewArray[i], color);

	}

	// Clear the depth buffer

	gDeviceContext->ClearDepthStencilView(d_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

}

ID3D11ShaderResourceView* DeferredBuffersClass::GetShaderResourceView(int view) {

	return d_shaderResourceViewArray[view];
}