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

	//targa 이미지 파일을 메모리로 load한다.
	retVal = LoadTarga(filename, height, width);
	if (!retVal)
	{
		return false;
	}

	//텍스쳐 내용을 설정한다.
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

	//빈 텍스쳐를 생성한다.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	rowPitch = (width * 4) * sizeof(unsigned char);

	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	//쉐이더 리소스 뷰 내용을 설정한다.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	//쉐이더 리소스 뷰의 텍스쳐를 생성한다.
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// 텍스쳐의 밉맵을 생성한다.
	deviceContext->GenerateMips(m_textureView);

	// 텍스처를 불러오기 위해 사용된 targa image data를 제거한다.
	delete[] m_targaData;
	m_targaData = 0;

	return true;
}

void TextureClass::Shutdown()
{
	//텍스쳐 뷰 리소스를 해제
	RELEASE(m_textureView);

	//텍스쳐 해제
	RELEASE(m_texture);

	//targa 정보를 삭제
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

	//Binary로 targa파일을 연다.
	error = fopen_s(OUT &filePtr, filename, "rb");

	//파일 해더를 읽어온다.
	count = (unsigned int)fread(&targaFileHeader, sizeof(targaFileHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	//헤더에서 중요정보(width, height, bit per pixel)를 가져온다
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;
	
	//bpp가 32bit인지 확인한다. 이외에는 지금은 허용하지 않는다.
	if (bpp != 32)
	{
		return false;
	}
	
	//32bit image기준으로 size를 계산한다.
	imageSize = width * height * 4;

	//targa image의 데이터를 위해서 메모리 할당을 한다.
	targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	//targa image 데이터를 읽어들인다.
	count = (unsigned int)fread(OUT targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	//file stream을 닫는다
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

	//targa image data를 포멧에 맞춰서 입력한다.
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

	//targa image data를 다 사용하였으니 삭제한다.
	delete[] targaImage;
	targaImage = 0;

	return true;
}
