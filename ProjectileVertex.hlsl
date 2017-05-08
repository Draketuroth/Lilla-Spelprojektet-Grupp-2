struct VS_IN
{
	float3 Pos : POSITION;
};

struct VS_OUT
{
	float3 Pos : POSITION;
};


VS_OUT VS_Main(VS_IN input) 
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = input.Pos;
	
	return output;
}