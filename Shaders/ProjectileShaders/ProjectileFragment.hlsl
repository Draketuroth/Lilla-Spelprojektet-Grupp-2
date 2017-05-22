SamplerState texSampler: register(s0);
Texture2D tex0 : register(t0);

struct PS_IN
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
};


float4 PS_Main(PS_IN input) : SV_Target
{
	float3 texColor = tex0.Sample(texSampler, input.Tex).xyz;

	float4 color = float4(texColor, 1.0f);

	return color;
}