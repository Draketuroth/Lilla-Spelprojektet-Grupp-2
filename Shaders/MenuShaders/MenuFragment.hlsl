struct PS_IN
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};


float4 PS_main() : SV_TARGET
{
	return float4(0.69f, 0.57f, 0.23f, 1.0f);
}