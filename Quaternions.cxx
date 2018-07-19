#include "Quaternions.h"
Quaternion::Quaternion()
{
}

Quaternion::~Quaternion()
{
}

Quaternion::Matrix4 Quaternion::GetRotationMatrix()
{
	Matrix4 matrix;
	matrix[0][0] = 1 - 2 * (mComponent[y] * mComponent[y] + mComponent[z] * mComponent[z]);
	matrix[0][1] = 2 * (mComponent[x] * mComponent[y] - mComponent[w] * mComponent[z]);
	matrix[0][2] = 2 * (mComponent[x] * mComponent[z] + mComponent[w] * mComponent[y]);
	matrix[0][3] = 0;

	matrix[1][0] = 2 * (mComponent[x] * mComponent[y] + mComponent[w] * mComponent[z]);
	matrix[1][1] = 1 - 2 * (mComponent[x] * mComponent[x] + mComponent[z] * mComponent[z]);
	matrix[1][2] = 2 * (mComponent[y] * mComponent[z] - mComponent[w] * mComponent[x]);
	matrix[1][3] = 0;

	matrix[2][0] = 2 * (mComponent[x] * mComponent[z] - mComponent[w] * mComponent[y]);
	matrix[2][1] = 2 * (mComponent[y] * mComponent[z] + mComponent[w] * mComponent[x]);
	matrix[2][2] = 1 - 2 * (mComponent[x] * mComponent[x] + mComponent[y] * mComponent[y]);
	matrix[2][3] = 0;

	matrix[3][0] = 0;
	matrix[3][1] = 0;
	matrix[3][2] = 0;
	matrix[3][3] = 1;
	
	return matrix;
}

void Quaternion::SetQuaternion(Vector3 axis, float theta)
{
	axis.Normalize();
	float fai = theta / 2;
	mComponent[0] = (sinf(fai)*axis)[0];
	mComponent[1] = (sinf(fai)*axis)[1];
	mComponent[2] = (sinf(fai)*axis)[2];

	mComponent[3] = cosf(fai);
}

Quaternion::Matrix4 Quaternion::GetRotationMatrix(Vector3 src, Vector3 dst)
{
	src.Normalize();
	dst.Normalize();
	Vector3 v = itk::CrossProduct(src, dst);
	float e = src*dst;
	float h = 1 / (1 + e);

	const short x = 0, y = 1, z = 2;
	Matrix4 matrix;
	matrix[0][0] = e + h*v[x] * v[x];
	matrix[0][1] = h*v[x] * v[y] - v[z];
	matrix[0][2] = h*v[x] * v[z] + v[y];
	matrix[0][3] = 0;

	matrix[1][0] = h*v[x] * v[y] + v[z];
	matrix[1][1] = e + h*v[y] * v[y];
	matrix[1][2] = h*v[y] * v[z] - v[x];
	matrix[1][3] = 0;

	matrix[2][0] = h*v[x] * v[z] - v[y];
	matrix[2][1] = h*v[y] * v[z] + v[x];
	matrix[2][2] = e + h*v[z] * v[z];
	matrix[2][3] = 0;

	matrix[3][0] = 0;
	matrix[3][1] = 0;
	matrix[3][2] = 0;
	matrix[3][3] = 1;

	return matrix;
}
