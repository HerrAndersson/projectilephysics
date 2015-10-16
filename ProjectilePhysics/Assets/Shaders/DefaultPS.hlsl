Texture2D ShadowMap : register(t0);
Texture2D AssetTexture : register(t1);

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

struct pixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;

	float4 worldPos : TEXCOORD1;
	float3 viewDir : POSITION;
	float3 colorModifier : COLORMODIFIER;
};

float4 main(pixelInputType input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 finalColor = AssetTexture.Sample(sampleStateClamp, input.tex);

	return finalColor;

	//float3 wp = input.worldPos.xyz;
	//float3 lightToPixelVec = normalize(lightPos - wp);

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

	////Get local illumination from the "sun" on the whole scene
	//float3 lightDir = normalize(lightPos - float3(512, 0, 512));
	//float lightIntensity = saturate(dot(input.normal.xyz, lightDir)) + 0.35f;
	//finalColor = saturate(finalColor * lightIntensity);

	//return finalColor;

}



//More filtering
//float s0 = ShadowMap.Sample(sampleStateClamp, smTex).r;
//float s1 = ShadowMap.Sample(sampleStateClamp, smTex + float2(-dx, -dx)).r;
//float s2 = ShadowMap.Sample(sampleStateClamp, smTex + float2(-dx, 0.0f)).r;
//float s3 = ShadowMap.Sample(sampleStateClamp, smTex + float2(-dx, dx)).r;
//float s4 = ShadowMap.Sample(sampleStateClamp, smTex + float2(0.0f, -dx)).r;
//float s5 = ShadowMap.Sample(sampleStateClamp, smTex + float2(0.0f, dx)).r;
//float s6 = ShadowMap.Sample(sampleStateClamp, smTex + float2(dx, -dx)).r;
//float s7 = ShadowMap.Sample(sampleStateClamp, smTex + float2(dx, 0.0f)).r;
//float s8 = ShadowMap.Sample(sampleStateClamp, smTex + float2(dx, dx)).r;

//float2 texelPos = smTex * shadowMapSize;
//float2 lerps = frac(texelPos);

//float shadowCoeff0 = lerp(lerp(s0, s2, lerps.x), lerp(s1, s4, lerps.x), lerps.y);
//float shadowCoeff1 = lerp(lerp(s0, s7, lerps.x), lerp(s4, s6, lerps.x), lerps.y);
//float shadowCoeff2 = lerp(lerp(s0, s7, lerps.x), lerp(s5, s8, lerps.x), lerps.y);
//float shadowCoeff3 = lerp(lerp(s0, s2, lerps.x), lerp(s3, s5, lerps.x), lerps.y);

//float shadowCoeff = lerp(lerp(shadowCoeff0, shadowCoeff3, lerps.x), lerp(shadowCoeff2, shadowCoeff1, lerps.x), lerps.y);