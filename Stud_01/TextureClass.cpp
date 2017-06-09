#include "TextureClass.h"



TextureClass::TextureClass()
{
}


TextureClass::~TextureClass()
{
}


bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool retVal;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	//targa �̹��� ������ �޸𸮷� load�Ѵ�.
	retVal = LoadTarga(filename, height, width);
	if (!retVal)
	{
		return false;
	}

	//�ؽ��� ������ �����Ѵ�.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	//�� �ؽ��ĸ� �����Ѵ�.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	rowPitch = (width * 4) * sizeof(unsigned char);

	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	//���̴� ���ҽ� �� ������ �����Ѵ�.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	//���̴� ���ҽ� ���� �ؽ��ĸ� �����Ѵ�.
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// �ؽ����� �Ӹ��� �����Ѵ�.
	deviceContext->GenerateMips(m_textureView);

	// �ؽ�ó�� �ҷ����� ���� ���� targa image data�� �����Ѵ�.
	delete[] m_targaData;
	m_targaData = 0;

	return true;
}

void TextureClass::Shutdown()
{
	//�ؽ��� �� ���ҽ��� ����
	RELEASE(m_textureView);

	//�ؽ��� ����
	RELEASE(m_texture);

	//targa ������ ����
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}

	return;
}

ID3D11ShaderResourceView * TextureClass::GetTexture()
{
	return m_textureView;
}

bool TextureClass::LoadTarga(char * filename, int & height, int & width)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	unsigned char* targaImage;
	TargaHeader targaFileHeader;

	//Binary�� targa������ ����.
	error = fopen_s(OUT &filePtr, filename, "rb");

	//���� �ش��� �о�´�.
	count = (unsigned int)fread(&targaFileHeader, sizeof(targaFileHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	//������� �߿�����(width, height, bit per pixel)�� �����´�
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;
	
	//bpp�� 32bit���� Ȯ���Ѵ�. �̿ܿ��� ������ ������� �ʴ´�.
	if (bpp != 32)
	{
		return false;
	}
	
	//32bit image�������� size�� ����Ѵ�.
	imageSize = width * height * 4;

	//targa image�� �����͸� ���ؼ� �޸� �Ҵ��� �Ѵ�.
	targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	//targa image �����͸� �о���δ�.
	count = (unsigned int)fread(OUT targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	//file stream�� �ݴ´�
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	//Allocate memory for the targe destination data.
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		return false;
	}

	index = 0;

	k = (width * height * 4) - (width * 4);

	//targa image data�� ���信 ���缭 �Է��Ѵ�.
	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2]; //Red
			m_targaData[index + 1] = targaImage[k + 1]; //Green
			m_targaData[index + 2] = targaImage[k + 0];	//Blue
			m_targaData[index + 3] = targaImage[k + 3]; //Alpha

			k += 4;
			index += 4;
		}
		k -= (width * 8);
	}

	//targa image data�� �� ����Ͽ����� �����Ѵ�.
	delete[] targaImage;
	targaImage = 0;

	return true;
}
