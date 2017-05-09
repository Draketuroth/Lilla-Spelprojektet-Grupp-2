cbuffer GS_CONSTANT_BUFFER : register(b0) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix fortressWorldMatrix;
	matrix lightViewProj;
	float4 cameraPos;

};
cbuffer PLAYER_TRANSFORM : register(b1) {

	matrix matrixW;
	matrix matrixWVP;
}
cbuffer VS_SKINNED_DATA : register (b2) {

	float4x4 gBoneTransforms[16];
};


//struct VS_IN
//{
//	float3 pos : POSITION;
//};
//struct VS_OUT
//{
//	float4 pos : SV_POSITION;
//
//};
struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	float4 Weight : BLENDWEIGHT;
	uint4 BoneIndices : BLENDINDICES;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;

};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	float3 position = float3(0.0f, 0.0f, 0.0f);
	float3 normal = float3(0.0f, 0.0f, 0.0f);

	//for (int i = 0; i < 4; i++)
	//{
	//	position += input.Weight[i] * mul(float4(input.Pos, 1.0f), gBoneTransforms[input.BoneIndices[i]]).xyz;
	//}
	//output.Pos = mul(float4(position, 1.0f), mul(matrixW, lightViewProj));
	output.Pos = mul(float4(input.Pos, 1.0f), mul(matrixW, lightViewProj));

	return output;
}