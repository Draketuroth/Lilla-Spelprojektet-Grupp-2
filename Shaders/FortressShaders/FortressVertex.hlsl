
cbuffer VS_CONSTANT_BUFFER : register(b0) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix fortressWorldMatrix;
	float4 cameraPos;
};

struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
};

struct VS_OUT
{
	float4 Norm: NORMAL;
	float2 Tex: TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 WPos : WPOSITION;
	float3 ViewPos : CAMERAPOS;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	// To store and calculate the World position for output to the pixel shader, the input position must be multiplied with the World matrix
	float3 worldPosition = mul(float4(input.Pos, 1.0f), fortressWorldMatrix).xyz;
	output.WPos = worldPosition;

	// To store and calculate the WorldViewProj, the input position must be multiplied with the WorldViewProj matrix

	output.Pos = mul(float4(input.Pos.xyz, 1.0f), fortressWorldMatrix);
	output.Pos = mul(float4(output.Pos.xyz, 1.0f), matrixView);
	output.Pos = mul(float4(output.Pos.xyz, 1.0f), matrixProjection);

	// For the normal to properly work and to later be used correctly when creating the basic diffuse shading, it's required to be computed in world coordinates

	output.Norm = mul(float4(input.Norm, 1.0f), fortressWorldMatrix);

	output.Tex = input.Tex;

	output.ViewPos = cameraPos.xyz - worldPosition.xyz;

	return output;

}