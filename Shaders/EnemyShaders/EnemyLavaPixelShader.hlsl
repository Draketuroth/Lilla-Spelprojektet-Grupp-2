SamplerState texSampler: register(s0);
SamplerState shadowSampler : register(s1);
Texture2D tex0 : register(t0);
Texture2D shadowMap : register(t1);

struct PS_IN
{
	float3 Norm: NORMAL;
	float2 Tex : TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 WPos : WPOSITION;
	float3 ViewPos : POSITION1;
	float4 lPos : TEXCOORD1;
};


float4 PS_main(PS_IN input) : SV_Target
{
	float3 texColor;
	float4 color;
	//Lpos divided by W
	input.lPos.xyz /= input.lPos.w;

	//LPos in [0, 1]
	float2 smTexture = float2(0.5f * input.lPos.x + 0.5f, -0.5f * input.lPos.y + 0.5f);


	float depthBias = 0.0017f;

	float depth = input.lPos.z;
	float dx = 1.0f / 1920;
	float dy = 1.0f / 1080;


	texColor = tex0.Sample(texSampler, input.Tex).xyz;

	float s0 = (shadowMap.Sample(shadowSampler, smTexture).r + depthBias < depth) ? 0.25f : 1.0f;
	float s1 = (shadowMap.Sample(shadowSampler, smTexture + float2(dx, 0.0f)).r + depthBias < depth) ? 0.25f : 1.0f;
	float s2 = (shadowMap.Sample(shadowSampler, smTexture + float2(0.0f, dy)).r + depthBias < depth) ? 0.25f : 1.0f;
	float s3 = (shadowMap.Sample(shadowSampler, smTexture + float2(dx, dy)).r + depthBias < depth) ? 0.25f : 1.0f;

	float2 texelPos = smTexture * (dx * dy);

	float2 lerps = frac(texelPos);

	float shadowFactor = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);


	color = float4(texColor, 1.0f) * shadowFactor;

	return color;
}