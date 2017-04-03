//----------------------------------------------------------------------------------------------------------------------------------//
// Fragment Shader DirectX11
//
// Philip Velandria, Jonathan Sundberg, Linnea Vajda, Fredrik Linde
//----------------------------------------------------------------------------------------------------------------------------------//

// The registers are underlying hardware registers on the GPU where all data is stored during execution of the shaders
// There are different types of register for different types of data
// - SamplerState uses "s"
// - Texture uses "t"

SamplerState texSampler: register(s0);
Texture2D shaderTexture : register(t0);

struct PS_IN
{
	float4 Pos : SV_POSITION;
	float2 Tex: TEXCOORD;
	float3 Normal : NORMAL;
	float3 WPos : WPOSITION;
	float3 ViewPos : CAMERAPOS;
};

struct PS_OUT {

	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
};


PS_OUT PS_main(PS_IN input)
{
	PS_OUT output;

	// Sample the color from the texture and store it for output to the render target
	output.Color = shaderTexture.Sample(texSampler, input.Tex);

	// Store the normal output to the render target
	output.Normal = float4(input.Normal, 1.0f);

	return output;
};