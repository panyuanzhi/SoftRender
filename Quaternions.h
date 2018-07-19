#ifndef Quaternions_H
#define Quaternions_H
#include"Traits.h"
class Quaternion
{
public:
	typedef typename MathTraits::Matrix4 Matrix4;
	typedef typename MathTraits::Matrix3 Matrix3;
	typedef typename MathTraits::Vector3 Vector3;

	Quaternion();
	~Quaternion();
	Matrix4 GetRotationMatrix();
	void SetQuaternion(Vector3 axis, float theta);
	static Matrix4 GetRotationMatrix(Vector3 src, Vector3 dst);
private:
	//(qx,qy,qz,qw);
	//其中(qx,qy,qz)是虚部,qw是实部
	float mComponent[4];
	const short x = 0, y = 1, z = 2, w = 3;
};
#endif // !Quaternions_H
