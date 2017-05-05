cbuffer GS_CONSTANT_BUFFER : register(b0) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix inverseViewProjection;
	matrix lightViewProj;
	float4 cameraPos;

};

struct VS_IN
{
	float3 pos : POSITION;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
};
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	output.pos = mul(float4(input.pos, 1.0f), mul(matrixWorld, lightViewProj));

	return output;
}