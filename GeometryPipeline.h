#ifndef GeometryPipeline_H
#define GeometryPipeline_H
#include "Traits.h"
#include "Quaternions.h"
#include "vnl\algo\vnl_adjugate.h"
#include "Light.h"

//几何管线的主要作用是根据IBO将VBO中的顶点变换至屏幕空间。并将VBO中的属性进行顶点着色。
//几何管线的输入为IBO和VBO
//几何管线的输出是变换后的IBO和V2FBuffer(包含顶点坐标，和相关的顶点着色器输出)
class GeometryPipeline
{
public:
	typedef typename MathTraits::Matrix4 Matrix4;
	typedef typename MathTraits::Matrix3 Matrix3;
	typedef typename MathTraits::Vector3 Vector3;
	typedef typename MathTraits::Vector4 Vector4;
	typedef typename MathTraits::Point4 Point4;
	typedef typename RenderTraits::VBO VBO;
	typedef typename RenderTraits::IBO IBO;
	typedef typename RenderTraits::V2FBuffer V2FBuffer;
	enum Enum_Boundary { Left, Right, Top, Bottom, Near, Far };

	GeometryPipeline(VBO &vbo,IBO &ibo,float ScreenWidth,float ScreenHeight,const Light &light);
	~GeometryPipeline();
	//void ModelTranslate(float x, float y, float z);
	void ModelRotate(Vector3 src, Vector3 dst);
	void SetViewMatrix(const Matrix4 & view);
	void SetProjectionMatrix(const Matrix4 & proj);
	void SetModelMatrix(const Matrix4 &model);
	void Excute();
	void Init();

	V2FBuffer& GetV2FBuffer();
private:	
	V2F VertexShader(const VertexAttribution &vertexAttr);
	void ApplyVertexShader();

	void BackFaceCulling();
	bool BackFaceTest(Point4 v0, Point4 v1, Point4 v2);

	bool isInside(Point4 p, Enum_Boundary b);
	bool isCross(Point4 p1, Point4 p2, Enum_Boundary plane);
	V2F Intersect(const V2F &sp1,const V2F &sp2, Enum_Boundary plane);
	//对于顶点序列 V0,V1,……,Vn,CloseClip裁剪线段Vn,V0
	void CloseClip(V2FBuffer &v2fBuffer, V2F* first[], V2F* start[]);
	void ClipPoint(V2F v2f, Enum_Boundary plane, V2FBuffer &v2fBuffer, V2F* first[], V2F* start[]);
	void PolygonClipSuthHodg(TriangleIndex triIndex, V2FBuffer &v2fBuffer);
	void Clip();
	void ClipTriangle(TriangleIndex triIndex, IBO &ibo, V2FBuffer &v2fBuffer);

	void PerspectiveDivision(V2F &v2f);
	void ScreenTransform(V2F &v2f);
	void PerspectiveDivisionAndScreenTransform();
private:
	Matrix4 ModelMatrix;
	Matrix4 ViewMatrix;
	Matrix4 ProjectionMatrix;

	VBO &mVBO;
	IBO &mIBO;
	V2FBuffer mV2FBuffer;

	float mScreenWidth;
	float mScreenHeight;

	const Light &mLight;
};
#endif // !GeometryPipeline_H
