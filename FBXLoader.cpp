//----------------------------------------------------------------------------------------------------------------------------------//
// FBX Loader.cpp DirectX11
// Based in FBX SDK 2015 for Visual Studio 2012
// BTH - Fredrik Linde TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

#include "FBXLoader.h"

// loading funktions
//1. ProcessControlPoints (process vertices)
//2. LoadSkeletonHierarchy (processes joints and clusters)
//3. GatherAnimationData ( processes keyframes and weights)
//4. CreateVertexData ( makes vertex buffer for vertices) 

// updating 
//1. buffercomponents->createSkeletalBuffer.

//----------------------------------------------------------------------------------------------------------------------------------//
// PRIMARY FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------//

FbxImport::FbxImport() {

	gBoneBuffer = nullptr;
	gBoneVertexBuffer = nullptr;
}

FbxImport::~FbxImport() {


}

void FbxImport::ReleaseAll() {

	SAFE_RELEASE(gBoneBuffer);
	SAFE_RELEASE(gBoneVertexBuffer);
}

void FbxImport::UpdateAnimation(ID3D11DeviceContext* gDeviceContext, int animIndex, FileImporter &importer) {

	gDeviceContext->Map(gBoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &boneMappedResource);
	VS_SKINNED_DATA* boneBufferPointer = (VS_SKINNED_DATA*)boneMappedResource.pData;

	for (int i = 0; i < importer.skinnedMeshes[0].hierarchy.size(); i++) {

		Interpolate(boneBufferPointer,i, gDeviceContext, animIndex, importer); // check Interpolate function.
	}

	gDeviceContext->Unmap(gBoneBuffer, 0);

}

void FbxImport::Interpolate(VS_SKINNED_DATA* boneBufferPointer, int jointIndex, ID3D11DeviceContext* gDeviceContext, int animIndex, FileImporter &importer) {

	// Animation has just started, so return the first keyframe

	FbxLongLong animationLength = importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Length;

	if (animTimePos <= importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[0].TimePos) //first keyframe
	{

		XMFLOAT4X4 M;

		XMVECTOR S = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[0].Scale);
		XMVECTOR T = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[0].Translation);
		XMVECTOR Q = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[0].RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, T));

		boneBufferPointer->gBoneTransform[jointIndex] = XMMatrixTranspose(XMLoadFloat4x4(&M)) * XMMatrixTranspose(invertedBindPose[jointIndex]);
	}

	// Animation has reached its end, so return the last keyframe

	else if (animTimePos >= importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[animationLength - 1].TimePos) // last keyframe
	{

		XMFLOAT4X4 M;

		XMVECTOR S = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[animationLength - 1].Scale);
		XMVECTOR P = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[animationLength - 1].Translation);
		XMVECTOR Q = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[animationLength - 1].RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));

		boneBufferPointer->gBoneTransform[jointIndex] = XMMatrixTranspose(XMLoadFloat4x4(&M)) * XMMatrixTranspose(invertedBindPose[jointIndex]);
	}

	// Animation time is between two frames so they should be interpolated

	else 
	{

		XMFLOAT4X4 M;
		// I am using an int here to truncate the animation timepose to know which matrices I am interested about
		// Ex. if time is 1.2, the returning frame is 1.
		int currentFrameIndex = animTimePos;
		float kFirst = importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex].TimePos;
		float kLast = importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex + 1].TimePos;
		
		float interpolationProcent = (animTimePos - kFirst) / (kLast - kFirst);

		XMVECTOR kFirstScale = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex].Scale); // interpolating between the current keyframe and the comming keyframe.
		XMVECTOR kLastScale = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex + 1].Scale);

		XMVECTOR kFirstTranslation = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex].Translation);
		XMVECTOR kLastTranslation = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex + 1].Translation);

		XMVECTOR kFirstQuaternion = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex].RotationQuat);
		XMVECTOR kLastQuaternion = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex + 1].RotationQuat);

		XMVECTOR S = XMVectorLerp(kFirstScale, kLastScale, interpolationProcent);
		XMVECTOR T = XMVectorLerp(kFirstTranslation, kLastTranslation, interpolationProcent);
		XMVECTOR Q = XMQuaternionSlerp(kFirstQuaternion, kLastQuaternion, interpolationProcent);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, T));
		
		boneBufferPointer->gBoneTransform[jointIndex] = XMMatrixTranspose(XMLoadFloat4x4(&M)) * XMMatrixTranspose(invertedBindPose[jointIndex]);

	}
	//HLSL->boneshaders->vertexshader
}

//----------------------------------------------------------------------------------------------------------------------------------//
// SECONDARY FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------//

XMMATRIX FbxImport::Load4X4JointTransformations(Joint joint, int transformIndex){	// Function to specifically convert joint transformations to XMFLOAT4X4

	ofstream logFile;
	logFile.open("log.txt", ios::out | ios::app);

	logFile << "---------------------------\n" << "gBoneTransform " << transformIndex << "\n---------------------------" << endl;

	XMFLOAT4X4 matrix;
	
	matrix.m[0][0] = joint.GlobalBindposeInverse.Get(0, 0);
	matrix.m[0][1] = joint.GlobalBindposeInverse.Get(0, 1);
	matrix.m[0][2] = joint.GlobalBindposeInverse.Get(0, 2);
	matrix.m[0][3] = joint.GlobalBindposeInverse.Get(0, 3);
	logFile << (int)matrix.m[0][0] << " " << (int)matrix.m[0][1] << " " << (int)matrix.m[0][2] << " " << (int)matrix.m[0][3] << "\n";

	matrix.m[1][0] = joint.GlobalBindposeInverse.Get(1, 0);
	matrix.m[1][1] = joint.GlobalBindposeInverse.Get(1, 1);
	matrix.m[1][2] = joint.GlobalBindposeInverse.Get(1, 2);
	matrix.m[1][3] = joint.GlobalBindposeInverse.Get(1, 3);
	logFile << (int)matrix.m[1][0] << " " << (int)matrix.m[1][1] << " " << (int)matrix.m[1][2] << " " << (int)matrix.m[1][3] << "\n";

	matrix.m[2][0] = joint.GlobalBindposeInverse.Get(2, 0);
	matrix.m[2][1] = joint.GlobalBindposeInverse.Get(2, 1);
	matrix.m[2][2] = joint.GlobalBindposeInverse.Get(2, 2);
	matrix.m[2][3] = joint.GlobalBindposeInverse.Get(2, 3);
	logFile << (int)matrix.m[2][0] << " " << (int)matrix.m[2][1] << " " << (int)matrix.m[2][2] << " " << (int)matrix.m[2][3] << "\n";

	matrix.m[3][0] = joint.GlobalBindposeInverse.Get(3, 0);
	matrix.m[3][1] = joint.GlobalBindposeInverse.Get(3, 1);
	matrix.m[3][2] = joint.GlobalBindposeInverse.Get(3, 2);
	matrix.m[3][3] = joint.GlobalBindposeInverse.Get(3, 3);
	logFile << (int)matrix.m[3][0] << " " << (int)matrix.m[3][1] << " " << (int)matrix.m[3][2] << " " << (int)matrix.m[3][3] << "\n";
	
	XMMATRIX converted = XMLoadFloat4x4(&matrix);

	logFile << "\n";
	logFile.clear();

	return converted;
}

