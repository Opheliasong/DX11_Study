#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////

/////////////
//INCLUDE
/////////////
#include <Windows.h>
#include "D3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "Colorshaderclass.h"
#include "TextureShaderClass.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

/////////////////////////////////////
//Class Name : GraphicsClass
/////////////////////////////////////

class GraphicsClass
{
public :
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;	
	TextureShaderClass* m_TextureShader;
};
