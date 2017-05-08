struct PS_IN
{
	float3 Pos : POSITION;
};

float4 PS_Main(PS_IN input) : SV_Target
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}