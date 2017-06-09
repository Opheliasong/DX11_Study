#include "ModelClass.h"



ModelClass::ModelClass()
{
	m_vertexCount = 0;
	m_indexCount = 0;
}

ModelClass::ModelClass(const ModelClass &)
{
}


ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename)
{
	bool retVal;

	//버텍스와 인덱스 버퍼를 초기화 한다.
	retVal = InitializeBuffers(device);
	if (!retVal)
	{
		return false;
	}

	//모델을 위한 텍스쳐를 로드한다.
	retVal = LoadTexture(device, deviceContext, textureFilename);
	if (!retVal)
	{
		return false;
	}
	return true;
}

void ModelClass::Shutdown()
{
	//모델의 텍스처를 해제한다.
	ReleaseTexture();

	//버텍스와 인덱스 버퍼를 해제한다.
	ShutdownBuffers();
	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	//Put the vertex and inex buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView * ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT retval;

	//Set the number of vertices in the vertex array.
	m_vertexCount = 4; 

	//Set the number of indicies in the index array.
	m_indexCount = 6;

	//Crate the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	//Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	//Fill the vertex and index array!!
	//Load the vertex arrray with data.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);	//Bottom left.
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);		//Top middle.
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
	vertices[1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);		//Bottom right
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[2].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[3].position = XMFLOAT3(2.0f, 1.0f, 0.0f);
	vertices[3].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[3].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	//Load the index array with data
	indices[0] = 0;	//Bottom left.
	indices[1] = 1;	//Top middle.
	indices[2] = 2;	//Bottom right
	indices[3] = 1; //Top middle 
	indices[4] = 3; //new Top right.
	indices[5] = 2; //Bottom right


	//정적 버텍스 버퍼의 description을 설정한다.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//버텍스 버퍼를 생성한다.
	retval = device->CreateBuffer(&vertexBufferDesc, &vertexData, OUT &m_vertexBuffer);
	if(FAILED(retval))
	{
		return false;
	}

	//Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	retval = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(retval))
	{
		return false;
	}

	//Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	//Release the index buffer
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	//Release the vertex buffer
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	//Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive that should be rendered from this vertex buffer.
	//in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device * device, ID3D11DeviceContext * deviceContext, char * fileName)
{
	bool retVal;

	//텍스처 오브젝트 생성
	m_Texture = new TextureClass;

	//텍스처 오브젝트 초기화
	retVal = m_Texture->Initialize(device, deviceContext, fileName);
	if (!retVal)
	{
		return false;
	}
	
	return true;
}

void ModelClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
	return;
}
