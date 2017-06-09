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

	//렌더링에 사용할 쉐이더의 파라메터를 설정한다.
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

	//포인터들과 지역변수들 초기화
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//버텍스 쉐이더 코드 컴파일
	retVal = D3DCompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	

	if (FAILED(retVal))
	{
		//만약 쉐이더 컴파일이 실패할 경우 에러 메세지를 출력한다.
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

	//픽셀 쉐이더 코드 컴파일
	retVal = D3DCompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);

	if (FAILED(retVal))
	{
		//만약 쉐이더 컴파일이 실패할 경우 에러 메세지를 출력한다.
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

	//버퍼로 부터 버텍스 쉐이더를 제작한다.
	retVal = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);

	if (FAILED(retVal))
	{
		return false;
	}

	// 버퍼로부터 픽셀 쉐이더를 제작한다.
	retVal = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);

	if (FAILED(retVal))
	{
		return false;
	}

	// 버텍스 입력 형태 Description을 제작한다.
	// 버텍스 타입 구조가 ModelClass 와 shaer와 일치해야 한다.
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

	//LightShader에는 Normal 입력값이 하나 더 추가되었다.
	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// 폴리곤 구조의 갯수를 계산한다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 버텍스 입력 구조를 생성한다.
	retVal = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), OUT &m_layout);
	if (FAILED(retVal))
	{
		return false;
	}

	//버텍스 쉐이더 버퍼와 픽셀 쉐이더 버퍼는 이제 더이상 사용하지 않으므로 해제한다.
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

	//텍스처 샘플 state 설명서를 제작한다.
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

	//텍스처 샘플링 state를 생성한다.
	retVal = device->CreateSamplerState(&samplerDesc, &m_samplerState);
	if (FAILED(retVal))
	{
		return false;
	}

	
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC; //해당 리소스가 GPU와 CPU두개가 모두 접근이 가능한 DYNAMIC usage로 설정
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

	//각 메트릭스들을 변환한다.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projMatrix = XMMatrixTranspose(projMatrix);

	//우선 버퍼에 쓰기 위해 lock을 한다.
	retVal = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, OUT &mappedResource);
	if (FAILED(retVal))
	{
		return false;
	}

	//버퍼내부에 접근하기 위해 포인터를 가져온다.
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	
	//버퍼에 변환된 매트릭스들을 입력한다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projMatrix;

	//버퍼를 Unlock 한다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	//마지막으로 갱신된 값을 가진 버퍼를 버텍스 쉐이더에 넘겨준다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	//텍스쳐 리소스를 픽셀 쉐이더에 넘겨준다. (현재는 텍스쳐)
	deviceContext->PSSetShaderResources(0, 1, &texture);


	//광원 정적 버퍼를 쓰기 위해 lock을 한다.
	retVal = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, OUT &mappedResource);
	if (FAILED(retVal))
	{
		return false;
	}

	//버퍼내부에 접근하기 위해 포인터를 가져온다.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	//버퍼에 변환된 매트릭스들을 입력한다.
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = ligihtDirection;
	dataPtr2->padding = 0.0f;

	//버퍼를 Unlock 한다.
	deviceContext->Unmap(m_lightBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);
	return true;
}

void LightShaderClass::
RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{
	//인풋 레이아웃을 설정한다.
	deviceContext->IASetInputLayout(m_layout);

	//버텍스 & 픽셀 쉐이더의 삼각형을 그리기 위해 설정한다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//픽셀 쉐이더에 샘플 state를 설정한다.
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);

	//삼각형을 그린다.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

