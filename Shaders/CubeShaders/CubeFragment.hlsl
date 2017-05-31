//----------------------------------------------------------------------------------------------------------------------------------//
// Fragment Shader DirectX11
//
// BTH - Fredrik Linde TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

// The registers are underlying hardware registers on the GPU where all data is stored during execution of the shaders
// There are different types of register for different types of data
// - SamplerState uses "s"
// - Texture uses "t"

SamplerState texSampler: register(s0);
SamplerState shadowSampler : register(s1);
//SamplerComparisonState shadowSampler : register(s1);
Texture2D shadowMap : register(t0);
Texture2D platformGrassTexture : register(t1);
Texture2D platformStoneTexture : register(t2);
Texture2D platformStoneCracksTexture : register(t3);

struct PS_IN
{
	float3 Norm: NORMAL;
	float2 Tex : TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 Random : WPOSITION;
	float3 ViewPos : POSITION1;
	float4 lPos : TEXCOORD1;
};
float2 texOffset(int u, int v)
{
	return float2(u * (1.0f / 1920), v * (1.0f / 1080));
}

float4 PS_main(PS_IN input) : SV_Target
{

	//Shadow map stuff
	input.lPos.xyz /= input.lPos.w;

	//From [-1, 1] to [0, 1];
	float2 smTexture = float2(0.5f * input.lPos.x + 0.5f, -0.5f * input.lPos.y + 0.5f);

	float depth = input.lPos.z;
	//depth += 0.01f;
	float depthBias = 0.0017f;

	float dx = 1.0f/1920;
	float dy = 1.0f/1080;

	float3 texGrassColor;
	float3 texStoneColor;
	float3 texCracksColor;

	texGrassColor = platformGrassTexture.Sample(texSampler, input.Tex).xyz;
	texStoneColor = platformStoneTexture.Sample(texSampler, input.Tex).xyz;
	texCracksColor = platformStoneCracksTexture.Sample(texSampler, input.Tex).xyz;

	float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 damageColor;
	float4 red = float4(1.0f, 0.0f, 0.0f, 1.0f);
	
	float s0 = (shadowMap.Sample(shadowSampler, smTexture).r + depthBias < depth) ? 0.25f : 1.0f;
	float s1 = (shadowMap.Sample(shadowSampler, smTexture + float2(dx, 0.0f)).r + depthBias < depth) ? 0.25f : 1.0f;
	float s2 = (shadowMap.Sample(shadowSampler, smTexture + float2(0.0f, dy)).r + depthBias < depth) ? 0.25f : 1.0f;
	float s3 = (shadowMap.Sample(shadowSampler, smTexture + float2(dx, dy)).r + depthBias < depth) ? 0.25f : 1.0f;

	float2 texelPos = smTexture * (dx * dy);

	float2 lerps = frac(texelPos);

	float shadowFactor = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);

	// Now the Sample state will sample the color output from the texture file so that we can return the correct color
	
	[branch]
	if(input.Random.z == 1){

		color = float4(texGrassColor, 1.0f);

	}

	if(input.Random.z == 2){

		color = float4(texStoneColor, 1.0f);
	}

	if (input.Random.z == 3) {

		color = float4(texCracksColor, 1.0f);

	}

	if(input.Random.y == 1){

		return color * shadowFactor;

	}

	else {

		damageColor = lerp(color, red, 0.3);
		return damageColor * shadowFactor;
	}

};