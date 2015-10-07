SamplerState SampleType;

Texture2D ShadowMap : register(t0);
Texture2D shaderTexture[4] : register(t1);

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
	float4 blendMapColor = shaderTexture[0].Sample(sampleStateWrap, input.tex);
	float4 ambient = float4(0.05f, 0.05f, 0.05f, 0.0f);
	int repeat = 32;

	float4 textureColor = shaderTexture[1].Sample(sampleStateWrap, input.tex * repeat) * blendMapColor.r;
	textureColor += shaderTexture[2].Sample(sampleStateWrap, input.tex * repeat) * blendMapColor.g;
	textureColor += shaderTexture[3].Sample(sampleStateWrap, input.tex * repeat) * blendMapColor.b;
	float4 finalColor = textureColor;





	//float4 sm = ShadowMap.Sample(SampleType, input.tex);
	//return float4(pow(sm.x, 7000), pow(sm.y, 7000), pow(sm.z, 7000), 1);


	//////SHADOW MAPEN ÄR KORREKT, SAMPLINGVÄRDET ÄR FÖR LÅGT

	//float4 wp4 = float4(input.worldPos.xyz, 1.0f);
	//float4 lightSpacePos = mul(mul(wp4, lightView), lightProj);

	//float3 wp = input.worldPos.xyz;
	//float3 lightToPixelVec = normalize(lightPos - wp);
	//float howMuchLight = dot(lightToPixelVec, input.normal);
	//if (howMuchLight > 0.0f)
	//{

	//	float2 smTex;
	//	smTex.x = 0.5f + (lightSpacePos.x / lightSpacePos.w * 0.5f);
	//	smTex.y = 0.5f - (lightSpacePos.y / lightSpacePos.w * 0.5f);

	//	float depth = lightSpacePos.z / lightSpacePos.w;

	//	float epsilon = 0.004f;
	//	float dx = 1.0f / shadowMapSize;

	//	//Less filtering
	//	float s0 = ShadowMap.Sample(SampleType, smTex).r;
	//	float s1 = ShadowMap.Sample(SampleType, smTex + float2(dx, 0.0f)).r;
	//	float s2 = ShadowMap.Sample(SampleType, smTex + float2(0.0f, dx)).r;
	//	float s3 = ShadowMap.Sample(SampleType, smTex + float2(dx, dx)).r;

	//	float2 texelPos = smTex * shadowMapSize;
	//	float2 lerps = frac(texelPos);
	//	float shadowCoeff = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);

	//	shadowCoeff = float(ShadowMap.SampleCmpLevelZero(sampleStateComparison, smTex, depth + epsilon));

	//	//shadowCoeff = pow(shadowCoeff, 7000);
	//	///*depth = pow(depth, 7000);*/
	//	//shadowCoeff = saturate(shadowCoeff);
	//	//depth = saturate(depth);

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