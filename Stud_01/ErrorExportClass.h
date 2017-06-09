#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include<fstream>
#include "Util.h"
using namespace DirectX;
using namespace std;

class ShaderErrorExportClass
{
public:

	ShaderErrorExportClass()
	{
	}

	virtual ~ShaderErrorExportClass()
	{
	}

	virtual void ShaderErrorExportClass::OutputShaderErrorMessage(ID3D10Blob* errMessage, HWND hwnd, WCHAR* shaderFileName)
	{
		char* compileErrors;
		unsigned long long bufferSize, i;
		ofstream fout;

		//���� �޼��� �ؽ�Ʈ ������ �����͸� �����´�.
		compileErrors = (char*)(errMessage->GetBufferPointer());

		//�޼����� ���̸� �����Ѵ�.
		bufferSize = errMessage->GetBufferSize();

		//���� �޼��� log���Ͽ� ���� ���� �غ��Ѵ�.
		fout.open("shader_error.txt");

		//���� �޼����� �ۼ��Ѵ�.
		for (i = 0; i < bufferSize; i++)
		{
			fout << compileErrors[i];
		}

		//���� �ݱ�
		fout.close();

		//���� �޼����� ���� �Ѵ�.
		RELEASE(errMessage);

		//�ش� �����޽��� log������ Ȯ���ϵ��� �������� �˸� �޼����� �˷��ش�.
		MessageBox(hwnd, L"Error compilling shader. Check shader-error.txt for message.", shaderFileName, MB_OK);

		return;
	}
};

