
#include "FileImporter.h"

FileImporter::FileImporter() {


}

FileImporter::~FileImporter() {


}

void FileImporter::readFormat() {

	char headerContent[3];

	ifstream in("Format//vertexBinaryData.txt", ios::in | ios::binary);

	if (in.is_open()){

	in.read(reinterpret_cast<char*>(&headerContent), sizeof(Header));

	cout << "Number of meshes: " << (uint32_t)headerContent[0] << endl;
	cout << "Number of lights: " << (uint32_t)headerContent[1] << endl;
	cout << "Number of cameras: " << (uint32_t)headerContent[2] << endl;

	}

	in.close();

}