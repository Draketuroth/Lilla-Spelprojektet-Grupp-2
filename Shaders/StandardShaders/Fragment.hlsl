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

	float shadowCheck = (shadowMap.Sample(shadowSampler, smTexture).r + 0.0001f < depth) ? 0.25f : 1.0f;

	texColor = tex0.Sample(texSampler, input.Tex).xyz;

	color = float4(texColor, 1.0f);

	return color;// *shadowCheck;

};