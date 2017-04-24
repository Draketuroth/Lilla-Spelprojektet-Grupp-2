
#include "FileImporter.h"

FileImporter::FileImporter() {


}

FileImporter::~FileImporter() {


}

void FileImporter::readFormat() {

	uint32_t headerContent[3];

	ifstream in("Format\\vertexBinaryData", ios::binary);

	in.read(reinterpret_cast<char*>(&headerContent), sizeof(headerContent));


}