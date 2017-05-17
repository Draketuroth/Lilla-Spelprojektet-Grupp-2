#define MAX_JOINTS 24
#define MAX_ENEMY 20

cbuffer GS_CONSTANT_BUFFER : register(b0) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix fortressWorldMatrix;
	matrix lightViewProj;
	float4 cameraPos;

};
struct PerIceEnemyInstanceData {

	matrix gBoneTransform[MAX_JOINTS];
};
cbuffer ICE_ENEMY_TRANSFORM : register(b1) {

	matrix matrixW[MAX_ENEMY];
};

cbuffer ICE_ENEMY_SKINNED_DATA : register(b2) {

	PerIceEnemyInstanceData enemyInstance[MAX_ENEMY];
};

struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	float4 Weight : BLENDWEIGHT;
	uint4 BoneIndices : BLENDINDICES;
	uint InstanceId : SV_InstanceID;
};
struct VS_OUT
{
	float4 Pos : SV_POSITION;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	float3 position = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; ++i) // loop trough the 4 weights
	{
		position += input.Weight[i] * mul(float4(input.Pos, 1.0f), enemyInstance[input.InstanceId].gBoneTransform[input.BoneIndices[i]]).xyz; // the vertex position is affected by the joint movement and influenced by the corresponding weight
	}
	output.Pos = mul(float4(position, 1.0f), mul(matrixW[input.InstanceId], lightViewProj));

	return output;
}