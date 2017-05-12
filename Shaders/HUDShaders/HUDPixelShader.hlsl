
SamplerState texSampler: register(s0);
Texture2D tex0 : register(t0);



struct PS_IN
{

	float4 Pos : SV_POSITION;
	float2 Tex: TEXCOORD;
	

};

float4 PS_main(PS_IN input) : SV_Target
{
	float4 texColor;
	float4 color;

	texColor = tex0.Sample(texSampler, input.Tex);
	//texColor.x *= 0;
	//texColor.z *= 0;
	color = float4(texColor);

	return color;
}