#include "TextureShaderClass.h"

TextureShaderClass::TextureShaderClass()
{
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass &)
{
}


TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool retVal;

	//Initialize the vertex and pixel shaders.
	retVal = InitializeShader(device, hwnd, L"texture.vs", L"texture.ps");

	return retVal;
}

void TextureShaderClass::Shutdown()
{
	//Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool retVal;

	//�������� ���� ���̴��� �Ķ���� ����
	retVal = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	
	if (retVal)
	{
		RenderShader(deviceContext, indexCount);
	}

	return retVal;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT retVal;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	//�����͵�� ���������� �ʱ�ȭ
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//���ؽ� ���̴� �ڵ� ������
	retVal = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);

	if (FAILED(retVal))
	{
		//���� ���̴� �������� ������ ��� ���� �޼����� ����Ѵ�.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Vertex shader File", MB_OK);
		}
		return false;
	}

	//�ȼ� ���̴� �ڵ� ������
	retVal = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);

	if (FAILED(retVal))
	{
		//���� ���̴� �������� ������ ��� ���� �޼����� ����Ѵ�.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Pixel shader File", MB_OK);
		}
		return false;
	}

	//���۷� ���� ���ؽ� ���̴��� �����Ѵ�.
	retVal = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);

	// ���۷κ��� �ȼ� ���̴��� �����Ѵ�.
	retVal = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(retVal))
	{
		return false;
	}

	// ���ؽ� �Է� ���� ������ �����Ѵ�.
	// ���ؽ� Ÿ�� ������ ModelClass �� shaer�� ��ġ�ؾ� �Ѵ�.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// ������ ������ ������ ����Ѵ�.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���ؽ� �Է� ������ �����Ѵ�.
	retVal = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);

	if (FAILED(retVal))
	{
		return false;
	}
	
	//���ؽ� ���̴� ���ۿ� �ȼ� ���̴� ���۴� ���� ���̻� ������� �����Ƿ� �����Ѵ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	retVal = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(retVal))
	{
		return false;
	}

	//�ؽ�ó ���� state ������ �����Ѵ�.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//�ؽ�ó ���ø� state�� �����Ѵ�.
	retVal = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(retVal))
	{
		return false;
	}

	return true;
}

void TextureShaderClass::ShutdownShader()
{
	//sampler state�� �����Ѵ�.
	RELEASE(m_sampleState);	

	//matrix ���۸� �����Ѵ�.
	RELEASE(m_matrixBuffer);

	//layout�� �����Ѵ�
	RELEASE(m_layout);

	//�ȼ� ���̴��� �����Ѵ�.
	RELEASE(m_pixelShader);

	//���ؽ� ���̴��� �����Ѵ�.
	RELEASE(m_vertexShader);

	return;
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT retVal;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//�� ��Ʈ�������� ��ȯ�Ѵ�.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projMatrix = XMMatrixTranspose(projMatrix);

	//�켱 ���ۿ� ���� ���� lock�� �Ѵ�.
	retVal = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, OUT &mappedResource);
	if (FAILED(retVal))
	{
		return false;
	}

	//���۳��ο� �����ϱ� ���� �����͸� �����´�.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//���ۿ� ��ȯ�� ��Ʈ�������� �Է��Ѵ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projMatrix;

	//���۸� Unlock �Ѵ�.
	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	//���������� ���ŵ� ���� ���� ���۸� ���ؽ� ���̴��� �Ѱ��ش�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	//�ؽ��� ���ҽ��� �ȼ� ���̴��� �Ѱ��ش�. (����� �ؽ���)
	deviceContext->PSSetShaderResources(0, 1, &texture);
	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//��ǲ ���̾ƿ��� �����Ѵ�.
	deviceContext->IASetInputLayout(m_layout);

	//���ؽ� & �ȼ� ���̴��� �ﰢ���� �׸��� ���� �����Ѵ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//�ȼ� ���̴��� ���� state�� �����Ѵ�.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//�ﰢ���� �׸���.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
