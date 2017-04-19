
SamplerState PointSampler : register(s0);

cbuffer LightBuffer : register(b0) {

	float4 Position[3];
	float4 Color[3];
};

cbuffer GS_CONSTANT_BUFFER : register(b1) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix inverseViewProjection;
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
	float4 lPosition[3];
	float4 lColor[3];
	float lRadius[3];

	lPosition[0] = float4(3.0f, 7.0, 9.0f, 1.0f);
	lPosition[1] = float4(14.0f, 40.0, 3.0f, 1.0f);
	lPosition[2] = float4(0.0f, 5.0, -3.0f, 1.0f);

	lColor[0] = float4(3.0f, 0.0, 1.0f, 1.0f);
	lColor[1] = float4(6.0f, 2.0, 14.0f, 1.0f);
	lColor[2] = float4(0.0f, 1.0, 5.0f, 1.0f);

	lRadius[0] = 10.5f;
	lRadius[1] = 20.0f;
	lRadius[2] = 40.0f;

	// Retrieve data from G-buffer
	float3 FragPos = gPositionTexture.Sample(PointSampler, input.tex);
	float3 Normal = gNormalTexture.Sample(PointSampler, input.tex);
	float3 Albedo = gAlbedoSpecTexture.Sample(PointSampler, input.tex);

	// Then calculate lightning as usual
	float3 lightning = Albedo * 0.1;
	float3 viewDir = normalize(cameraPos - FragPos).xyz;

	for (int i = 0; i < 3; i++) {

		float distance = length(lPosition[i] - FragPos);

		if (distance < lRadius[i]){

		float3 lightDir = normalize(lPosition[i] - FragPos);
		float3 reflection = reflect(-lightDir.xyz, Normal);
		float3 specular = pow(max(dot(reflection, viewDir), 0.0f), 8.0f);
		float3 diffuse = max(dot(Normal, lightDir), 0.0f) * Albedo * lColor[i];
		lightning += diffuse + specular;

		}
	}

	return float4(lightning, 1.0f);
}