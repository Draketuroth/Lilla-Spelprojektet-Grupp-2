
#include "FileImporter.h"

FileImporter::FileImporter() {


}

FileImporter::~FileImporter() {


}

bool FileImporter::readFormat(string file) {

	ifstream in(file, ios::binary);

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
			cout << "Animations: " << meshSubHeaderContent.animations << endl;
			cout << "Texture: " << meshSubHeaderContent.hasTexture << "\n\n";

			meshHeader.push_back(meshSubHeaderContent);

		}

		for (UINT i = 0; i < fileHeader.nrOfMeshes; i++) {

			cout << "---------------------MESH " << i << "---------------------" << endl;

			// Check the vertex layout for the mesh

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

					uint32_t stringSize;
					string buffer;

					in.read(reinterpret_cast<char *>(&stringSize), sizeof(uint32_t));

					assert(stringSize < 30);
					
					buffer.resize(stringSize);

					in.read(&buffer[0], stringSize);
					currentMesh.textureName = &buffer[0];
					cout << "Texture Name: " << currentMesh.textureName.c_str() << endl;


				}

				else {

					currentMesh.textureName = "No texture attached to this mesh";
					cout << "Texture Name: " << currentMesh.textureName.c_str() << endl;

				}
					//------------------------------------------------------//
					// GATHER VERTICES
					//------------------------------------------------------//

					uint32_t vertexCount = meshHeader[i].controlPoints;

					Vertex* vertices = new Vertex[vertexCount];
					in.read(reinterpret_cast<char*>(vertices), sizeof(Vertex) * vertexCount);

					for (UINT i = 0; i < vertexCount; i++) {

						currentMesh.vertices.push_back(vertices[i]);
					}

					delete vertices;

					//------------------------------------------------------//
					// PUSH BACK STANDARD MESH
					//------------------------------------------------------//

					standardMeshes.push_back(currentMesh);

				}
			

			else if (meshHeader[i].vertexLayout == 1) {

				Mesh_Skinned currentMesh;

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

					uint32_t stringSize;
					string buffer;

					in.read(reinterpret_cast<char *>(&stringSize), sizeof(uint32_t));

					assert(stringSize < 30);

					buffer.resize(stringSize);

					in.read(&buffer[0], stringSize);
					currentMesh.textureName = &buffer[0];
					
					cout << "Texture Name: " << currentMesh.textureName.c_str() << endl;

				}

				else {

					currentMesh.textureName = "No texture attached to this mesh";
					cout << "Texture Name: " << currentMesh.textureName.c_str() << endl;

				}

				//------------------------------------------------------//
				// GATHER VERTICES
				//------------------------------------------------------//

				uint32_t vertexCount = meshHeader[i].controlPoints;

				Vertex_Deformer* vertices = new Vertex_Deformer[vertexCount];
				in.read(reinterpret_cast<char*>(vertices), sizeof(Vertex_Deformer) * vertexCount);

				for (UINT i = 0; i < vertexCount; i++) {

					currentMesh.vertices.push_back(vertices[i]);
				}

				delete vertices;

				//------------------------------------------------------//
				// GATHER JOINT BINDPOSE MATRICES
				//------------------------------------------------------//
				uint32_t jointCount = meshHeader[i].hierarchySize;

				Joint_Container* joints = new Joint_Container[jointCount];

				XMFLOAT4X4* bindPoseMatrices = new XMFLOAT4X4[jointCount];
				in.read(reinterpret_cast<char*>(bindPoseMatrices), sizeof(XMFLOAT4X4) * jointCount);

				for (UINT i = 0; i < jointCount; i++) {

					XMMATRIX currentBindPose = XMLoadFloat4x4(&bindPoseMatrices[i]);
					
					joints[i].inverseBindPoseMatrix = currentBindPose;
				}

				delete bindPoseMatrices;

				//------------------------------------------------------//
				// GATHER JOINT ANIMATIONS
				//------------------------------------------------------//

				uint32_t animationCount = meshHeader[i].animations;

				for(UINT animationIndex = 0; animationIndex < animationCount; animationIndex++){

					uint32_t keyFramesCount;
					in.read(reinterpret_cast<char*>(&keyFramesCount), sizeof(keyFramesCount));

				for(UINT i = 0; i < jointCount; i++){

					XMFLOAT4X4* jointGlobalTransforms = new XMFLOAT4X4[keyFramesCount];
					in.read(reinterpret_cast<char*>(jointGlobalTransforms), sizeof(XMFLOAT4X4) * keyFramesCount);

						for (UINT j = 0; j < keyFramesCount; j++) {

							Joint_Keyframe currentKeyFrameTransform;

							// Get keyframe matrix
							XMMATRIX keyFrameMatrix = XMLoadFloat4x4(&jointGlobalTransforms[j]);
							currentKeyFrameTransform.GlobalTransform = keyFrameMatrix;

							// Set time pose
							currentKeyFrameTransform.TimePos = j;

							// Split up keyframe matrix to scale, quaternion and translation
							XMVECTOR keyFrameScale, keyFrameRotationQuat, keyFrameTranslation;
							XMMatrixDecompose(&keyFrameScale, &keyFrameRotationQuat, &keyFrameTranslation, keyFrameMatrix);

							XMStoreFloat4(&currentKeyFrameTransform.Scale, keyFrameScale);
							XMStoreFloat4(&currentKeyFrameTransform.RotationQuat, keyFrameRotationQuat);
							XMStoreFloat4(&currentKeyFrameTransform.Translation, keyFrameTranslation);

							joints[i].Animations[animationIndex].Sequence.push_back(currentKeyFrameTransform);
						}

						joints[i].Animations[animationIndex].Length = keyFramesCount;

					}

				cout << "Animation " << animationIndex << " loaded!" << endl;

				}

				//------------------------------------------------------//
				// PUSH BACK SKINNED MESH
				//------------------------------------------------------//

				for (UINT i = 0; i < jointCount; i++) {

					currentMesh.hierarchy.push_back(joints[i]);
				}

				//------------------------------------------------------//
				// PUSH BACK SKINNED MESH
				//------------------------------------------------------//

				skinnedMeshes.push_back(currentMesh);

			}
		
		
		}

		for (UINT i = 0; i < fileHeader.nrOfCameras; i++)
		{
			Cameras camera;
			in.read(reinterpret_cast<char*>(&camera), sizeof(Cameras));
			cameras.push_back(camera);
			cout << "Camera pos X: " << camera.position.x << " Camera pos Y: " << camera.position.y << " Camera pos Z: " << camera.position.z << endl;
			cout << "Camera rot X: " << camera.rotation.x << " Camera rot Y: " << camera.rotation.y << " Camera rot Z: " << camera.rotation.z << endl;
 		}
		
		for (UINT i = 0; i < fileHeader.nrOfLights; i++)
		{
			Light light;
			in.read(reinterpret_cast<char*>(&light), sizeof(Light));
			lights.push_back(light);

			cout << "Light pos X: " << light.position.x << " Light pos Y: " << light.position.y << " Light pos Z: " << light.position.z << endl;
			cout << "Light color R: " << light.color.x << " Light color G: " << light.color.y << " Light color B: " << light.color.z << endl;

		}


	}

	else {

		cout << "Format couldn't be found, closing file reading..." << endl;
		in.close();
		
		return false;
	}

	// Close file when done reading
	in.close();

	return true;
	

}