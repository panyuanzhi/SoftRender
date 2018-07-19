#include "Mesh.h"

CubeMesh::CubeMesh()
{
	GenVertices();
	GenIndex();
	GenAttribution();
}

void CubeMesh::Scale(float x, float y, float z)
{
	VBO::iterator it = mVBO.begin();
	VBO::iterator end = mVBO.end();
	for (; it != end; ++it) {
		it->point[0] *= x;
		it->point[1] *= y;
		it->point[2] *= z;
	}
}

void CubeMesh::GenTriangle(Point4 v[], Point2 uv[])
{
}

void CubeMesh::GenVertices()
{
	Point4 v[4];
	//上表面
	v[0][0] = -1; v[0][1] = 1; v[0][2] = -1; v[0][3] = 1;
	v[1][0] = 1; v[1][1] = 1; v[1][2] = -1; v[1][3] = 1;
	v[2][0] = 1; v[2][1] = 1; v[2][2] = 1; v[2][3] = 1;
	v[3][0] = -1; v[3][1] = 1; v[3][2] = 1; v[3][3] = 1;
	ConstructQuad(v);

	//下表面
	v[0][0] = -1; v[0][1] = -1; v[0][2] = -1; v[0][3] = 1;
	v[1][0] = 1; v[1][1] = -1; v[1][2] = -1; v[1][3] = 1;
	v[2][0] = 1; v[2][1] = -1; v[2][2] = 1; v[2][3] = 1;
	v[3][0] = -1; v[3][1] = -1; v[3][2] = 1; v[3][3] = 1;

	Point4 inverse[4];
	for (int i = 0; i < 4; ++i) {
		inverse[i] = v[3 - i];
	}
	//因为下面表面应该是朝下的，所以顶点顺序反转一下。后面同理
	ConstructQuad(inverse);

	//前表面
	v[0][0] = -1; v[0][1] = -1; v[0][2] = -1; v[0][3] = 1;
	v[1][0] = 1; v[1][1] = -1; v[1][2] = -1; v[1][3] = 1;
	v[2][0] = 1; v[2][1] = 1; v[2][2] = -1; v[2][3] = 1;
	v[3][0] = -1; v[3][1] = 1; v[3][2] = -1; v[3][3] = 1;
	ConstructQuad(v);

	//后表面
	v[0][0] = -1; v[0][1] = -1; v[0][2] = 1; v[0][3] = 1;
	v[1][0] = 1; v[1][1] = -1; v[1][2] = 1; v[1][3] = 1;
	v[2][0] = 1; v[2][1] = 1; v[2][2] = 1; v[2][3] = 1;
	v[3][0] = -1; v[3][1] = 1; v[3][2] = 1; v[3][3] = 1;
	for (int i = 0; i < 4; ++i) {
		inverse[i] = v[3 - i];
	}
	ConstructQuad(inverse);

	//右表面
	v[0][0] = 1; v[0][1] = -1; v[0][2] = -1; v[0][3] = 1;
	v[1][0] = 1; v[1][1] = -1; v[1][2] = 1; v[1][3] = 1;
	v[2][0] = 1; v[2][1] = 1; v[2][2] = 1; v[2][3] = 1;
	v[3][0] = 1; v[3][1] = 1; v[3][2] = -1; v[3][3] = 1;
	ConstructQuad(v);

	//左表面
	v[0][0] = -1; v[0][1] = -1; v[0][2] = -1; v[0][3] = 1;
	v[1][0] = -1; v[1][1] = -1; v[1][2] = 1; v[1][3] = 1;
	v[2][0] = -1; v[2][1] = 1; v[2][2] = 1; v[2][3] = 1;
	v[3][0] = -1; v[3][1] = 1; v[3][2] = -1; v[3][3] = 1;
	for (int i = 0; i < 4; ++i) {
		inverse[i] = v[3 - i];
	}
	ConstructQuad(inverse);
}

void CubeMesh::ConstructQuad(Point4 v[])
{
	Point2 uv[4];
	uv[0][0] = 0; uv[0][1] = 0;
	uv[1][0] = 1; uv[1][1] = 0;
	uv[2][0] = 1; uv[2][1] = 1;
	uv[3][0] = 0; uv[3][1] = 1;
	for (int i = 0; i < 4; ++i) {
		VertexAttribution attr;
		attr.point = v[i];
		attr.uv = uv[i];
		mVBO.push_back(attr);
	}
}

void CubeMesh::GenIndex()
{
	int vertex=0;
	for (int side = 0; side < 6; ++side) {
		TriangleIndex index;
		index.v[0] = vertex;
		index.v[1] = vertex + 1;
		index.v[2] = vertex + 2;
		mIBO.push_back(index);
		index.v[1] = vertex + 2;
		index.v[2] = vertex + 3;
		mIBO.push_back(index);
		vertex += 4;
	}
}

void CubeMesh::GenAttribution()
{
	IBO::iterator it = mIBO.begin();
	IBO::iterator end = mIBO.end();
	for (; it != end; ++it) {
		ConstructAttribution(*it);
	}
}

void CubeMesh::ConstructAttribution(TriangleIndex triangle)
{
	typedef Vector3 Edge;
	Edge p0p1, p0p2;
	p0p1[0] = mVBO[triangle.v[1]].point[0] - mVBO[triangle.v[0]].point[0]; 
	p0p1[1] = mVBO[triangle.v[1]].point[1] - mVBO[triangle.v[0]].point[1]; 
	p0p1[2] = mVBO[triangle.v[1]].point[2] - mVBO[triangle.v[0]].point[2];

	p0p2[0] = mVBO[triangle.v[2]].point[0] - mVBO[triangle.v[0]].point[0]; 
	p0p2[1] = mVBO[triangle.v[2]].point[1] - mVBO[triangle.v[0]].point[1]; 
	p0p2[2] = mVBO[triangle.v[2]].point[2] - mVBO[triangle.v[0]].point[2];

	Vector3 normal = itk::CrossProduct(p0p2, p0p1);
	normal.Normalize();

	//构建线性方程组C=A T.transpose
	//					B.transpose
	Matrix2 A;

	A[0][0] = mVBO[triangle.v[1]].uv[0] - mVBO[triangle.v[0]].uv[0]; 
	A[0][1] = mVBO[triangle.v[1]].uv[1] - mVBO[triangle.v[0]].uv[1];

	A[1][0] = mVBO[triangle.v[2]].uv[0] - mVBO[triangle.v[0]].uv[0]; 
	A[1][1] = mVBO[triangle.v[2]].uv[1] - mVBO[triangle.v[0]].uv[1];

	Matrix2x3 C;
	C[0][0] = p0p1[0]; C[0][1] = p0p1[1]; C[0][2] = p0p1[2];
	C[1][0] = p0p2[0]; C[1][1] = p0p2[1]; C[1][2] = p0p2[2];

	Matrix2x3 TB;
	TB = A.GetInverse()*C.GetVnlMatrix();
	Vector3 tangent = TB[0];
	tangent.Normalize();
	Vector3 bitangent = TB[1];
	bitangent.Normalize();
	for (int i = 0; i < 3; ++i) {
		mVBO[triangle.v[i]].normal = normal;
		mVBO[triangle.v[i]].tangent = tangent;
		mVBO[triangle.v[i]].bitangent = bitangent;
	}
}

const Mesh::VBO & Mesh::GetVBO()
{
	return mVBO;
}

const Mesh::IBO & Mesh::GetIBO()
{
	return mIBO;
}