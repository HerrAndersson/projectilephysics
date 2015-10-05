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

	//float4 sm = ShadowMap.Sample(SampleType, input.tex);
	//return float4(pow(sm.x, 7000), pow(sm.y, 7000), pow(sm.z, 7000), 1);


	float4 finalColor = textureColor;









//	// Standard lighting equation
//	float4 vTotalLightDiffuse = float4(0, 0, 0, 1);
//	float3 lightDir = normalize(g_LightPos - In.vPos);  // direction of light
//	vTotalLightDiffuse += g_LightDiffuse * max(0, dot(In.vNormal, lightDir));
//	vTotalLightDiffuse.a = 1.0f;
//
//	// Now, consult the ShadowMap to see if we're in shadow
//	float4 lightingPosition = GetPositionFromLight(In.vPos);// Get our position on the shadow map
//
//	// Get the shadow map depth value for this pixel   
//	float2 ShadowTexC = 0.5 * lightingPosition.xy / lightingPosition.w + float2(0.5, 0.5);
//	ShadowTexC.y = 1.0f - ShadowTexC.y;
//
//	float shadowdepth = tex2D(ShadowMapSampler, ShadowTexC).r;
//
//	// Check our value against the depth value
//	float ourdepth = 1 - (lightingPosition.z / lightingPosition.w);
//
//	// Check the shadowdepth against the depth of this pixel
//	// a fudge factor is added to account for floating-point error
//	if (shadowdepth - 0.03 > ourdepth)
//	{
//		// we're in shadow, cut the light
//		vTotalLightDiffuse = float4(0, 0, 0, 1);
//	}
//
//	Output.RGBColor = tex2D(MeshTextureSampler, In.TextureUV) *
//		(vTotalLightDiffuse + g_LightAmbient);
//
//	return Output;
//
//}





























	//SHADOW MAPEN ÄR KORREKT, SAMPLINGEN ÄR DOCK FÖR LITEN ELLER NÅGOT





	float3 wp = input.worldPos.xyz;
	float3 lightToPixelVec = lightPos - wp;
	float4 lightSpacePos = mul(mul(input.worldPos, lightView), lightProj);

	float howMuchLight = dot(normalize(lightToPixelVec), normalize(input.normal));
	if (howMuchLight > 0.0f)
	{
		float2 smTex;
		smTex.x = 0.5f + (lightSpacePos.x / lightSpacePos.w * 0.5f);
		smTex.y = 0.5f - (lightSpacePos.y / lightSpacePos.w * 0.5f);

		float depth = lightSpacePos.z / lightSpacePos.w;

		float epsilon = 0.0004f;
		float dx = 1.0f / shadowMapSize;

		//Less filtering
		float s0 = ShadowMap.Sample(SampleType, smTex).r;
		float s1 = ShadowMap.Sample(SampleType, smTex + float2(dx, 0.0f)).r;
		float s2 = ShadowMap.Sample(SampleType, smTex + float2(0.0f, dx)).r;
		float s3 = ShadowMap.Sample(SampleType, smTex + float2(dx, dx)).r;

		float2 texelPos = smTex * shadowMapSize;
		float2 lerps = frac(texelPos);
		float shadowCoeff = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);
	
		if (shadowCoeff < depth - epsilon)
		{
			finalColor = saturate(finalColor * shadowCoeff);

			return finalColor;
		}
	}

	////Get local illumination from the "sun" on the whole scene
	//float3 lightDir = normalize(lightPos - float3(512, 0, 512));	
	//float lightIntensity = saturate(dot(input.normal.xyz, lightDir)) + 0.35f;
	//finalColor = saturate(finalColor * lightIntensity);
	//finalColor = saturate(finalColor + ambient);

	return finalColor;

}