SamplerState SampleType : register(s1);

Texture2D ShadowMap : register(t0);
Texture2D shaderTexture[4];

SamplerState sampleStateClamp : register(s0);
SamplerState sampleStateWrap : register(s1);
SamplerComparisonState sampleStateComparison : register(s2);

cbuffer lightBuffer : register(b0)
{
	matrix lightView;
	matrix lightProj;
	float3 lightPos;
	int shadowMapSize;
};

struct VS_OUT
{
	float4 position		: SV_POSITION0;
	float2 tex			: TEXCOORD0;
	float3 normal		: NORMAL;

	float4 worldPos		: TEXCOORD1;
	float3 viewDir		: POSITION;
	float3 colorModifier: COLORMODIFIER;
};

float4 main(VS_OUT input) : SV_TARGET
{
	float4 blendMapColor = shaderTexture[0].Sample(SampleType, input.tex);
	float4 textureColor;
	int repeat = 32;
	textureColor = shaderTexture[1].Sample(SampleType, input.tex * repeat) * blendMapColor.r;
	textureColor += shaderTexture[2].Sample(SampleType, input.tex * repeat) * blendMapColor.g;
	textureColor += shaderTexture[3].Sample(SampleType, input.tex * repeat) * blendMapColor.b;

	float4 finalColor = textureColor;

	//Get local illumination from the "sun" on the whole scene
	float3 lightDir = normalize(lightPos - float3(512, 0, 512));	
	float lightIntensity = saturate(dot(input.normal.xyz, lightDir)) + 0.35f;
	finalColor = saturate(finalColor * lightIntensity);

	return finalColor;

}