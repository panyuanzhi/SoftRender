#include "BoundingVolume.h"

BoundingSphere::BoundingSphere()
{
	mRadius = 0;
	mCenter[0] = 0; mCenter[1] = 0; mCenter[2] = 0; mCenter[3] = 1;
}

BoundingSphere::BoundingSphere(Point4 center, float radius)
{
	mCenter = center;
	mRadius = radius;
}

void BoundingSphere::ComputeBounding(const VBO & vbo)
{
	VBO::const_iterator it = vbo.begin();
	VBO::const_iterator end = vbo.end();

	//¼ÆËãÔ²ÐÄ
	Vector4 sum(0.0);
	for (; it != end; ++it) {
		Vector4 point;
		point[0] = (*it).point[0];
		point[1] = (*it).point[1];
		point[2] = (*it).point[2];
		point[3] = (*it).point[3];
		sum += point;
	}
	float size = vbo.size();
	mCenter= sum / size;
	
	//¼ÆËã°ë¾¶
	float radius = 0;
	it = vbo.begin();
	for (; it != end; ++it) {
		Vector4 distance = (*it).point - mCenter;
		float d = distance.GetNorm();
		radius = (radius > d) ? radius : d;
	}
	mRadius = radius;
}

bool BoundingSphere::OutSide(Point4 v0, Point4 v1, Point4 v2)
{
	Vector3 v0v1,v0v2;
	v0v1[0] = v1[0] - v0[0]; v0v1[1] = v1[1] - v0[1]; v0v1[2] = v1[2] - v0[2];
	v0v2[0] = v2[0] - v0[0]; v0v2[1] = v2[1] - v0[1]; v0v2[2] = v2[2] - v0[2];
	Vector3 normal = itk::CrossProduct(v0v2, v0v1);
	normal.Normalize();
	Vector3 vec;
	vec[0] = mCenter[0] - v0[0]; vec[1] = mCenter[1] - v0[1]; vec[2] = mCenter[2] - v0[2];
	float distance = vec*normal;
	if (distance <= -mRadius)
		return true;
	else return false;
}

BoundingSphere BoundingSphere::Merge(BoundingSphere sp1, BoundingSphere sp2)
{
	if (sp1.GetRadius() == 0 && sp2.GetRadius() == 0) {
		BoundingSphere empty;
		return empty;
	}
	else if (sp1.GetRadius() == 0 && sp2.GetRadius() != 0) {
		return sp2;
	}
	else if (sp1.GetRadius() != 0 && sp2.GetRadius() == 0) {
		return sp1;
	}
	else {
		Point4 c1 = sp1.GetCenter();
		Point4 c2 = sp2.GetCenter();
		Vector4 direction = c2 - c1;
		direction.Normalize();
		Point4 begin = c1 - sp1.GetRadius()*direction;
		Point4 end = c2 + sp2.GetRadius()*direction;
		Point4 zero;
		Point4 center = ((end - zero) + (begin - zero)) / 2;
		Vector4 vec = end - begin;
		float diameter = vec.GetNorm();
		return BoundingSphere(center, diameter / 2);
	}
}

BoundingSphere::Point4 BoundingSphere::GetCenter()
{
	return mCenter;
}

float BoundingSphere::GetRadius()
{
	return mRadius;
}

void BoundingSphere::SetCenter(Point4 center)
{
	mCenter = center;
}
