#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: TextureClass.h
////////////////////////////////////////////////////////////////////////////////

/////////////
//INCLUDE
/////////////
#include <d3d11.h>
#include <stdio.h>
#include "Util.h"

/////////////////////////////////////
//Class Name : TextureClass
/////////////////////////////////////
class TextureClass
{
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	TextureClass();
	~TextureClass();
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool LoadTarga(char* filename, int& height, int& width);

private:
	unsigned char* m_targaData;
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;
};
