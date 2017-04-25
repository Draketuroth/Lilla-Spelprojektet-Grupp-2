
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
	
		cout << "Number of meshes: " << fileHeader.nrOfMeshes << endl;
		cout << "Number of cameras: " << fileHeader.nrOfCameras << endl;
		cout << "Number of lights: " << fileHeader.nrOfLights << "\n\n";

		cout << "---------------------MESH SUB HEADER---------------------" << endl;
	
		for (UINT i = 0; i < fileHeader.nrOfMeshes; i++) {

			// Next is the mesh sub-header. There will be 25 bytes for each mesh found. 
			Mesh_Header meshSubHeaderContent;

			in.read(reinterpret_cast<char*>(&meshSubHeaderContent), sizeof(Mesh_Header));

			cout << "Vertex layout: " << meshSubHeaderContent.vertexLayout << endl;
			cout << "Vertices: " << meshSubHeaderContent.controlPoints << endl;
			cout << "Joints: " << meshSubHeaderContent.hierarchySize << endl;
			cout << "Keyframes: " << meshSubHeaderContent.keyframes << endl;
			cout << "Texture: " << meshSubHeaderContent.hasTexture << "\n\n";

			meshHeader.push_back(meshSubHeaderContent);

		}

		for (UINT i = 0; i < fileHeader.nrOfMeshes; i++) {

			cout << "---------------------MESH " << i << "---------------------" << endl;

			if (meshHeader[i].vertexLayout == 0) {

				Mesh_Transform meshTransformation;
				in.read(reinterpret_cast<char*>(&meshTransformation), sizeof(Mesh_Transform));
				cout << "Position: " << meshTransformation.meshPosition.x << ", "
									<< meshTransformation.meshPosition.y << ", "
									<< meshTransformation.meshPosition.z << endl;

				cout << "Rotation: " << meshTransformation.meshRotation.x << ", "
									<< meshTransformation.meshRotation.y << ", "
									<< meshTransformation.meshRotation.z << endl;

				cout << "Scale: " << meshTransformation.meshScale.x << ", "
								 << meshTransformation.meshScale.y << ", "
								 << meshTransformation.meshScale.z << endl;

				Material_Attributes materialAttributes;
				in.read(reinterpret_cast<char*>(&materialAttributes), sizeof(Material_Attributes));

				cout << "Ambient: " << materialAttributes.ambient.x << ", "
									<< materialAttributes.ambient.y << ", "
									<< materialAttributes.ambient.z << ", "
									<< materialAttributes.ambient.w << endl;

				cout << "Diffuse: " << materialAttributes.diffuse.x << ", "
									<< materialAttributes.diffuse.y << ", "
									<< materialAttributes.diffuse.z << ", "
									<< materialAttributes.diffuse.w << endl;

				cout << "Specular: " << materialAttributes.specular.x << ", "
									 << materialAttributes.specular.y << ", "
									 << materialAttributes.specular.z << ", "
									 << materialAttributes.specular.w << endl;

				if (meshHeader[i].hasTexture) {

					string textureName;
					in.read(reinterpret_cast<char*>(&textureName), sizeof(string));
				}

				for (UINT i = 0; i << meshHeader[i].controlPoints; i++) {


				}



			}

			else if (meshHeader[i].vertexLayout == 1) {


			}
		
		
		}
		


	}

	else {

		in.close();
	}

	//in.close();

}