#include "Graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;
	m_LightShader = 0;
	m_Light = 0;
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

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// Create the model object.
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	// Initialize the model object.
	retVal = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "stone01.tga");
	if (!retVal)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// 텍스쳐 쉐이더를 생성한다.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// 텍스쳐 쉐이더 초기화
	retVal = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!retVal)
	{
		MessageBox(hwnd, L"Could not initialize the Texture shader object.", L"Error", MB_OK);
		return false;
	}

	//라이트 쉐이더 오브젝트를 생성한다.
	m_LightShader = new LightShaderClass();
	if (!m_LightShader)
	{
		return false;
	}

	//라이트 쉐이더 오브젝트를 초기화를 한다.
	retVal = m_LightShader->initilaize(m_Direct3D->GetDevice(), hwnd);
	if (!retVal)
	{
		MessageBox(hwnd, L"Could not initialize the Light shader object.", L"Error", MB_OK);
		return false;
	}

	//라이트 오브젝트를 생성한다.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	//라이트 오브젝트 초기화
	m_Light->SetDiffuseColor(0.5f, 1.0f, 0.5f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	//return!
	return true;
}

void GraphicsClass::Shutdown()
{
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}
	SHUTDOWN_AND_DELETE(m_LightShader);

	// Release the model object.
	/*if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}*/
	SHUTDOWN_AND_DELETE(m_Model);

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	
	//Release the Direct3D object
	/*if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}*/
	SHUTDOWN_AND_DELETE(m_Direct3D);

	return;
}

bool GraphicsClass::Frame()
{
	static float rotation = 0.0f;

	rotation += (float)XM_PI * 0.01f;
	if (rotation > 90.0f)
	{
		rotation -= 90.0f;
	}

	bool retVal;

	//Render the graphics scene.
	retVal = Render(rotation);
	if (!retVal)
	{
		return false;
	}
	return true;
}

bool GraphicsClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(1.0f, 1.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	worldMatrix = XMMatrixRotationX(rotation) * worldMatrix;

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// Render the model using the color shader.
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), 
		worldMatrix, viewMatrix, projectionMatrix, 
		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());

	if (!result)
	{
		return false;
	}

	//Present the rendered scene to the screen.
	m_Direct3D->EndScene();


	return true;
}
