SamplerState SampleType : register(s1);

Texture2D shaderTexture[4] : register(t1);
Texture2D ShadowMap : register(t0);

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
	float4 ambient = float4(0.05f, 0.05f, 0.05f, 0.0f);
	int repeat = 32;

	float4 textureColor = shaderTexture[1].Sample(SampleType, input.tex * repeat) * blendMapColor.r;
	textureColor += shaderTexture[2].Sample(SampleType, input.tex * repeat) * blendMapColor.g;
	textureColor += shaderTexture[3].Sample(SampleType, input.tex * repeat) * blendMapColor.b;

	//return ShadowMap.Sample(SampleType, input.tex);
	float4 finalColor = textureColor;


	////Calculate light to pixel vector 
	//float3 wp = input.worldPos.xyz;
	///*float3 lightToPixelVec = lightPos - wp;*/ //original
	//float3 lightToPixelVec = wp - lightPos;
	//lightToPixelVec = normalize(lightToPixelVec);

	////Sample and add shadows for the shadow map.
	//float4 lightSpacePos = mul(input.worldPos, lightView);
	//lightSpacePos = mul(lightSpacePos, lightProj);

	//float howMuchLight = dot(lightToPixelVec, input.normal);
	//if (howMuchLight > 0.0f)
	//{
	//	float2 smTex;
	//	smTex.x = 0.5f + (lightSpacePos.x / lightSpacePos.w * 0.5f);
	//	smTex.y = 0.5f - (lightSpacePos.y / lightSpacePos.w * 0.5f);

	//	float depth = lightSpacePos.z / lightSpacePos.w;

	//	float epsilon = 0.0004f;
	//	float dx = 1.0f / shadowMapSize;

	//	//Less filtering
	//	float s0 = ShadowMap.Sample(sampleStateClamp, smTex).r;
	//	float s1 = ShadowMap.Sample(sampleStateClamp, smTex + float2(dx, 0.0f)).r;
	//	float s2 = ShadowMap.Sample(sampleStateClamp, smTex + float2(0.0f, dx)).r;
	//	float s3 = ShadowMap.Sample(sampleStateClamp, smTex + float2(dx, dx)).r;

	//	float2 texelPos = smTex * shadowMapSize;
	//	float2 lerps = frac(texelPos);
	//	float shadowCoeff = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);

	//	if (shadowCoeff < depth - epsilon)
	//	{
	//		finalColor = saturate(finalColor * shadowCoeff);
	//	}
	//}

	//Get local illumination from the "sun" on the whole scene
	float3 lightDir = normalize(lightPos - float3(512, 0, 512));	
	float lightIntensity = saturate(dot(input.normal.xyz, lightDir)) + 0.35f;
	finalColor = saturate(finalColor * lightIntensity);
	finalColor = saturate(finalColor + ambient);

	return finalColor;

}