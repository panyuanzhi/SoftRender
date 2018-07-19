#pragma once

#include "Traits.h"
#include "BoundingVolume.h"
class Camera {
public:
	typedef typename MathTraits::Point4 Point4;
	typedef typename MathTraits::Vector3 Vector3;
	typedef typename MathTraits::Matrix4 Matrix4;

	void SetViewVolume(float right, float left, float top, float bottom, float Near, float Far);
	void SetPosition(float x, float y, float z);
	void SetReferenceFrame(Vector3 r, Vector3 u, Vector3 d);
	void Update();
	const Matrix4 & GetViewMatrix();
	const Matrix4 & GetProjectionMatrix();
	// ”æ∞ÃÂ∞¸Œß≤‚ ‘
	bool OutSide(BoundingSphere bounding);
private:
	float mRight, mLeft, mTop, mBottom, mNear, mFar;
	Point4 mPosition;
	Vector3 mR, mU, mD;

	Matrix4 mViewMatrix;
	Matrix4 mProjectionMatrix;
};