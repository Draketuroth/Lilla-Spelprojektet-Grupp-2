cbuffer PROJECTILE_TRANSFORM : register(b0)
{
	matrix worldMatrix;
	matrix worldViewProjection;
};

struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};


VS_OUT VS_Main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(float4(input.Pos.xyz, 1.0f), worldViewProjection);
	output.Tex = input.Tex;
	return output;
}