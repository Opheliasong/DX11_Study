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

		//에러 메세지 텍스트 버퍼의 포인터를 가져온다.
		compileErrors = (char*)(errMessage->GetBufferPointer());

		//메세지의 길이를 측정한다.
		bufferSize = errMessage->GetBufferSize();

		//에러 메세지 log파일에 쓰기 위해 준비한다.
		fout.open("shader_error.txt");

		//에러 메세지를 작성한다.
		for (i = 0; i < bufferSize; i++)
		{
			fout << compileErrors[i];
		}

		//파일 닫기
		fout.close();

		//에러 메세지를 해제 한다.
		RELEASE(errMessage);

		//해당 에러메시지 log파일을 확인하도록 유저에게 알림 메세지로 알려준다.
		MessageBox(hwnd, L"Error compilling shader. Check shader-error.txt for message.", shaderFileName, MB_OK);

		return;
	}
};

