#include "Transform.h"

Transform::Transform()
{
	mMatrix.SetIdentity();
}

void Transform::SetTranslate(float x, float y, float z)
{
	mTranslate[0] = x; mTranslate[1] = y; mTranslate[2] = z;
}

void Transform::SetRoation(Vector3 src, Vector3 dst)
{
	mRotation = Quaternion::GetRotationMatrix(src, dst);
}

void Transform::SetScale(float x, float y, float z)
{
	mScale[0] = x; mScale[1] = y, mScale[2] = z;
}

void Transform::Update()
{
	//直接构造变换矩阵(纸上推导计算过程)

	//填写缩放 旋转项
	mMatrix[0][0] = mRotation[0][0] * mScale[0]; mMatrix[0][1] = mRotation[0][1] * mScale[1]; mMatrix[0][1] = mRotation[0][2] * mScale[2];
	mMatrix[1][0] = mRotation[1][0] * mScale[0]; mMatrix[1][1] = mRotation[1][1] * mScale[1]; mMatrix[1][1] = mRotation[1][2] * mScale[2];
	mMatrix[2][0] = mRotation[2][0] * mScale[0]; mMatrix[2][1] = mRotation[2][1] * mScale[1]; mMatrix[2][1] = mRotation[2][2] * mScale[2];

	//填写平移项
	mMatrix[0][3] = mTranslate[0]; mMatrix[1][3] = mTranslate[1]; mMatrix[2][3] = mTranslate[2];

	//填写齐次项影响项
	mMatrix[3][0] = 0; mMatrix[3][1] = 0; mMatrix[3][2] = 0; mMatrix[3][3] = 1;
}

const Transform::Matrix4 & Transform::GetTransform()
{
	return mMatrix;
}

void Transform::SetIdentity()
{
	mMatrix.SetIdentity();
}

Transform::Vector3 Transform::GetTranslate()
{
	return mTranslate;
}
