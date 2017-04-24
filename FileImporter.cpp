
#include "FileImporter.h"

FileImporter::FileImporter() {


}

FileImporter::~FileImporter() {


}

void FileImporter::readFormat() {

	Header headerContent;

	ifstream in("Format//vertexBinaryData.txt", ios::in | ios::binary);

	if (in.is_open()){

	in.read(reinterpret_cast<char*>(&headerContent), sizeof(Header));

	cout << "Number of meshes: " << headerContent.nrOfMeshes << endl;
	cout << "Number of lights: " << headerContent.nrOfLights << endl;
	cout << "Number of cameras: " << headerContent.nrOfCameras << endl;

	}

	in.close();

}