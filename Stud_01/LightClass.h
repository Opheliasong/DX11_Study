#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: LightClass.h
////////////////////////////////////////////////////////////////////////////////

/////////////
//INCLUDE
/////////////
#include <DirectXMath.h>
using namespace DirectX;

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass& rhs);
	~LightClass();

	void SetDiffuseColor(float r, float g, float b, float a);
	void SetDirection(float x, float y, float z);

	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetDirection();

private:
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction;
};

