SamplerState texSampler : register(s0);
Texture2D mainMenu : register(t0);

struct PS_IN
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	
};


float4 PS_main(PS_IN input) : SV_TARGET
{
	float3 texColor;

	texColor = mainMenu.Sample(texSampler, input.Tex).xyz;
	float4 color = float4(texColor, 1.0f);

	

	return color;
}