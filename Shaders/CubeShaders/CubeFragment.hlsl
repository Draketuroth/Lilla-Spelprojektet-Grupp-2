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
	float depthBias = 0.003f;

	float dx = 1.0f/1920;
	float dy = 1.0f/1080;

	float3 lightSource = float3(0.0f, 100.0f, 0.0f);	// Light source in the form of a point light
	float3 lightVector;
	float lightIntensity;
	float3 diffuseLight;
	float3 specularLight;

	float nDotL;
	float3 texColor;
	float4 color;

	float shinyPower = 20.0f;

	float3 Ld = float3(0.6f, 0.6f, 0.6f);	// Ld represents the light source intensity
	float3 Ka = float3(0.2f, 0.2f, 0.2f);		// Ka is the hardcoded ambient light
	float3 Ks = float3(1.0f, 1.0f, 1.0f);	// Ks is the hardcoded specular light
	float3 Kd = float3(1.0f, 1.0f, 1.0f);	// Kd represents the diffuse reflectivity cofficient
	float3 ads;

	float3 n = normalize(input.Norm);	// The n component is self-explanatory, but represents the normal of the surface
	float3 s = normalize(lightSource - input.WPos);	// The s component represents the direction from the surface to light source in world coordinates
	float3 v = normalize(input.ViewPos).xyz;	// The v component represents the viewer position in world coordinates
	float3 r = reflect(-s.xyz, n);	// The r component represent the reflection of the light direction vector with the the normal n

	diffuseLight = Kd * max(dot(s, n), 0.0f);

	specularLight = Ks * pow(max(dot(r, v), 0.0f), shinyPower);

	ads = Ld * (Ka + diffuseLight + specularLight);


	/*float sum = 0;
	float x, y;
	for (y = -1.5; y < 1.5; y++)
	{
		for (x = -1.5; x < 1.5; x++)
		{
			sum += shadowMap.SampleCmpLevelZero(shadowSampler, input.lPos.xy + texOffset(x, y), depth);
		}
	}
	float shadowFactor = sum / 16.0f;*/
	
	
	float s0 = (shadowMap.Sample(shadowSampler, smTexture).r + depthBias < depth) ? 0.25f : 1.0f;
	float s1 = (shadowMap.Sample(shadowSampler, smTexture + float2(dx, 0.0f)).r + depthBias < depth) ? 0.25f : 1.0f;
	float s2 = (shadowMap.Sample(shadowSampler, smTexture + float2(0.0f, dy)).r + depthBias < depth) ? 0.25f : 1.0f;
	float s3 = (shadowMap.Sample(shadowSampler, smTexture + float2(dx, dy)).r + depthBias < depth) ? 0.25f : 1.0f;

	float2 texelPos = smTexture * (dx * dy);

	float2 lerps = frac(texelPos);

	float shadowFactor = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);

//	float shadowFactor = (shadowMap.Sample(shadowSampler, smTexture).r + 0.005f < depth) ? 0.25f : 1.0f;

	// Now the Sample state will sample the color output from the texture file so that we can return the correct color
	texColor = tex0.Sample(texSampler, input.Tex).xyz;

	color = float4(texColor, 1.0f);

	return (float4(ads, 1.0f) * color) * shadowFactor;// * shadowCheck;
};