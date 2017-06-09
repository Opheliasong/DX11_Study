#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: LightShaderClass.h
////////////////////////////////////////////////////////////////////////////////

/////////////
//INCLUDE
/////////////
#include <d3d11.h>
#include <stdio.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "Util.h"
#include "ErrorExportClass.h"

/////////////////////////////////////
//Class Name : LightShaderClass
/////////////////////////////////////
class LightShaderClass : ShaderErrorExportClass
{
private:
	struct MatrixBufferType
	{	
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBufferType
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;
	};
		
public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass& rhs);
	~LightShaderClass();

	bool initilaize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix, ID3D11ShaderResourceView * texture, XMFLOAT3 ligihtDirection, XMFLOAT4 diffuseColor);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_samplerState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
};