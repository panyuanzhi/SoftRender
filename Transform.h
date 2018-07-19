#pragma once
#include "Traits.h"
#include "Quaternions.h"
class Transform {
public:
	typedef typename MathTraits::Matrix4 Matrix4;
	typedef typename MathTraits::Matrix3 Matrix3;
	typedef typename MathTraits::Vector4 Vector4;
	typedef typename MathTraits::Vector3 Vector3;

	Transform();
	void SetTranslate(float x, float y, float z);
	void SetRoation(Vector3 src, Vector3 dst);
	void SetScale(float x, float y, float z);
	//先缩放，再旋转，最后平移,即Y=RSX+T
	void Update();
	const Matrix4& GetTransform();
	void SetIdentity();
	Vector3 GetTranslate();
private: 

private:
	Matrix4 mMatrix;

	Matrix4 mRotation;
	Vector3 mTranslate;
	Vector3 mScale;
};