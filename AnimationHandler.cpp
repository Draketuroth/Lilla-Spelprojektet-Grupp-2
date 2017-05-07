//----------------------------------------------------------------------------------------------------------------------------------//
// AnimationHandler.cpp
// 
// BTH - Fredrik Linde TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

#include "AnimationHandler.h"

//----------------------------------------------------------------------------------------------------------------------------------//
// PRIMARY FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------//

AnimationHandler::AnimationHandler() {

	gCharacterBoneBuffer = nullptr;
}

AnimationHandler::~AnimationHandler() {


}

void AnimationHandler::ReleaseAll() {

	SAFE_RELEASE(gCharacterBoneBuffer);
	SAFE_RELEASE(gEnemyBoneBUffer);
}

void AnimationHandler::UpdatePlayerAnimation(ID3D11DeviceContext* gDeviceContext, int animIndex, FileImporter &importer) {

	animTimePos = playerAnimTimePos;
	// Open up a new XMFLOAT4x4 array to temporarily store the updated joint transformations
	XMFLOAT4X4* localJointTransforms = new XMFLOAT4X4[importer.skinnedMeshes[0].hierarchy.size()];

	// Interpolate will sort out the interpolation for every joint's animation, thus returns a matrix for every iteration
	for (int i = 0; i < importer.skinnedMeshes[0].hierarchy.size(); i++) {

		localJointTransforms[i] = Interpolate(i, gDeviceContext, animIndex, importer); // check Interpolate function.
	}

	// With all the precalculated matrices at our disposal, let's update the transformations on the GPU
	ZeroMemory(&boneMappedResource, sizeof(boneMappedResource));
	gDeviceContext->Map(gCharacterBoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &boneMappedResource);
	CHARACTER_SKINNED_DATA* boneBufferPointer = (CHARACTER_SKINNED_DATA*)boneMappedResource.pData;

	// Set root node properties
	XMMATRIX globalTx, invBindPose, skinnedTx;

	// Initialize the root joint
	importer.skinnedMeshes[0].hierarchy[0].LocalTx = XMLoadFloat4x4(&localJointTransforms[0]);
	importer.skinnedMeshes[0].hierarchy[0].GlobalTx = importer.skinnedMeshes[0].hierarchy[0].LocalTx;	// skel[0].GlobalTx = skel[0].LocalTx

	globalTx = importer.skinnedMeshes[0].hierarchy[0].GlobalTx;
	invBindPose = importer.skinnedMeshes[0].hierarchy[0].inverseBindPoseMatrix;
	skinnedTx = globalTx * invBindPose;

	XMStoreFloat4x4(&boneBufferPointer->gBoneTransform[0], XMMatrixTranspose(skinnedTx));	// skel[0].GlobalTx * skel[0].invBindPose

	// Every joint must be updated before unmapping the subresource
	for (UINT i = 1; i < importer.skinnedMeshes[0].hierarchy.size(); i++) {

		// Create a reference to the currenct joint to be processed
		Joint_Container &b = importer.skinnedMeshes[0].hierarchy[i];

		// Get the current joint LOCAL transformation at the current animation time pose
		b.LocalTx = XMLoadFloat4x4(&localJointTransforms[i]);

		b.GlobalTx = importer.skinnedMeshes[0].hierarchy[b.parentIndex].GlobalTx * b.LocalTx;	// skel[b.parent].GlobalTx * b.LocalTx

		skinnedTx = b.GlobalTx * b.inverseBindPoseMatrix;

		// XMMatrixTranspose(skinnedTx)
		XMStoreFloat4x4(&boneBufferPointer->gBoneTransform[i], XMMatrixTranspose(skinnedTx)); // b.GlobalTx * b.invBindPose
	}

	gDeviceContext->Unmap(gCharacterBoneBuffer, 0);

	delete localJointTransforms;

}

XMFLOAT4X4 AnimationHandler::Interpolate(int jointIndex, ID3D11DeviceContext* gDeviceContext, int animIndex, FileImporter &importer) {

	// Animation has just started, so return the first keyframe

	int animationLength = importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Length - 1;

	if (animTimePos <= importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[0].TimePos) //first keyframe
	{

		XMFLOAT4X4 M;

		XMVECTOR S = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[0].Scale);
		XMVECTOR T = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[0].Translation);
		XMVECTOR Q = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[0].RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, T));

		return M;
	}

	// Animation has reached its end, so return the last keyframe

	else if (animTimePos >= importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[animationLength].TimePos) // last keyframe
	{

		XMFLOAT4X4 M;

		XMVECTOR S = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[animationLength].Scale);
		XMVECTOR P = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[animationLength].Translation);
		XMVECTOR Q = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[animationLength].RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));

		return M;
	}

	// Animation time is between two frames so they should be interpolated

	else 
	{

		XMFLOAT4X4 M;
		// I am using an int here to truncate the animation timepose to know which matrices I am interested about
		// Ex. if time is 1.2, the returning frame is 1.
		int currentFrameIndex = animTimePos;
		
		if (currentFrameIndex < 1) {

			currentFrameIndex = 1;
		}

		float kFirst = importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex].TimePos;
		float kLast = importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex + 1].TimePos;
		
		// Though the interpolation percent will be mainly responsible of returning a slightly changed matrix
		float interpolationPercent = (animTimePos - kFirst) / (kLast - kFirst);

		XMVECTOR kFirstScale = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex].Scale); // interpolating between the current keyframe and the comming keyframe.
		XMVECTOR kLastScale = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex + 1].Scale);

		XMVECTOR kFirstTranslation = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex].Translation);
		XMVECTOR kLastTranslation = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex + 1].Translation);

		XMVECTOR kFirstQuaternion = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex].RotationQuat);
		XMVECTOR kLastQuaternion = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex + 1].RotationQuat);

		XMVECTOR S = XMVectorLerp(kFirstScale, kLastScale, interpolationPercent);
		XMVECTOR T = XMVectorLerp(kFirstTranslation, kLastTranslation, interpolationPercent);
		XMVECTOR Q = XMQuaternionSlerp(kFirstQuaternion, kLastQuaternion, interpolationPercent);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, T));
		
		return M;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------//
// SECONDARY FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------//



