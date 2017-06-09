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

	// ��ġ ������ 4��°������ ���� ��İ���� ���� �����Ѵ�.
    input.position.w = 1.0f;

	// ���ؽ��� ��ġ���� ����, ��, ���� ��Ŀ� ���缭 ����� �Ѵ�.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	//�ȼ����̴��� ���ؼ� �ؽ��� ��ǥ���� ���� �����Ѵ�.
	output.tex = input.tex;

	//���� ��Ʈ������ ����Ͽ� �븻���͸� ����Ѵ�.
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	//�븻 ������ normalizeó��.
	output.normal = normalize(output.normal);

	return output;
}