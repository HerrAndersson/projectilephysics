SamplerState SampleType : register(s1);
Texture2D shaderTexture[4];

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

	//Get local illumination from the "sun" on the whole scene
	//Moving in circle:
	//X: = originX + sin(angle)*radius;
	//Y: = originY + cos(angle)*radius;

	float3 lightDir = normalize(float3(512, 256, -100) - float3(512, 0, 512));						
	float lightIntensity = saturate(dot(input.normal.xyz, lightDir)) + 0.35;		 // Calculate the amount of light on this pixel.
	float4 outputColor = saturate(textureColor * lightIntensity);					 // Determine the final amount of diffuse color based on the color of the pixel combined with the light intensity.

	return outputColor;
}