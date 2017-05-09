cbuffer GS_CONSTANT_BUFFER : register(b0) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix fortressWorldMatrix;
	matrix lightViewProj;
	float4 cameraPos;

};

cbuffer PLATFORM_INSTANCE_BUFFER : register(b1) {

	matrix worldMatrix[400];
};

struct VS_IN {
	float3 pos : POSITION;
	uint InstanceId : SV_InstanceID;
};
struct VS_OUT {
	float4 pos : SV_POSITION;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	output.pos = mul(float4(input.pos, 1.0f), mul(worldMatrix[input.InstanceId], lightViewProj));

	return output;
}