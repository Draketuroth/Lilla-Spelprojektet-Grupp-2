
struct PS_IN
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	
};


float4 PS_Main(PS_IN input) : SV_Target
{
	float4 color = {1.0f,0.3f,0.3f,1.0f};

	

	return color;
}