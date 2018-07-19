#ifndef Traits_H
#define Traits_H

#include <itkMatrix.h>
#include <itkMesh.h>
#include "itkRGBPixel.h"
#include "itkImage.h"
#include "itkTriangleCell.h"
#include "itkPoint.h"
struct A2V
{

};
class MathTraits
{
public:
	typedef typename itk::Matrix<float, 4, 4> Matrix4;
	typedef typename itk::Matrix<float, 3, 3> Matrix3;
	typedef typename itk::Matrix<float, 2, 2> Matrix2;
	typedef typename itk::Matrix<float, 2, 3> Matrix2x3;
	typedef typename itk::Vector< float, 3 > Vector3;
	typedef typename itk::Vector< float, 4 > Vector4;
	typedef typename itk::Point<float, 4U> Point4;
	typedef typename itk::Point<float, 2U> Point2;
};

class MeshTraits {
public:
	//目前暂时将顶点属性和着色器输出混在一起。
	//属性向量为{1,normal,point,uv,tangent,bitangent}
	typedef typename itk::Vector<float, 16> VertexAttributionType;
	typedef typename itk::Mesh< VertexAttributionType, 4 > MeshType;
	typedef typename MeshType SingleCellMesh;
	typedef typename MeshType SinglePointMesh;
	typedef typename itk::RGBPixel< unsigned char > ImagePixelType;
	typedef typename itk::Image< ImagePixelType, 2 > ImageType;
	typedef typename MeshType::CellType CellType;
	typedef typename itk::TriangleCell<CellType> TriangleCell;
	typedef typename CellType::CellAutoPointer CellAutoPointer;
};
struct VertexAttribution
{
	MathTraits::Point4 point;
	MathTraits::Point2 uv;
	MathTraits::Vector3 normal;
	MathTraits::Vector3 tangent;
	MathTraits::Vector3 bitangent;
};
struct TriangleIndex {
	unsigned int v[3];
};

struct V2F {
public:
	typedef typename MathTraits::Point4 Point4;
	typedef typename MathTraits::Point2 Point2;
	typedef typename MathTraits::Vector3 Vector3;
	typedef typename MathTraits::Vector4 Vector4;

public:
	V2F();
	V2F(float one, Point4 point, Point2 uv, Vector3 normal, Vector3 tangent, Vector3 bitangent);
	static V2F Lerp(const V2F &a, const V2F &b, float u);
	V2F operator + (V2F &b);
	V2F operator -(V2F &b);
	V2F operator *(float u);
public:
	float one = 1.0f;
	Point4 point;
	Point2 uv;
	MathTraits::Vector3 normal;
	MathTraits::Vector3 tangent;
	MathTraits::Vector3 bitangent;
};

inline V2F::V2F()
{
}

inline V2F::V2F(float one, Point4 point, Point2 uv, Vector3 normal, Vector3 tangent, Vector3 bitangent) :
	one(one), point(point), uv(uv), normal(normal), tangent(tangent), bitangent(bitangent)
{
}

inline V2F V2F::Lerp(const V2F & a, const V2F & b, float u)
{
	return V2F(a.one + u*(b.one - a.one), a.point + u*(b.point - a.point), a.uv + u*(b.uv - a.uv),
		a.normal + u*(b.normal - a.normal), a.tangent + u*(b.tangent - a.tangent),
		a.bitangent + u*(b.bitangent - a.bitangent));
}
inline V2F V2F::operator+(V2F & b)
{
	Point4 p;
	p[0] = point[0] + b.point[0];
	p[1] = point[1] + b.point[1];
	p[2] = point[2] + b.point[2];
	p[3] = point[3] + b.point[3];

	Point2 tmpUV;
	tmpUV[0] = uv[0] + b.uv[0];
	tmpUV[1] = uv[1] + b.uv[1];

	return V2F(one+b.one,p,tmpUV,normal+b.normal,tangent+b.tangent,bitangent+b.bitangent);
}
inline V2F V2F::operator-(V2F & b)
{
	Point4 p;
	p[0] = point[0] - b.point[0];
	p[1] = point[1] - b.point[1];
	p[2] = point[2] - b.point[2];
	p[3] = point[3] - b.point[3];

	Point2 tmpUV;
	tmpUV[0] = uv[0] - b.uv[0];
	tmpUV[1] = uv[1] - b.uv[1];

	return V2F(one - b.one, p, tmpUV, normal - b.normal, tangent - b.tangent, bitangent - b.bitangent);
}
inline V2F V2F::operator*(float u)
{
	Point4 p;
	p[0] = point[0] * u;
	p[1] = point[1] * u;
	p[2] = point[2] * u;
	p[3] = point[3] * u;

	Point2 tmpUV;
	tmpUV[0] = uv[0] * u;
	tmpUV[1] = uv[1] * u;

	return V2F(one*u,p,tmpUV,normal*u,tangent*u,bitangent*u);
}

class RenderTraits {
public:
	typedef typename std::vector<VertexAttribution> VBO;
	typedef typename std::list<TriangleIndex> IBO;
	typedef typename std::vector<V2F> V2FBuffer;
};
#endif // !Traits_H
