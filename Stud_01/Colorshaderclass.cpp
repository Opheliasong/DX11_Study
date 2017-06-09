#include "Colorshaderclass.h"

ColorShaderClass::ColorShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_matrixBuffer = 0;
	m_layout = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass &)
{
}

ColorShaderClass::~ColorShaderClass()
{
}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool retVal;

	//Initialize the vertex and pixel shaders.
	retVal = InitializeShader(device, hwnd, L"color.vs", L"color.ps");
	if (!retVal)
	{
		return false;
	}
	return true;
}

void ColorShaderClass::Shutdown()
{
	//Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool retVal;

	//Set the shader parameters that it will use for rendering
	retVal = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!retVal)
	{
		return false;
	}

	//Now render the prepared buffers with the shader
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{
	HRESULT retVal;
	ID3D10Blob* errMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	errMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//compile the vertex shader code.
	retVal = D3DCompileFromFile(vsFileName, NULL, NULL, 
								"ColorVertexShader", "vs_5_0", 
								D3D10_SHADER_ENABLE_STRICTNESS, 
								0, &vertexShaderBuffer, &errMessage);

	if (FAILED(retVal))
	{
		if (errMessage)
		{
		//If the shader failed to compile t should have writen something to the error message.
			OutputShaderErrorMessage(errMessage, hwnd, vsFileName);
		}
		else
		{
		//If there was nothing in the error message then it simply could not find the shader file itself.
			MessageBox(hwnd, vsFileName, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	// Compile the pixel shader code.
	retVal = D3DCompileFromFile(psFileName, NULL, NULL, 
								"ColorPixelShader", "ps_5_0", 
								D3D10_SHADER_ENABLE_STRICTNESS, 
								0, &pixelShaderBuffer, &errMessage);
	if (FAILED(retVal))
	{
		if (errMessage)
		{
		//If the shader failed to compile it should have writen something to the error message.
			OutputShaderErrorMessage(errMessage, hwnd, psFileName);
		}
		else
		{
		//If there was nothing in the error message then it simply could not find the file itself.
			MessageBox(hwnd, psFileName, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	//버퍼로부터 버텍스 쉐이더를 제작한다.
	retVal = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
										vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(retVal))
	{
		return false;
	}

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

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
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

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	//Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	//Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	//Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	//Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
	return;
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix)
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
	retVal = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0,OUT &mappedResource);
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

	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//인풋 레이아웃을 설정한다.
	deviceContext->IASetInputLayout(m_layout);

	//Set the vertex and pixel shader that will be used to render this triangle.
	//버텍스 & 픽셀 쉐이더의 삼각형을 그리기 위해 사용한
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
