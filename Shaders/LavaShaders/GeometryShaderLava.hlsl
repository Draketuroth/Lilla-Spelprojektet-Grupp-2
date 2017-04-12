cbuffer GS_CONSTANT_BUFFER : register(b0) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	float4 cameraPos;

};

struct GS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;

};

struct GS_OUT
{
	float3 Pos : POSITION2;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	float3 WPos : POSITION;
	float3 ViewPos : POSITION1;

};


[maxvertexcount(6)]
void GS_main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> triStream)
{
	GS_OUT output;

	float3 offSet = float3(1.0f, 1.0f, 1.0f);

	// Calculate the length of the sides (edges) A and B to use them for calculating the normal (Delta Pos)

	float3 edge1 = input[1].Pos.xyz - input[0].Pos.xyz;
	float3 edge2 = input[2].Pos.xyz - input[0].Pos.xyz;

	// Calculate the normal to determine the direction for the new triangle to be created ( closer to the camera )

	float3 normalAB = normalize(cross(edge1, edge2));

	// UINT is an unsigned INT. The range is 0 through 4294967295 decimals
	uint i;
	for (i = 0; i < 3; i++)
	{
		// To store and calculate the World position for output to the pixel shader, the input position must be multiplied with the World matrix
		float3 worldPosition = mul(float4(input[i].Pos, 1.0f), matrixWorld).xyz;
		output.WPos = worldPosition;

		// To store and calculate the WorldViewProj, the input position must be multiplied with the WorldViewProj matrix

		output.Pos = mul(float4(input[i].Pos.xyz, 1.0f), worldViewProj);

		// For the normal to properly work and to later be used correctly when creating the basic diffuse shading, it's required to be computed in world coordinates

		output.Norm = mul(float4(normalAB, 1.0f), matrixWorld);

		output.Tex = input[i].Tex;

		output.ViewPos = cameraPos - worldPosition;

		triStream.Append(output);	// The output stream can be seen as list which adds the most recent vertex to the last position in that list
	}

};