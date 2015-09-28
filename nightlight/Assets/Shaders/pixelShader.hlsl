Texture2D AssetTexture : register(t0);
Texture2D ShadowMap : register(t1);

SamplerState sampleStateClamp : register(s0);
SamplerState sampleStateWrap : register(s1);
SamplerComparisonState sampleStateComparison : register(s2);

struct pixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;

	float4 worldPos : TEXCOORD1;
	float3 viewDir : POSITION;
	float3 colorModifier : COLORMODIFIER;
};

float4 pixelShader(pixelInputType input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	
	float3 wp = input.worldPos.xyz;
	float3 reflection;
	float4 specular = float4(0.0f, 1.0f, 0.0f, 1.0f);
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	float4 diffuse = AssetTexture.Sample(sampleStateClamp, input.tex);
	float3 finalAmbient = (diffuse * lightAmbientSpot).xyz;

	//calculate light to pixel vector for spotlight
	float3 lightToPixelVec = lightPosSpot - wp;;
	float d = length(lightToPixelVec);
	lightToPixelVec /= d;

	//Sample and add shadows for the shadow map.
	float4 lightSpacePos = input.worldPos;
	lightSpacePos = mul(lightSpacePos, lightView);
	lightSpacePos = mul(lightSpacePos, lightProj);

	float howMuchLight = dot(lightToPixelVec, input.normal);
	if (howMuchLight > 0.0f)
	{
		float2 smTex;
		smTex.x = 0.5f + (lightSpacePos.x / lightSpacePos.w * 0.5f);
		smTex.y = 0.5f - (lightSpacePos.y / lightSpacePos.w * 0.5f);

		float depth = lightSpacePos.z / lightSpacePos.w;

		float epsilon = 0.0004f;
		float dx = 1.0f / shadowMapSize;

		//Less filtering
		float s0 = ShadowMap.Sample(sampleStateClamp, smTex).r;
		float s1 = ShadowMap.Sample(sampleStateClamp, smTex + float2(dx, 0.0f)).r;
		float s2 = ShadowMap.Sample(sampleStateClamp, smTex + float2(0.0f, dx)).r;
		float s3 = ShadowMap.Sample(sampleStateClamp, smTex + float2(dx, dx)).r;

		float2 texelPos = smTex * shadowMapSize;
		float2 lerps = frac(texelPos);
		float shadowCoeff = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);

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

		if (shadowCoeff < depth - epsilon)
		{
			finalColor = saturate(finalColor * shadowCoeff);
		}
		else
		{
			finalColor += (diffuse * lightDiffuseSpot).xyz;																			//Add light to the finalColor of the pixel
			finalColor /= (lightAttSpot[0] + (lightAttSpot[1] * (d* d* d) / 4.5) + (lightAttSpot[2] * (d * d * d * d) / 4.5));		//Calculate Light's Distance Falloff factor
			finalColor *= pow(max(dot(-lightToPixelVec, lightDirSpot), 0.0f), lightConeSpot);										//Calculate falloff from center to edge of pointlight cone
		}
	}

	return float4(finalColor += input.colorModifier, diffuse.a);
}