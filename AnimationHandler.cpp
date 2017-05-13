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
	gEnemyBoneBuffer = nullptr;
}

AnimationHandler::~AnimationHandler() {


}

void AnimationHandler::ReleaseAll() {

	SAFE_RELEASE(gCharacterBoneBuffer);
	SAFE_RELEASE(gEnemyBoneBuffer);
}

void AnimationHandler::UpdatePlayerAnimation(ID3D11DeviceContext* gDeviceContext, int animIndex, FileImporter &importer, float playerTimePos) {

	animTimePos = playerTimePos;
	// Open up a new XMFLOAT4x4 array to temporarily store the updated joint transformations
	vector<XMFLOAT4X4> globalJointTransforms;
	globalJointTransforms.resize(importer.skinnedMeshes[0].hierarchy.size());

	// Interpolate will sort out the interpolation for every joint's animation, thus returns a matrix for every iteration
	for (int i = 0; i < importer.skinnedMeshes[0].hierarchy.size(); i++) {

		globalJointTransforms[i] = Interpolate(i, gDeviceContext, animIndex, importer); // check Interpolate function.
	}

	// With all the precalculated matrices at our disposal, let's update the transformations on the GPU
	ZeroMemory(&boneMappedResource, sizeof(boneMappedResource));
	gDeviceContext->Map(gCharacterBoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &boneMappedResource);
	CHARACTER_SKINNED_DATA* boneBufferPointer = (CHARACTER_SKINNED_DATA*)boneMappedResource.pData;

	for (UINT i = 0; i < importer.skinnedMeshes[0].hierarchy.size(); i++) {

		// Create a reference to the currenct joint to be processed
		Joint_Container &b = importer.skinnedMeshes[0].hierarchy[i];

		// Get the current joint LOCAL transformation at the current animation time pose
		b.GlobalTx = XMLoadFloat4x4(&globalJointTransforms[i]);

		XMStoreFloat4x4(&boneBufferPointer->gBoneTransform[i], XMMatrixTranspose(b.inverseBindPoseMatrix * b.GlobalTx));
	}

	gDeviceContext->Unmap(gCharacterBoneBuffer, 0);

}

void AnimationHandler::UpdateEnemyAnimation(ID3D11DeviceContext* gDeviceContext, FileImporter &importer, int currentInstance, int animIndex, float instanceTimePos) {

	// Clear last frame's jonit transformation
	iceEnemyFinalTransformations[currentInstance].clear();

	animTimePos = instanceTimePos;

	vector<XMFLOAT4X4> globalJointTransforms;
	globalJointTransforms.resize(importer.skinnedMeshes[0].hierarchy.size());

	// Interpolate will sort out the interpolation for every joint's animation, thus returns a matrix for every iteration
	for (int i = 0; i < importer.skinnedMeshes[0].hierarchy.size(); i++) {

		globalJointTransforms[i] = Interpolate(i, gDeviceContext, animIndex, importer); // check Interpolate function.
	}

	for (UINT i = 0; i < importer.skinnedMeshes[0].hierarchy.size(); i++) {

		// Create a reference to the currenct joint to be processed
		Joint_Container &b = importer.skinnedMeshes[0].hierarchy[i];

		// Get the current joint LOCAL transformation at the current animation time pose
		b.GlobalTx = XMLoadFloat4x4(&globalJointTransforms[i]);

		XMFLOAT4X4 finalTransform;

		// Store the final transformation of the current joint and push back to its transformation vector
		XMStoreFloat4x4(&finalTransform, XMMatrixTranspose(b.inverseBindPoseMatrix * b.GlobalTx));
		iceEnemyFinalTransformations[currentInstance].push_back(finalTransform);
	}

}

bool AnimationHandler::MapEnemyAnimations(ID3D11DeviceContext* gDeviceContext, int nrOfEnemies, FileImporter &importer) {

	HRESULT hr;

	// With all the precalculated matrices at our disposal, let's update the enemy transformations on the GPU
	ZeroMemory(&boneMappedResource, sizeof(boneMappedResource));
	
	// Check if the mapping was okay
	hr = gDeviceContext->Map(gEnemyBoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &boneMappedResource);

	if (FAILED(hr)) {

		return false;
	}

	// Create buffer to the instance buffer
	ICE_ENEMY_SKINNED_DATA* boneBufferPointer = (ICE_ENEMY_SKINNED_DATA*)boneMappedResource.pData;

	for (int instanceIndex = 0; instanceIndex < nrOfEnemies; instanceIndex++) {

		for (UINT transformIndex = 0; transformIndex  < iceEnemyFinalTransformations[instanceIndex].size(); transformIndex++) {

			// Get the current joint LOCAL transformation at the current animation time pose
			XMFLOAT4X4 finalTransform = iceEnemyFinalTransformations[instanceIndex][transformIndex];

			boneBufferPointer->enemyInstance[instanceIndex].gBoneTransform[transformIndex] = finalTransform;
		}
	}

	gDeviceContext->Unmap(gEnemyBoneBuffer, 0);

	return true;
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
		XMVECTOR T = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[animationLength].Translation);
		XMVECTOR Q = XMLoadFloat4(&importer.skinnedMeshes[0].hierarchy[jointIndex].Animations[animIndex].Sequence[animationLength].RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, T));

		return M;
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



