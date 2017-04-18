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

	uint i;

	float3 sideA = input[1].Pos.xyz - input[0].Pos.xyz;
	float3 sideB = input[2].Pos.xyz - input[0].Pos.xyz;

	float3 normalAB = normalize(cross(sideA, sideB));

	float3 normal, viewVector;

	// Calculate the normal to determine the direction for the new triangle to be created ( closer to the camera )

	float4 position = mul(float4(input[0].Pos, 1.0f), worldViewProj);
	float4 position2 = mul(float4(input[1].Pos, 1.0f), worldViewProj);
	float4 position3 = mul(float4(input[2].Pos, 1.0f), worldViewProj);

	float3 triangleSideA = (position - position2).xyz;
	float3 triangleSideB = (position - position3).xyz;

	normal = normalize(cross(triangleSideA, triangleSideB));

	if (dot(normal.xyz, -position.xyz) > 0.0f) {

		for (i = 0; i < 3; i++)
		{
			float3 worldPosition = mul(float4(input[i].Pos, 1.0f), matrixWorld).xyz;
			output.WPos = worldPosition;

			output.Pos = mul(float4(input[i].Pos.xyz, 1.0f), worldViewProj);

			output.Norm = mul(float4(normalAB, 1.0f), matrixWorld);
			output.Tex = input[i].Tex;

			output.ViewPos = cameraPos.xyz - worldPosition.xyz;

			triStream.Append(output);
		}

	}
};