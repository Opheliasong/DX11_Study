#ifndef __MODELCLASS_H_
#define __MODELCLASS_H_

////////////////////////
// Filename : ModelClass.h
////////////////////////

////////////////////////
// INCLUDES
////////////////////////
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

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
		XMFLOAT4 color;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	//Vertex and index buffer
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

#endif // !__MODELCLASS_H_
