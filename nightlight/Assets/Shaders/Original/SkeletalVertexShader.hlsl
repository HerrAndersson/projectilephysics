cbuffer matrixBufferPerObject : register(cb0)
{
	matrix worldMatrix;
	matrix bone[30];
};

cbuffer matrixBufferPerFrame : register(cb1)
{
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 camPos;
};

struct vertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	int4 Index : INDICES;
	float4 Weight : WEIGHTS;
};

struct vertexOutput
{
	float4 position : SV_POSITION0;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;

	float3 worldPos : TEXCOORD1;
	float3 viewDir : POSITION;
};

vertexOutput skeletalVertexShader(vertexInputType input)
{
	vertexOutput output;

	input.position.w = 1.0f;

	matrix boneTransform = bone[input.Index.x] * input.Weight.x;
	boneTransform += bone[input.Index.y] * input.Weight.y;
	boneTransform += bone[input.Index.z] * input.Weight.z;
	boneTransform += bone[input.Index.w] * input.Weight.w;

	output.position = mul(input.position, boneTransform);

	output.position = mul(output.position, worldMatrix);

	output.worldPos = output.position;
	output.viewDir = camPos.xyz - output.worldPos.xyz;
	output.viewDir = normalize(output.viewDir);

	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;
	output.normal = mul(input.normal, worldMatrix);
	output.normal = normalize(output.normal);

	return output;
}