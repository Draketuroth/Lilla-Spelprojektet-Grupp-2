//struct VS_IN
//{
//	float3 Pos : POSITION;
//	float2 Tex : TEXCOORD;
//};
//struct VS_OUT
//{
//	float4 Pos : SV_POSITION;
//	float2 Tex : TEXCOORD;
//};
//
//VS_OUT VS_main(VS_IN input)
//{
//	VS_OUT output = (VS_OUT)0;
//
//	output.Pos = float4(input.Pos, 1.0f);
//	output.Tex = input.Tex;
//
//	return output;
//}
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