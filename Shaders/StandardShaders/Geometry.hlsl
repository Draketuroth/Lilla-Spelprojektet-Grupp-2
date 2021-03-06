//----------------------------------------------------------------------------------------------------------------------------------//
// Geometry Shader DirectX11
//
// Fredrik Linde
//----------------------------------------------------------------------------------------------------------------------------------//

// The registers are underlying hardware registers on the GPU where all data is stored during execution of the shaders
// There are different types of register for different types of data
// - Buffers uses "b"

cbuffer GS_CONSTANT_BUFFER : register(b0) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix fortressWorldMatrix;
	matrix lightViewProj;
	float4 cameraPos;
};

cbuffer PLAYER_TRANSFORM : register(b1) {

	matrix matrixW;
	matrix matrixWVP;
}

struct GS_IN
{
	float3 Pos : POSITION;
	float2 Tex: TEXCOORD;
	float3 Norm : NORMAL;
	float4 lPos : TEXCOORD1;

};

struct GS_OUT
{
	float4 Norm: NORMAL;
	float2 Tex: TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 WPos : WPOSITION;
	float3 ViewPos : CAMERAPOS;
	float4 lPos : TEXCOORD1;

};

// The geometry shader takes an entire primitive as an array as its input, but it also require an upper limit for the amount of 
// vertices to output. Therefore the program runs for every primitive. Because we are going to output two triangles, the total 
// vertex count must be a total of 6

[maxvertexcount(6)]
void GS_main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> triStream)
{
	GS_OUT output;

	// UINT is an unsigned INT. The range is 0 through 4294967295 decimals
	uint i;

	for (i = 0; i < 3; i++)
	{
		// To store and calculate the World position for output to the pixel shader, the input position must be multiplied with the World matrix
		float3 worldPosition = mul(float4(input[i].Pos, 1.0f), matrixW).xyz;
		output.WPos = worldPosition;

		//Light coordinate
		output.lPos = mul(input[i].lPos, mul(worldPosition, lightViewProj));

		// To store and calculate the WorldViewProj, the input position must be multiplied with the WorldViewProj matrix

		output.Pos = mul(float4(input[i].Pos.xyz, 1.0f), matrixWVP);

		// For the normal to properly work and to later be used correctly when creating the basic diffuse shading, it's required to be computed in world coordinates

		output.Norm = mul(float4(input[i].Norm, 1.0f), matrixW);

		output.Tex = input[i].Tex;

		output.ViewPos = cameraPos.xyz - worldPosition.xyz;

		triStream.Append(output);	// The output stream can be seen as list which adds the most recent vertex to the last position in that list
	}

};