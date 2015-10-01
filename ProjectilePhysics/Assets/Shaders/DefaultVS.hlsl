cbuffer matrixBufferPerObject : register(b0)
{
	matrix worldMatrix;
	float3 colorModifier;
};

cbuffer matrixBufferPerFrame : register(b1)
{
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 camPos;
};

struct vertexInputType
{
	float4 position		: POSITION;
	float2 tex			: TEXCOORD0;
	float3 normal		: NORMAL;
};

struct vertexOutput
{
	float4 position		: SV_POSITION0;
	float2 tex			: TEXCOORD0;
	float3 normal		: NORMAL;

	float4 worldPos		: TEXCOORD1;
	float3 viewDir		: POSITION;
	float3 colorModifier: COLORMODIFIER;
};

vertexOutput main(vertexInputType input)
{
	vertexOutput output;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);

	output.worldPos = output.position;

	output.viewDir = camPos.xyz - output.worldPos.xyz;
	output.viewDir = normalize(output.viewDir);

	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	output.normal = mul(float4(input.normal, 1.0f), worldMatrix).xyz;
	output.normal = normalize(output.normal);

	output.colorModifier = colorModifier;

	return output;
}