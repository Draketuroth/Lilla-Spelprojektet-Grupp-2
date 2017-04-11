struct PS_IN
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};
float4 PS_main() : SV_TARGET
{

	return float4(0.6f, 0.4f, 0.2f, 1.0f);
}