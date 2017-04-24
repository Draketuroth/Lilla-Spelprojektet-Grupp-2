
#include "FileImporter.h"

FileImporter::FileImporter() {


}

FileImporter::~FileImporter() {


}

void FileImporter::readFormat() {

	Header fileHeader;
	char headerContent[3];

	ifstream in("Format//vertexBinaryData.txt", ios::in | ios::binary);

	if (in.is_open()){

	// Start reading the header
	in.read(reinterpret_cast<char*>(&headerContent), sizeof(Header));

	cout << "Number of meshes: " << (uint32_t)headerContent[0] << endl;
	fileHeader.nrOfMeshes = (uint32_t)headerContent[0];
	cout << "Number of cameras: " << (uint32_t)headerContent[1] << endl;
	fileHeader.nrOfCameras = (uint32_t)headerContent[1];
	cout << "Number of lights: " << (uint32_t)headerContent[2] << endl;
	fileHeader.nrOfLights = (uint32_t)headerContent[2];

	}

	in.close();

}