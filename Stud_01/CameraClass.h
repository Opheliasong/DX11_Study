#pragma once
/////////////////////////////////////
//Filename : CameraClass.h
/////////////////////////////////////

//////////////////
//Include
//////////////////
#include <DirectXMath.h>
using namespace DirectX;

////////////////////////////////
//Class name : CameraClass
///////////////////////////////

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);

private:
	float m_PositionX, m_PositionY, m_PositionZ;
	float m_RotationX, m_RotationY, m_RotationZ;
	XMMATRIX m_viewMatrix;
};
