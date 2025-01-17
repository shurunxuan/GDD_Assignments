
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float3 normal		: NORMAL;
};

struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
};

cbuffer externalData : register(b0)
{
	DirectionalLight light0;
	DirectionalLight light1;
};

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	float4 result = float4(0, 0, 0, 0);

	float3 normalizedLightDirection = normalize(light0.Direction);
	float diffuseLightIntensity = dot(input.normal, light0.Direction);
	diffuseLightIntensity = saturate(diffuseLightIntensity);

	float4 diffuse = saturate(diffuseLightIntensity * light0.DiffuseColor);
	result += diffuse + light0.AmbientColor;

	normalizedLightDirection = normalize(light1.Direction);
	diffuseLightIntensity = dot(input.normal, light1.Direction);
	diffuseLightIntensity = saturate(diffuseLightIntensity);

	diffuse = saturate(diffuseLightIntensity * light1.DiffuseColor);
	result += diffuse + light1.AmbientColor;

	result = saturate(result);

	return float4(result);
}