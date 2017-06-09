////////////////////////////////////////////////////////////////////////////////
// Filename: Light.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;    

	// 위치 벡터의 4번째인자의 값을 행렬계산을 위해 변경한다.
    input.position.w = 1.0f;

	// 버텍스의 위치값을 월드, 뷰, 투영 행렬에 맞춰서 계산을 한다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	//픽셀쉐이더를 위해서 텍스쳐 좌표계의 값을 저장한다.
	output.tex = input.tex;

	//월드 매트릭스를 사용하여 노말벡터를 계산한다.
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	//노말 벡터의 normalize처리.
	output.normal = normalize(output.normal);

	return output;
}