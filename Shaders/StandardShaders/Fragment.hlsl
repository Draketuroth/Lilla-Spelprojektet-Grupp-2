//----------------------------------------------------------------------------------------------------------------------------------//
// Fragment Shader DirectX11
//
// Fredrik Linde
//----------------------------------------------------------------------------------------------------------------------------------//

// The registers are underlying hardware registers on the GPU where all data is stored during execution of the shaders
// There are different types of register for different types of data
// - SamplerState uses "s"
// - Texture uses "t"

SamplerState texSampler : register(s0);
SamplerState shadowSampler : register(s1);
//SamplerComparisonState shadowSampler : register(s1);
Texture2D tex0 : register(t0);
Texture2D shadowMap : register(t1);


struct PS_IN
{
	float3 Norm: NORMAL; 
	float2 Tex : TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 WPos : WPOSITION;
	float3 ViewPos : CAMERAPOS;
	float4 lPos : TEXCOORD1;
};
float2 texOffset(int u, int v)
{
	return float2(u * (1.0f / 1920), v * (1.0f / 1080));
}

// The transformed geometry from the geometry shader is now mapped onto the active Render Target, which will be our back buffer
float4 PS_main(PS_IN input) : SV_Target
{
	float3 texColor;
	float4 color;

	//Division by W
	input.lPos.xyz /= input.lPos.w;
	
	////From [-1, 1] to [0, 1];
	float2 smTexture = float2(0.5f * input.lPos.x + 0.5f, -0.5f * input.lPos.y + 0.5f);

	float depth = input.lPos.z;
	float depthBias = 0.0017f;

	float sum = 0;
	float x, y;

	//float shadowFactor = (shadowMap.Sample(shadowSampler, smTexture).r + 0.005f < depth) ? 0.25f : 1.0f;

	//for (y = -1.5; y < 1.5; y++)
	//{
	//	for (x = -1.5; x < 1.5; x++)
	//	{
	//		sum += shadowMap.SampleCmpLevelZero(shadowSampler, input.lPos.xy + texOffset(x, y), depth);
	//	}
	//}
	//float shadowFactor = sum / 16.0f;

	float dx = 1.0f / 1920;
	float dy = 1.0f / 1080;

	float s0 = (shadowMap.Sample(shadowSampler, smTexture).r + depthBias < depth) ? 0.25f : 1.0f;
	float s1 = (shadowMap.Sample(shadowSampler, smTexture + float2(dx, 0.0f)).r + depthBias < depth) ? 0.25f : 1.0f;
	float s2 = (shadowMap.Sample(shadowSampler, smTexture + float2(0.0f, dy)).r + depthBias < depth) ? 0.25f : 1.0f;
	float s3 = (shadowMap.Sample(shadowSampler, smTexture + float2(dx, dy)).r + depthBias < depth) ? 0.25f : 1.0f;


	float2 texelPos = smTexture * (dx * dy);

	float2 lerps = frac(texelPos);

	float shadowFactor = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);

	// Now the Sample state will sample the color output from the texture file so that we can return the correct color
	texColor = tex0.Sample(texSampler, input.Tex).xyz;

	color = float4(texColor, 1.0f);

	return color * shadowFactor;

};