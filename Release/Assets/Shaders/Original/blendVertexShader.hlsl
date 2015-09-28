cbuffer matrixBufferPerBlendObject : register(cb0)
{
	matrix worldMatrix;
	float4 weight;
	float3 colorModifier;
};

cbuffer matrixBufferPerFrame : register(cb1)
{
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 camPos;
};

struct vertexInputType
{
	float4 position0	: POSITION0;
	float2 tex			: TEXCOORD0;
	float3 normal0		: NORMAL0;
	float3 position1	: POSITION1;
	float3 position2	: POSITION2;
	float3 position3	: POSITION3;
	float3 normal1		: NORMAL1;
	float3 normal2		: NORMAL2;
	float3 normal3		: NORMAL3;
};

struct vertexOutput
{
	float4 position		: SV_POSITION0;
	float2 tex			: TEXCOORD0;
	float3 normal		: NORMAL;

	float3 worldPos		: TEXCOORD1;
	float3 viewDir		: POSITION;
	float3 colorModifier: COLORMODIFIER;
};

vertexOutput blendVertexShader(vertexInputType input)
{
	vertexOutput output;

	input.position0 *= weight.x;
	input.normal0 *= weight.x;
	input.position1 *= weight.y;
	input.normal1 *= weight.y;
	input.position2 *= weight.z;
	input.normal2 *= weight.z;
	input.position3 *= weight.w;
	input.normal3 *= weight.w;

	input.position0.xyz += input.position1 + input.position2 + input.position3;
	input.normal0 += input.normal1 + input.normal2 + input.normal3;

	input.position0.w = 1.0f;


	output.position = mul(input.position0, worldMatrix);

	output.worldPos = output.position;
	output.viewDir = camPos.xyz - output.worldPos.xyz;
	output.viewDir = normalize(output.viewDir);

	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;
	output.normal = mul(input.normal0, worldMatrix);
	output.normal = normalize(output.normal);

	output.colorModifier = colorModifier;

	return output;
}