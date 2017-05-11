
SamplerState PointSampler : register(s0);

cbuffer LightBuffer : register(b0) {

	float4 Position[3];
	float4 Color[3];
};

cbuffer GS_CONSTANT_BUFFER : register(b1) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix fortressWorldMatrix;
	matrix lightViewProj;
	float4 cameraPos;
};

Texture2D gAlbedoSpecTexture : register(t0);
Texture2D gNormalTexture : register(t1);
Texture2D gPositionTexture : register(t2);
Texture2D depthTexture : register(t3);

struct PS_IN {

	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 PS_main(PS_IN input) : SV_TARGET
{

	// Retrieve data from G-buffer
	float3 FragPos = gPositionTexture.Sample(PointSampler, input.tex).xyz;
	float3 Normal = gNormalTexture.Sample(PointSampler, input.tex).xyz;
	float3 Albedo = gAlbedoSpecTexture.Sample(PointSampler, input.tex).xyz;
	float Specular = gAlbedoSpecTexture.Sample(PointSampler, input.tex).w;

	// Initialize the lightning
	float3 lightning = Albedo * 0.1;

	// Get the direction to the viewer
	float3 viewDir = normalize(cameraPos.xyz - FragPos).xyz;

	for (int i = 0; i < 3; i++) {

		// Calculate the distance between the light and the fragment position
		float lightDistance = length(Position[i].xyz - FragPos);

		// If the distance is smaller than the light radius, this light should contribute to the final color
		if (lightDistance < Position[i].w) {

			// Diffuse
			float3 lightDir = normalize(Position[i].xyz - FragPos);
			float3 diffuse = max(dot(Normal, lightDir), 0.0f) * Albedo * Color[i].xyz;

			// Specular
			float3 halfwayDir = normalize(lightDir + viewDir);
			float spec = pow(max(dot(Normal, halfwayDir), 0.0f), 16.0f);
			float3 specular = Color[i].xyz * spec * Specular;

			// Attenuation
			float distance = length(Position[i].xyz - FragPos);
			float attenuation = 0.1f;
			diffuse *= attenuation;
			specular *= attenuation;

			lightning += diffuse + specular;

		}

	}

		return float4(lightning, 1.0f);

	}