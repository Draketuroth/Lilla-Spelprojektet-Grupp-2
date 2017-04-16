
SamplerState PointSampler : register(s0);

cbuffer LightBuffer : register(b0) {

	float3 lightDirection;
	float padding;
};

cbuffer GS_CONSTANT_BUFFER : register(b1) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	float4 cameraPos;
};

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D worldTexture : register(t2);
Texture2D depthTexture : register(t3);

struct PS_IN {

	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 PS_main(PS_IN input) : SV_TARGET
{

	float4 color;
	float4 worldPos;
	float3 normal;

	float4 position;

	float4 normalData;
	float4 depthValue;

	float specularPower;
	float specularIntensity;

	float3 lightDir;
	float lightIntensity;
	float4 outputColor;

	// Sample the colors from the color texture
	color = colorTexture.Sample(PointSampler, input.tex);

	// Get normal data from the normal map
	normalData = normalTexture.Sample(PointSampler, input.tex);

	// Transform normal back into [-1, 1] range
	normal = 2.0f * normalData.xyz - 1.0f;

	// Get specular power and get it into [0, 255] range
	specularPower = normalData.a * 255;

	// Get specular intensity from the color map
	specularIntensity = colorTexture.Sample(PointSampler, input.tex).a;


	depthValue = depthTexture.Sample(PointSampler, input.tex).r;

	// Compute-screen space position

	position.x = input.tex.x * 2.0f - 1.0f;
	position.y = input.tex.y * 2.0f - 1.0f;
	position.z = depthValue;
	position.w = 1.0f;

	// Transform to world space


	worldPos = worldTexture.Sample(PointSampler, input.tex);

	// Invert the light direction for calculations
	lightDir = -lightDirection;

	// Calculate the amount of light on the current pixel
	lightIntensity = saturate(dot(normal.xyz, lightDir));

	// Determine the final amount of diffuse color based on the pixel color combined with the light intensity
	outputColor = saturate(color * lightIntensity);

	return outputColor;

}