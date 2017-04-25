
#include "FileImporter.h"

FileImporter::FileImporter() {


}

FileImporter::~FileImporter() {


}

void FileImporter::readFormat() {

	ifstream in("Format//vertexBinaryData.txt", ios::binary);

	if (in.is_open()){

		// Start reading the header. 12 bytes in total
		Header fileHeader;

		in.read(reinterpret_cast<char*>(&fileHeader), sizeof(Header));

		cout << "--------------------------HEADER-------------------------" << endl;
	
		cout << "Number of meshes: " << (uint32_t)fileHeader.nrOfMeshes << endl;
		cout << "Number of cameras: " << (uint32_t)fileHeader.nrOfCameras << endl;
		cout << "Number of lights: " << (uint32_t)fileHeader.nrOfLights << "\n\n";

		cout << "---------------------MESH SUB HEADER---------------------" << endl;
	
		// Next is the mesh sub-header. There will be 25 bytes for each mesh found. 
		Mesh_Header meshSubHeaderContent;
	
		for (UINT i = 0; i < fileHeader.nrOfMeshes; i++) {

			in.read(reinterpret_cast<char*>(&meshSubHeaderContent), sizeof(Mesh_Header));

			cout << "Vertex layout: " << (uint32_t)meshSubHeaderContent.vertexLayout << endl;
			cout << "Vertices: " << (uint32_t)meshSubHeaderContent.controlPoints << endl;
			cout << "Joints: " << (uint32_t)meshSubHeaderContent.hierarchySize << endl;
			cout << "Keyframes: " << (uint32_t)meshSubHeaderContent.keyframes << endl;
			cout << "Texture: " << (uint32_t)meshSubHeaderContent.hasTexture << "\n\n";

		}
		

	}

	else {

		in.close();
	}

	//in.close();

}