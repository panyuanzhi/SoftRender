#include "Camera.h"

void Camera::SetViewVolume(float right, float left, float top, float bottom, float Near, float Far)
{
	mRight = right; mLeft = left;
	mTop = top; mBottom = bottom;
	mNear = Near; mFar = Far;
}

void Camera::SetPosition(float x, float y, float z)
{
	mPosition[0] = x; mPosition[1] = y; mPosition[2] = z; mPosition[3] = 1;
}

void Camera::SetReferenceFrame(Vector3 r, Vector3 u, Vector3 d)
{
	r.Normalize(); 
	u.Normalize();
	d.Normalize();

	mR = r; mU = u; mD = d;
}

void Camera::Update()
{
	//更新视图矩阵
	mViewMatrix.SetIdentity();
	Vector3 E;
	E[0] = mPosition[0]; E[1] = mPosition[1]; E[2] = mPosition[2];
	mViewMatrix[0][0] = mR[0]; mViewMatrix[0][1] = mR[1]; mViewMatrix[0][2] = mR[2]; mViewMatrix[0][3] = -mR*E;
	mViewMatrix[1][0] = mU[0]; mViewMatrix[1][1] = mU[1]; mViewMatrix[1][2] = mU[2]; mViewMatrix[1][3] = -mU*E;
	mViewMatrix[2][0] = mD[0]; mViewMatrix[2][1] = mD[1]; mViewMatrix[2][2] = mD[2]; mViewMatrix[2][3] = -mD*E;

	//更新投影矩阵
	mProjectionMatrix.Fill(0);
	mProjectionMatrix[0][0] = 2 * mNear / (mRight - mLeft);	mProjectionMatrix[0][2] = -(mRight + mLeft) / (mRight - mLeft);
	mProjectionMatrix[1][1] = 2 * mNear / (mTop - mBottom);	mProjectionMatrix[1][2] = -(mTop + mBottom) / (mTop - mBottom);
	mProjectionMatrix[2][2] = mFar / (mFar - mNear); mProjectionMatrix[2][3] = -mFar*mNear / (mFar - mNear);
	mProjectionMatrix[3][2] = 1;
}

const Camera::Matrix4 & Camera::GetViewMatrix()
{
	return mViewMatrix;
}

const Camera::Matrix4 & Camera::GetProjectionMatrix()
{
	return mProjectionMatrix;
}

bool Camera::OutSide(BoundingSphere bounding)
{
	Point4 lbn, ltn, rbn, rtn, lbf, ltf, rbf, rtf;
	lbn[0] = mLeft; lbn[1] = mBottom; lbn[2] = mNear; lbn[3] = 1;
	ltn[0] = mLeft; ltn[1] = mTop; ltn[2] = mNear; ltn[3] = 1;
	rbn[0] = mRight; rbn[1] = mBottom; rbn[2] = mNear; rbn[3] = 1;
	rtn[0] = mRight; rtn[1] = mTop; rtn[2] = mNear; rtn[3] = 1;
	lbf[0] = mLeft; lbf[1] = mBottom; lbf[2] = mFar; lbf[3] = 1;
	ltf[0] = mLeft; ltf[1] = mTop; ltf[2] = mFar; ltf[3] = 1;
	rbf[0] = mRight; rbf[1] = mBottom; rbf[2] = mFar; rbf[3] = 1;
	rtf[0] = mRight; rtf[1] = mTop; rtf[2] = mFar; rtf[3] = 1;
	
	//包围体只要在6个平面中任意一个平面外，则该包围体在整个视景体之外
	if (bounding.OutSide(mPosition, lbn, ltn)
		|| bounding.OutSide(mPosition, ltn, rtn)
		|| bounding.OutSide(mPosition, rtn, rbn)
		|| bounding.OutSide(mPosition, rbn, lbn)
		|| bounding.OutSide(lbn, ltn, rtn)
		|| bounding.OutSide(rtf, ltf, lbf)) {
		return true;
	}
	else return false;
}
