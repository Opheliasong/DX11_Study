#include "Graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_Direct3D = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass &other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool retVal;

	//Create the Direct3D object;
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	//Initialize the Direct3D object.
	retVal = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!retVal)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}
	//return
	return true;
}

void GraphicsClass::Shutdown()
{
	//Release the Direct3D object
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
	return;
}

bool GraphicsClass::Frame()
{
	bool retVal;

	//Render the graphics scene.
	retVal = Render();
	if (!retVal)
	{
		return false;
	}
	return true;
}

bool GraphicsClass::Render()
{
	// Clear the buffers to begin the scene
	m_Direct3D->BeginScene(0.6f, 0.6f, 0.0f, 1.0f);

	//Present the rendered scene to the screen.
	m_Direct3D->EndScene();


	return true;
}
