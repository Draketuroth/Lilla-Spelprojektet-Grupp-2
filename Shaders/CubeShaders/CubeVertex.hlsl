//----------------------------------------------------------------------------------------------------------------------------------//
// Fragment Shader DirectX11
//
// BTH - Fredrik Linde TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

// The vertex shader is now only responsible for passing data and doesn't manipulate it any further

struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	uint InstanceId : SV_InstanceId;
};

struct VS_OUT
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	float4 lPos : TEXCOORD1;
	uint InstanceId : SV_InstanceId;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	
	output.Pos = input.Pos;

	output.lPos = float4(input.Pos, 1.0f);
	
	output.Tex = input.Tex;

	output.Norm = input.Norm;

	output.InstanceId = input.InstanceId;

	return output;
}


