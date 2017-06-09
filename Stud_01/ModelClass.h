#pragma once

////////////////////////
// Filename : ModelClass.h
////////////////////////

////////////////////////
// INCLUDES
////////////////////////
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "Util.h"
#include "TextureClass.h"

///////////////////////
// Class name: ModelClass
///////////////////////

class ModelClass
{
private:
	//define the our vertex type
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fileName);
	void ReleaseTexture();
private:
	//Vertex and index buffer
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	TextureClass* m_Texture;
};