Texture2D ShadowMap : register(t0);
Texture2D shaderTexture[4] : register(t1);

SamplerState sampleStateClamp : register(s0);
SamplerState sampleStateWrap : register(s1);
SamplerComparisonState sampleStateComparison : register(s2);
SamplerState sampleStatePoint : register(s3);

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

float2 texOffset(int u, int v)
{
	return float2(u * 1.0f / shadowMapSize, v * 1.0f / shadowMapSize);
}

float4 main(VS_OUT input) : SV_TARGET
{
	float4 blendMapColor = shaderTexture[0].Sample(sampleStateWrap, input.tex);
	float4 ambient = float4(0.05f, 0.05f, 0.05f, 0.0f);
	int repeat = 32;

	float4 textureColor = shaderTexture[1].Sample(sampleStateWrap, input.tex * repeat) * blendMapColor.r;
	textureColor += shaderTexture[2].Sample(sampleStateWrap, input.tex * repeat) * blendMapColor.g;
	textureColor += shaderTexture[3].Sample(sampleStateWrap, input.tex * repeat) * blendMapColor.b;
	float4 finalColor = textureColor;


	//float4 sm = ShadowMap.Sample(sampleStatePoint, input.tex);
	////return sm.xxxx;
	//return float4(pow(sm.x, 800), pow(sm.y, 800), pow(sm.z, 800), 1);
	float4 wp4 = float4(input.worldPos.xyz, 1.0f);
	float4 lightSpacePos = mul(mul(wp4, lightView), lightProj);

	float3 wp = input.worldPos.xyz;
	float3 lightToPixelVec = normalize(wp - lightPos);
	float howMuchLight = dot(lightToPixelVec, input.normal);

	//if (howMuchLight > 0.0f)
	//{
		float2 smTex;
		smTex.x = 0.5f + (lightSpacePos.x / lightSpacePos.w * 0.5f);
		smTex.y = 0.5f - (lightSpacePos.y / lightSpacePos.w * 0.5f);

		float depth = lightSpacePos.z / lightSpacePos.w;

		float epsilon = 0.000001f;
		float dx = 1.0f / shadowMapSize;

		//Less filtering
		float s0 = ShadowMap.Sample(sampleStatePoint, smTex).r;
		float s1 = ShadowMap.Sample(sampleStatePoint, smTex + float2(dx, 0.0f)).r;
		float s2 = ShadowMap.Sample(sampleStatePoint, smTex + float2(0.0f, dx)).r;
		float s3 = ShadowMap.Sample(sampleStatePoint, smTex + float2(dx, dx)).r;
		
		float2 texelPos = smTex * shadowMapSize;
		float2 lerps = frac(texelPos);
		float shadowCoeff = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);

		if (shadowCoeff < depth - epsilon)
		{
			finalColor = finalColor * saturate(((shadowCoeff * 10) - 2.5f) / 10);
		}
	//}


	//Get local illumination from the "sun" on the whole scene
	float3 lightDir = normalize(lightPos - float3(512, 0, 512));	
	float lightIntensity = saturate(dot(input.normal.xyz, lightDir)) + 0.35f;
	finalColor = saturate(finalColor * lightIntensity);
	finalColor = saturate(finalColor + ambient);

	return finalColor;
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



////Less filtering
//float s0 = ShadowMap.Sample(sampleStatePoint, smTex).r;
//float s1 = ShadowMap.Sample(sampleStatePoint, smTex + float2(dx, 0.0f)).r;
//float s2 = ShadowMap.Sample(sampleStatePoint, smTex + float2(0.0f, dx)).r;
//float s3 = ShadowMap.Sample(sampleStatePoint, smTex + float2(dx, dx)).r;
//
//float2 texelPos = smTex * shadowMapSize;
//float2 lerps = frac(texelPos);
//float shadowCoeff = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);



//float4 lpos = mul(mul(input.worldPos, lightView), lightProj);
//lpos.xyz /= lpos.w;
//
////if position is not visible to the light - dont illuminate it. results in hard light frustum
//if (lpos.x < -1.0f || lpos.x > 1.0f ||
//	lpos.y < -1.0f || lpos.y > 1.0f ||
//	lpos.z < 0.0f || lpos.z > 1.0f) return ambient;
//
////transform clip space coords to texture space coords (-1:1 to 0:1)
//lpos.x = lpos.x / 2 + 0.5;
//lpos.y = lpos.y / -2 + 0.5;
//
////apply shadow map bias
//lpos.z -= 0.004f;
//
////PCF sampling for shadow map
//float sum = 0;
//float x, y;
//
////perform PCF filtering on a 4 x 4 texel neighborhood
//for (y = -1.5; y <= 1.5; y += 1.0)
//{
//	for (x = -1.5; x <= 1.5; x += 1.0)
//	{
//		sum += ShadowMap.SampleCmpLevelZero(sampleStateComparison, lpos.xy + texOffset(x, y), lpos.z);
//	}
//}
//
//
//float shadowFactor = sum / 16.0;
//
////calculate ilumination at fragment
//float3 L = normalize(lightPos - input.worldPos.xyz);
//float ndotl = dot(normalize(input.normal), L);
//return ambient + shadowFactor*textureColor*ndotl;
