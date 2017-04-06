
SamplerState PointSampler : register(s0);

cbuffer LightBuffer {

	float3 lightDirection;
	float padding;
};

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);

struct PS_IN {

	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 PS_main(PS_IN input) : SV_TARGET
{

	float4 colors;
	float4 normals;
	float3 lightDir;
	float lightIntensity;
	float4 outputColor;

	// Start by retreiving the color data and normals for this pixel using a point sampler

	// Sample the colors from the color texture
	colors = colorTexture.Sample(PointSampler, input.tex);

	normals = normalTexture.Sample(PointSampler, input.tex);

	// Invert the light direction for calculations
	lightDir = -lightDirection;

	// Calculate the amount of light on the current pixel
	lightIntensity = saturate(dot(normals.xyz, lightDir));

	// Determine the final amount of diffuse color based on the pixel color combined with the light intensity
	outputColor = saturate(colors * lightIntensity);

	return outputColor;

}