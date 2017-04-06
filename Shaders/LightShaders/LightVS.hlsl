
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

VS_OUT VS_main(VS_IN input) {

	VS_OUT output = (VS_OUT)0;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldViewProj);

	output.tex = input.tex;

	return output;

}