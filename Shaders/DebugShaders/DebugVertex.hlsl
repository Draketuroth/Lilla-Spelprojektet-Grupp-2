
cbuffer GS_CONSTANT_BUFFER : register(b0)
{
	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix inverseViewProjection;
	float4 cameraPos;
};

cbuffer VS_SKINNED_DATA: register (b1) {

	float4x4 gBoneTransforms[16];
};

struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	uint InstanceId : SV_InstanceId;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	float4 position = float4(input.Pos, 1.0f);

	output.Pos = mul(position, gBoneTransforms[input.InstanceId]);
	output.Pos = mul(output.Pos,matrixView);
	output.Pos = mul(output.Pos, matrixProjection);

	output.Tex = input.Tex;

	return output;
}