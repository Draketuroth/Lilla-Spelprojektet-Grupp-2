cbuffer GS_CONSTANT_BUFFER : register(b0) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix fortressWorldMatrix;
	float4 cameraPos;

};

struct GS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;

};

struct GS_OUT
{
	float4 Norm: NORMAL;
	float2 Tex: TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 WPos : WPOSITION;
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
};