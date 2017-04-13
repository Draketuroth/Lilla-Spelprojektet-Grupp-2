
cbuffer GS_CONSTANT_BUFFER : register(b0) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	float4 cameraPos;

};

struct VS_IN {

	float4 position : POSITION;
	float2 tex : TEXCOORD;
};

struct VS_OUT {

	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

VS_OUT VS_main(uint id : SV_VertexID) {

	VS_OUT output;

	output.tex = float2((id << 1) & 2, id & 2);
	output.position = float4(output.tex * float2(2, -2) + float2(-1, 1), 0, 1);

	return output;

}