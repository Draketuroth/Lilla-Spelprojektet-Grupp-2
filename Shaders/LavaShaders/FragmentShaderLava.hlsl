//----------------------------------------------------------------------------------------------------------------------------------//
// Terrain Fragment Shader DirectX11
//
// Linnea Vajda TA15
//----------------------------------------------------------------------------------------------------------------------------------//


struct PS_IN
{
	float3 Pos : POSITION2;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	float3 WPos : POSITION;
	float3 ViewPos : POSITION1;

};


// The transformed geometry from the geometry shader is now mapped onto the active Render Target, which will be our back buffer
float4 PS_main(PS_IN input) : SV_Target
{

	float4 color = float4(1.0f, 0.0f, 0.0f, 1.0f);

	return color;
};

