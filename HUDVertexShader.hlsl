struct VS_IN
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD;

};

struct VS_OUT
{
	float2 Pos : POSITION;
	float2 Tex : TEXCOORD;

};





VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = input.Pos;
	output.Tex = input.Tex;
	return output;
}