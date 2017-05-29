//----------------------------------------------------------------------------------------------------------------------------------//
// Fragment Shader DirectX11
//
// BTH - Fredrik Linde TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

// The vertex shader is now only responsible for passing data and doesn't manipulate it any further

cbuffer VS_CONSTANT_BUFFER : register(b0) {

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


struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	uint InstanceId : SV_InstanceId;
};

struct VS_OUT
{
	float3 Norm: NORMAL;
	float2 Tex : TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 Random : WPOSITION;
	float3 ViewPos : POSITION1;
	float4 lPos : TEXCOORD1;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	
	float3 worldPosition = mul(float4(input.Pos, 1.0f), worldMatrix[input.InstanceId]).xyz;
	output.Random = float3(0.0f, 0.0f, 0.0f);
	output.Random.y = textureFlag[input.InstanceId].y;
	output.Random.z = textureFlag[input.InstanceId].z;
	//output.WPos = worldPosition;

	output.Pos = mul(float4(input.Pos.xyz, 1.0f), worldMatrix[input.InstanceId]);

	output.lPos = mul(float4(output.Pos.xyz, 1.0f), lightViewProj);

	output.Pos = mul(float4(output.Pos.xyz, 1.0f), matrixView);
	output.Pos = mul(float4(output.Pos.xyz, 1.0f), matrixProjection);

	output.Norm = mul(float4(input.Norm, 1.0f), matrixWorld).xyz;

	output.Tex = input.Tex;

	output.ViewPos = cameraPos.xyz - worldPosition.xyz;

	return output;
}


