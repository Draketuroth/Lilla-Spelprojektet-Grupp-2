

struct VS_IN
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
};

struct VS_OUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
};


VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = input.Pos;

	output.Tex = input.Tex;

	output.Normal = input.Normal;

	return output;
}