#pragma once
#include "Traits.h"
#include "Material.h"

class Mesh {
public:
	typedef typename RenderTraits::VBO VBO;
	typedef typename RenderTraits::IBO IBO;
	const VBO& GetVBO();
	const IBO& GetIBO();
protected:
	VBO mVBO;
	IBO mIBO;
};

class CubeMesh :public Mesh{
public:
	typedef typename MathTraits::Point4 Point4;
	typedef typename MathTraits::Point2 Point2;
	typedef typename MathTraits::Vector3 Vector3;
	typedef typename MathTraits::Matrix2 Matrix2;
	typedef typename MathTraits::Matrix2x3 Matrix2x3;
	CubeMesh();
	void Scale(float x, float y, float z);
private:
	//∂•µ„ƒÊ ±’ÎÀ≥–Ú
	void GenTriangle(Point4 v[], Point2 uv[]);
	void GenVertices();
	void ConstructQuad(Point4 v[]);
	void GenIndex();
	void GenAttribution();
	void ConstructAttribution(TriangleIndex triangle);
};