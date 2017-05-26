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
	matrix fortressWorldMatrix;
	matrix lightViewProj;
	float4 cameraPos;

};

cbuffer PLATFORM_INSTANCE_BUFFER : register(b1) {

	matrix worldMatrix[400];
	float4 textureFlag[400];
};

struct GS_IN
{
	float3 Pos : POSITION;
	float2 Tex: TEXCOORD;
	float3 Norm: NORMAL;
	float4 lPos : TEXCOORD1;
	uint InstanceId : SV_InstanceID;

};

struct GS_OUT
{
	float4 Norm: NORMAL;
	float2 Tex: TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 Random : WPOSITION;
	float3 ViewPos : POSITION1;
	float4 lPos : TEXCOORD1;
	
};

[maxvertexcount(3)]
 void GS_main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> triStream)
{	 
	 GS_OUT output;

	 uint i;

		for (i = 0; i < 3; i++)
		{
			float3 worldPosition = mul(float4(input[i].Pos, 1.0f), worldMatrix[input[i].InstanceId]).xyz;
			output.Random = float3(0.0f, 0.0f, 0.0f);
			output.Random.y = textureFlag[input[i].InstanceId].y;
			output.Random.z = textureFlag[input[i].InstanceId].z;
			//output.WPos = worldPosition;

			output.Pos = mul(float4(input[i].Pos.xyz, 1.0f), worldMatrix[input[i].InstanceId]);

			output.lPos = mul(float4(output.Pos.xyz, 1.0f), lightViewProj);

			output.Pos = mul(float4(output.Pos.xyz, 1.0f), matrixView);
			output.Pos = mul(float4(output.Pos.xyz, 1.0f), matrixProjection);

			output.Norm = mul(float4(input[i].Norm, 1.0f), matrixWorld);

			output.Tex = input[i].Tex;

			output.ViewPos = cameraPos.xyz - worldPosition.xyz;

			triStream.Append(output);
		}

};