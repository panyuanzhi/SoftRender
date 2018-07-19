#include "Light.h"
Light::Light()
{
}

Light::~Light()
{
}

void Light::SetDirection(float x, float y, float z)
{
	mDirection[0] = x; mDirection[1] = y; mDirection[2] = z;
	mDirection.Normalize();
}

void Light::SetIrradiance(float r, float g, float b)
{
	mIrradiance[0] = r; mIrradiance[1] = g; mIrradiance[2] = b; mIrradiance[3] = 1.0;
}

Light::Vector3 Light::GetDirection()
{
	return mDirection;
}

RGBColor Light::GetIrradiance()
{
	return mIrradiance;
}
