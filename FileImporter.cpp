
#include "FileImporter.h"

FileImporter::FileImporter() {


}

FileImporter::~FileImporter() {


}

void FileImporter::readFormat() {

	ifstream in("Format//vertexBinaryData.txt", ios::binary);

	if (in.is_open()){

		// Start reading the header. 12 bytes in total.
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

				Mesh_Standard currentMesh;


				in.read(reinterpret_cast<char*>(&currentMesh.meshTransformation), sizeof(Mesh_Transform));
				cout << "Position: " << currentMesh.meshTransformation.meshPosition.x << ", "
					<< currentMesh.meshTransformation.meshPosition.y << ", "
					<< currentMesh.meshTransformation.meshPosition.z << endl;

				cout << "Rotation: " << currentMesh.meshTransformation.meshRotation.x << ", "
					<< currentMesh.meshTransformation.meshRotation.y << ", "
					<< currentMesh.meshTransformation.meshRotation.z << endl;

				cout << "Scale: " << currentMesh.meshTransformation.meshScale.x << ", "
					<< currentMesh.meshTransformation.meshScale.y << ", "
					<< currentMesh.meshTransformation.meshScale.z << endl;

				//------------------------------------------------------//
				// MATERIAL ATTRIBUTES
				//------------------------------------------------------//

				in.read(reinterpret_cast<char*>(&currentMesh.materialAttributes), sizeof(Material_Attributes));

				cout << "Ambient: " << currentMesh.materialAttributes.ambient.x << ", "
					<< currentMesh.materialAttributes.ambient.y << ", "
					<< currentMesh.materialAttributes.ambient.z << ", "
					<< currentMesh.materialAttributes.ambient.w << endl;

				cout << "Diffuse: " << currentMesh.materialAttributes.diffuse.x << ", "
					<< currentMesh.materialAttributes.diffuse.y << ", "
					<< currentMesh.materialAttributes.diffuse.z << ", "
					<< currentMesh.materialAttributes.diffuse.w << endl;

				cout << "Specular: " << currentMesh.materialAttributes.specular.x << ", "
					<< currentMesh.materialAttributes.specular.y << ", "
					<< currentMesh.materialAttributes.specular.z << ", "
					<< currentMesh.materialAttributes.specular.w << endl;

				//------------------------------------------------------//
				// GET TEXTURE NAME IF ATTACHED TO MESH
				//------------------------------------------------------//

				if (meshHeader[i].hasTexture) {

					in.read(reinterpret_cast<char*>(&currentMesh.textureName), sizeof(string));
					cout << "Texture Name: " << currentMesh.textureName.c_str() << endl;

				}

				else {

					currentMesh.textureName = "No texture attached to this mesh";
					cout << "Texture Name: " << currentMesh.textureName.c_str() << endl;

				}

				//------------------------------------------------------//
				// GATHER VERTICES
				//------------------------------------------------------//
				int vertexCount = meshHeader[i].controlPoints;

			
				//------------------------------------------------------//
				// PUSH BACK STANDARD MESH
				//------------------------------------------------------//

				standardMeshes.push_back(currentMesh);
			
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