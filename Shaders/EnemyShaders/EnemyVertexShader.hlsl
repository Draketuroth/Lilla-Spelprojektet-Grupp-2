//----------------------------------------------------------------------------------------------------------------------------------//
// Fragment Shader DirectX11
//
// BTH - Jonathan Sundberg TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

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
	uint InstanceId : SV_InstanceId;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = input.Pos;

	output.Tex = input.Tex;

	output.Norm = input.Norm;

	output.InstanceId = input.InstanceId;

	return output;
}