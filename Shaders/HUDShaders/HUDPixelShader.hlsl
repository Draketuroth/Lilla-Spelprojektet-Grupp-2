struct PS_IN
{

	float4 Pos : SV_POSITION;
	float2 Tex: TEXCOORD;
	

};

float4 PS_main(PS_IN input) : SV_Target
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}