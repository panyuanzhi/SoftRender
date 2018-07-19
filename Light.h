#pragma once
#include "Traits.h"
#include "RGBColor.h"
class Light
{
public:
	typedef typename MathTraits::Vector3 Vector3;
public:
	Light();
	~Light();
	void SetDirection(float x, float y, float z);
	void SetIrradiance(float r, float g, float b);
	Vector3 GetDirection();
	RGBColor GetIrradiance();
private:
	Vector3 mDirection;
	//�����նȣ���λ������յķ��书�� W/m^2
	RGBColor mIrradiance;
};

