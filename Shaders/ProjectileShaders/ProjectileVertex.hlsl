#define MAX_PROJECTILE 10

cbuffer GS_CONSTANT_BUFFER : register(b0) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix fortressWorldMatrix;
	matrix lightViewProj;
	float4 cameraPos;

};

cbuffer PROJECTILE_TRANSFORM : register(b1)
{
	matrix worldMatrix[MAX_PROJECTILE];
	
};

struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	uint InstanceId : SV_InstanceId;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	
};


VS_OUT VS_Main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(float4(input.Pos.xyz, 1.0f), worldMatrix[input.InstanceId]);
	output.Pos = mul(float4(output.Pos.xyz, 1.0f), matrixView);
	output.Pos = mul(float4(output.Pos.xyz, 1.0f), matrixProjection);

	output.Norm = input.Norm;

	output.Tex = input.Tex;
	return output;
}