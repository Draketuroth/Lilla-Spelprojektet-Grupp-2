
struct PS_IN
{
	float4 pos : SV_POSITION;
};
float4 PS_main(PS_IN input) : SV_TARGET
{
	return float4(0.984f, 0.823f, 0.3f, 1.0f);
}