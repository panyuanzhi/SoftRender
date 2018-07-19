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
	//����(qx,qy,qz)���鲿,qw��ʵ��
	float mComponent[4];
	const short x = 0, y = 1, z = 2, w = 3;
};
#endif // !Quaternions_H
