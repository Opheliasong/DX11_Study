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

	//렌더링에 사용될 쉐이더의 파라메터 설정
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

	//포인터들과 지역변수들 초기화
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//버텍스 쉐이더 코드 컴파일
	retVal = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);

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
	retVal = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);

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

	// 버퍼로부터 픽셀 쉐이더를 제작한다.
	retVal = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(retVal))
	{
		return false;
	}

	// 버텍스 입력 형태 설명서를 제작한다.
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

	// 폴리곤 구조의 갯수를 계산한다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 버텍스 입력 구조를 생성한다.
	retVal = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);

	if (FAILED(retVal))
	{
		return false;
	}
	
	//버텍스 쉐이더 버퍼와 픽셀 쉐이더 버퍼는 이제 더이상 사용하지 않으므로 해제한다.
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
	retVal = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(retVal))
	{
		return false;
	}

	return true;
}

void TextureShaderClass::ShutdownShader()
{
	//sampler state를 해제한다.
	RELEASE(m_sampleState);	

	//matrix 버퍼를 해제한다.
	RELEASE(m_matrixBuffer);

	//layout을 해제한다
	RELEASE(m_layout);

	//픽셀 쉐이더를 해제한다.
	RELEASE(m_pixelShader);

	//버텍스 쉐이더를 해제한다.
	RELEASE(m_vertexShader);

	return;
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT retVal;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

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
	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//인풋 레이아웃을 설정한다.
	deviceContext->IASetInputLayout(m_layout);

	//버텍스 & 픽셀 쉐이더의 삼각형을 그리기 위해 설정한다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//픽셀 쉐이더에 샘플 state를 설정한다.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//삼각형을 그린다.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
