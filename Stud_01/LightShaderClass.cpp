#include "LightShaderClass.h"



LightShaderClass::LightShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_samplerState = 0;
	m_matrixBuffer = 0;
}

LightShaderClass::LightShaderClass(const LightShaderClass & rhs)
{
}


LightShaderClass::~LightShaderClass()
{
}

bool LightShaderClass::initilaize(ID3D11Device* device, HWND hwnd)
{
	bool retVal;

	retVal = InitializeShader(device, hwnd, L"light.vs", L"light.ps");
	
	return retVal;
}

void LightShaderClass::Shutdown()
{
	ShutdownShader();
	return;
}

bool LightShaderClass::Render(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView * texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	bool retVal;

	//�������� ����� ���̴��� �Ķ���͸� �����Ѵ�.
	retVal = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor);
	if (!retVal)
	{
		return retVal;
	}

	RenderShader(deviceContext, indexCount);
	return retVal;
}

bool LightShaderClass::InitializeShader(ID3D11Device * device, HWND hwnd, WCHAR * vsFilename, WCHAR* psFilename)
{
	HRESULT retVal;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	//�����͵�� ���������� �ʱ�ȭ
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//���ؽ� ���̴� �ڵ� ������
	retVal = D3DCompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	

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
	retVal = D3DCompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);

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

	if (FAILED(retVal))
	{
		return false;
	}

	// ���۷κ��� �ȼ� ���̴��� �����Ѵ�.
	retVal = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);

	if (FAILED(retVal))
	{
		return false;
	}

	// ���ؽ� �Է� ���� Description�� �����Ѵ�.
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

	//LightShader���� Normal �Է°��� �ϳ� �� �߰��Ǿ���.
	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// ������ ������ ������ ����Ѵ�.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���ؽ� �Է� ������ �����Ѵ�.
	retVal = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), OUT &m_layout);
	if (FAILED(retVal))
	{
		return false;
	}

	//���ؽ� ���̴� ���ۿ� �ȼ� ���̴� ���۴� ���� ���̻� ������� �����Ƿ� �����Ѵ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

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
	retVal = device->CreateSamplerState(&samplerDesc, &m_samplerState);
	if (FAILED(retVal))
	{
		return false;
	}

	
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC; //�ش� ���ҽ��� GPU�� CPU�ΰ��� ��� ������ ������ DYNAMIC usage�� ����
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	retVal = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(retVal))
	{
		return false;
	}

	return true;
}

void LightShaderClass::ShutdownShader()
{
	RELEASE(m_lightBuffer);
	RELEASE(m_matrixBuffer);
	RELEASE(m_samplerState);
	RELEASE(m_layout);
	RELEASE(m_pixelShader);
	RELEASE(m_vertexShader);

	return;
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix, ID3D11ShaderResourceView * texture, XMFLOAT3 ligihtDirection, XMFLOAT4 diffuseColor)
{
	HRESULT retVal;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;

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


	//���� ���� ���۸� ���� ���� lock�� �Ѵ�.
	retVal = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, OUT &mappedResource);
	if (FAILED(retVal))
	{
		return false;
	}

	//���۳��ο� �����ϱ� ���� �����͸� �����´�.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	//���ۿ� ��ȯ�� ��Ʈ�������� �Է��Ѵ�.
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = ligihtDirection;
	dataPtr2->padding = 0.0f;

	//���۸� Unlock �Ѵ�.
	deviceContext->Unmap(m_lightBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);
	return true;
}

void LightShaderClass::
RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{
	//��ǲ ���̾ƿ��� �����Ѵ�.
	deviceContext->IASetInputLayout(m_layout);

	//���ؽ� & �ȼ� ���̴��� �ﰢ���� �׸��� ���� �����Ѵ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//�ȼ� ���̴��� ���� state�� �����Ѵ�.
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);

	//�ﰢ���� �׸���.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

