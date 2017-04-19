//----------------------------------------------------------------------------------------------------------------------------------//
// Terrain Fragment Shader DirectX11
//
// Linnea Vajda TA15
//----------------------------------------------------------------------------------------------------------------------------------//


struct PS_IN
{
	float4 Norm: NORMAL;
	float2 Tex: TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 WPos : WPOSITION;
	float3 ViewPos : POSITION1;

};


// The transformed geometry from the geometry shader is now mapped onto the active Render Target, which will be our back buffer
float4 PS_main(PS_IN input) : SV_Target
{

	//float4 color = float4(1.0f, 0.0f, 0.0f, 1.0f);

	return input.Norm;
};

