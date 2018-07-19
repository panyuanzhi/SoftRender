#pragma once
#include "Traits.h"
class BoundingSphere {
public:
	typedef typename MathTraits::Point4 Point4;
	typedef typename MathTraits::Vector4 Vector4;
	typedef typename RenderTraits::VBO VBO;
	typedef typename MathTraits::Matrix4 Matrix4;
	typedef typename MathTraits::Vector3 Vector3;

	BoundingSphere();
	BoundingSphere(Point4 center, float radius);
	void ComputeBounding(const VBO &vbo);
	//判断包围球是否在v0v1v2 三点构成的平面之外
	//三点逆时针顺序。遵守右手法则。法线一侧为平面内侧
	//在外面返回true
	bool OutSide(Point4 v0, Point4 v1, Point4 v2);
	static BoundingSphere Merge(BoundingSphere sp1, BoundingSphere sp2);
	Point4 GetCenter();
	float GetRadius();
	void SetCenter(Point4 center);
private:
	Point4 mCenter;
	float mRadius;
};