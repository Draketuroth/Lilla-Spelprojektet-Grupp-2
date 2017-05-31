//----------------------------------------------------------------------------------------------------------------------------------//
// Vertex Shader DirectX11
//
// Fredrik Linde
//----------------------------------------------------------------------------------------------------------------------------------//

cbuffer VS_SKINNED_DATA: register (b0) {

	float4x4 gBoneTransforms[30];
};

cbuffer VS_CONSTANT_BUFFER : register(b1) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix fortressWorldMatrix;
	matrix lightViewProj;
	float4 cameraPos;
};

cbuffer PLAYER_TRANSFORM : register(b2) {

	matrix matrixW;
	matrix matrixWVP;
}

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
	float4 Norm: NORMAL;
	float2 Tex: TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 WPos : WPOSITION;
	float3 ViewPos : CAMERAPOS;
	float4 lPos : TEXCOORD1;
};


VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	// Perform vertex blending

	float3 position = float3(0.0f, 0.0f, 0.0f);
	float3 normal = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; ++i) // loop trough the 4 weights
	{

		position += input.Weight[i] * mul(float4(input.Pos, 1.0f), gBoneTransforms[input.BoneIndices[i]]).xyz; // the vertex position is affected by the joint movement and influenced by the corresponding weight

		normal += input.Weight[i] * mul(input.Norm, (float3x3)gBoneTransforms[input.BoneIndices[i]]); // we make it 3x3 matrix to skip homogenus and take away translation.
	}

	// To store and calculate the World position for output to the pixel shader, the input position must be multiplied with the World matrix
	float3 worldPosition = mul(float4(position, 1.0f), matrixW).xyz;
	output.WPos = worldPosition;

	//Light coordinate
	output.lPos = mul(position, mul(worldPosition, lightViewProj));

	// To store and calculate the WorldViewProj, the input position must be multiplied with the WorldViewProj matrix

	output.Pos = mul(float4(position, 1.0f), matrixWVP);

	// For the normal to properly work and to later be used correctly when creating the basic diffuse shading, it's required to be computed in world coordinates

	output.Norm = mul(float4(normal, 1.0f), matrixW);

	output.Tex = input.Tex;

	output.ViewPos = cameraPos.xyz - worldPosition.xyz;

	return output;
}