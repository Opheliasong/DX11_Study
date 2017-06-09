#include "LightClass.h"



LightClass::LightClass()
{
}

LightClass::LightClass(const LightClass & rhs)
{
}


LightClass::~LightClass()
{
}

void LightClass::SetDiffuseColor(float r, float g, float b, float a)
{
	m_diffuseColor = XMFLOAT4(r, g, b, a);
	return;
}

void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = XMFLOAT3(x, y, z);
	return;
}

XMFLOAT4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}

XMFLOAT3 LightClass::GetDirection()
{
	return m_direction;
}
