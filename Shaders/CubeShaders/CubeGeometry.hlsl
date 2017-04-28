//----------------------------------------------------------------------------------------------------------------------------------//
// Geometry Shader DirectX11
//
// BTH - Fredrik Linde TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

// The registers are underlying hardware registers on the GPU where all data is stored during execution of the shaders
// There are different types of register for different types of data
// - Buffers uses "b"

cbuffer GS_CONSTANT_BUFFER : register(b0) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix inverseViewProjection;
	float4 cameraPos;

};

cbuffer PLATFORM_INSTANCE_BUFFER : register(b1) {

	matrix worldMatrix[30];
};

struct GS_IN
{
	float3 Pos : POSITION;
	float2 Tex: TEXCOORD;
	uint InstanceId : SV_InstanceId;

};

struct GS_OUT
{
	float4 Norm: NORMAL;
	float2 Tex: TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 WPos : WPOSITION;
	float3 ViewPos : POSITION1;
	
};

[maxvertexcount(3)]
 void GS_main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> triStream)
{	 
	 GS_OUT output;

	 uint i;

	float3 sideA = input[1].Pos.xyz - input[0].Pos.xyz;
	float3 sideB = input[2].Pos.xyz - input[0].Pos.xyz;

	float3 normalAB = normalize(cross(sideA, sideB));

		for (i = 0; i < 3; i++)
		{
			float3 worldPosition = mul(float4(input[i].Pos, 1.0f), matrixWorld).xyz;
			output.WPos = worldPosition;

			output.Pos = mul(float4(input[i].Pos.xyz, 1.0f), worldMatrix[input[i].InstanceId]);
			output.Pos = mul(float4(output.Pos.xyz, 1.0f), matrixView);
			output.Pos = mul(float4(output.Pos.xyz, 1.0f), matrixProjection);

			output.Norm = mul(float4(normalAB, 1.0f), matrixWorld);
			output.Tex = input[i].Tex;

			output.ViewPos = cameraPos.xyz - worldPosition.xyz;

			triStream.Append(output);
		}

};