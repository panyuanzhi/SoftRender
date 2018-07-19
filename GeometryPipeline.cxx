#include "GeometryPipeline.h"

GeometryPipeline::GeometryPipeline(VBO &vbo, IBO &ibo, float ScreenWidth, float ScreenHeight, const Light &light)
	:mVBO(vbo),mIBO(ibo),mScreenWidth(ScreenWidth),mScreenHeight(ScreenHeight),mLight(light)
{
	ModelMatrix.SetIdentity();
	ViewMatrix.SetIdentity();
	ProjectionMatrix.Fill(0);
}

GeometryPipeline::~GeometryPipeline()
{
}

//void GeometryPipeline::ModelTranslate(float x, float y, float z)
//{
//	Matrix4 matrix;
//	matrix.SetIdentity();
//	matrix[0][3] = x; matrix[1][3] = y; matrix[2][3] = z;
//	ModelMatrix = matrix*ModelMatrix;
//}

void GeometryPipeline::ModelRotate(Vector3 src, Vector3 dst)
{
	Matrix4 matrix = Quaternion::GetRotationMatrix(src, dst);
	ModelMatrix = matrix*ModelMatrix;
}

void GeometryPipeline::SetViewMatrix(const Matrix4 & view)
{
	ViewMatrix = view;
	//ViewMatrix.SetIdentity();
	//Vector3 E;
	//E[0] = Eye[0]; E[1] = Eye[1]; E[2] = Eye[2];
	//ViewMatrix[0][0] = R[0]; ViewMatrix[0][1] = R[1]; ViewMatrix[0][2] = R[2]; ViewMatrix[0][3] = -R*E;
	//ViewMatrix[1][0] = U[0]; ViewMatrix[1][1] = U[1]; ViewMatrix[1][2] = U[2]; ViewMatrix[1][3] = -U*E;
	//ViewMatrix[2][0] = D[0]; ViewMatrix[2][1] = D[1]; ViewMatrix[2][2] = D[2]; ViewMatrix[2][3] = -D*E;
}

void GeometryPipeline::SetProjectionMatrix(const Matrix4 & proj)
{
	ProjectionMatrix = proj;
	//ProjectionMatrix.Fill(0);
	//ProjectionMatrix[0][0] = 2 * Near / (Right - Left);	ProjectionMatrix[0][2] = -(Right + Left) / (Right - Left);
	//ProjectionMatrix[1][1] = 2 * Near / (Top - Bottom);	ProjectionMatrix[1][2] = -(Top + Bottom) / (Top - Bottom);
	//ProjectionMatrix[2][2] = Far / (Far - Near); ProjectionMatrix[2][3] = -Far*Near / (Far - Near);
	//ProjectionMatrix[3][2] = 1;
}

void GeometryPipeline::SetModelMatrix(const Matrix4 & model)
{
	ModelMatrix = model;
}

const int nClip = 6;
void GeometryPipeline::PolygonClipSuthHodg(TriangleIndex triIndex,V2FBuffer &v2fBuffer)
{
	V2F* first[nClip] = { 0,0,0,0,0,0 }, *s[nClip] ;
	for (int i = 0; i < 3; ++i) {
		ClipPoint(mV2FBuffer[triIndex.v[i]], Enum_Boundary::Left, v2fBuffer, first, s);
	}
	CloseClip(v2fBuffer, first, s);
}

void GeometryPipeline::ApplyVertexShader()
{
	std::vector<VertexAttribution>::iterator it = mVBO.begin();
	std::vector<VertexAttribution>::iterator end = mVBO.end();
	for (; it != end; ++it) {
		V2F vertexShaderOutput = VertexShader(*it);
		mV2FBuffer.push_back(vertexShaderOutput);
	}

	//顶点着色结束后，释放VBO中的内容。
	VBO().swap(mVBO);
}

void GeometryPipeline::BackFaceCulling()
{
	std::list<TriangleIndex>::iterator it = mIBO.begin();
	std::list<TriangleIndex>::iterator end = mIBO.end();
	while (it != end) {
		Point4 v0 = mV2FBuffer[(*it).v[0]].point;
		Point4 v1 = mV2FBuffer[(*it).v[1]].point;
		Point4 v2 = mV2FBuffer[(*it).v[2]].point;
		if (BackFaceTest(v0, v1, v2)) {
			++it;
		}
		else {
			it = mIBO.erase(it);
		}
	}
}

V2F GeometryPipeline::VertexShader(const VertexAttribution &vertexAttr)
{
	Point4 point= ModelMatrix*vertexAttr.point;
	point = ViewMatrix*point;
	point = ProjectionMatrix*point;
	V2F v2f;
	v2f.point = ProjectionMatrix*ViewMatrix*ModelMatrix*vertexAttr.point;

	//VectorType view;
	//view[0] = p[0] - Eye[0]; view[1] = p[1] - Eye[1]; view[2] = p[2] - Eye[2];

	vnl_matrix<float> vnlModel(3, 3);
	vnlModel[0][0] = ModelMatrix[0][0]; vnlModel[0][1] = ModelMatrix[0][1]; vnlModel[0][2] = ModelMatrix[0][2];
	vnlModel[1][0] = ModelMatrix[1][0]; vnlModel[1][1] = ModelMatrix[1][1]; vnlModel[1][2] = ModelMatrix[1][2];
	vnlModel[2][0] = ModelMatrix[2][0]; vnlModel[2][1] = ModelMatrix[2][1]; vnlModel[2][2] = ModelMatrix[2][2];

	vnl_matrix<float> vnlModelAdj = vnl_adjugate(vnlModel);
	Matrix3 modelAdj(vnlModelAdj);
	Matrix3 modelAdjT(modelAdj.GetTranspose());

	//WorldNormal
	v2f.normal = modelAdjT *vertexAttr.normal;
	v2f.normal.Normalize();

	
	//uv
	v2f.uv = vertexAttr.uv;

	//WorldTangent
	Matrix3 model3x3(vnlModel);
	v2f.tangent = model3x3*vertexAttr.tangent;

	//WorldBitangent
	v2f.bitangent = model3x3*vertexAttr.bitangent;

	//float cosThetaI = worldNormal*mLightDir;
	//cosThetaI = (cosThetaI > 0) ? cosThetaI : 0;
	//cosThetaI = 1;
	//RGBColor Cspec;
	//Cspec[1] = 1.0;
	//VectorType h = (view + mLightDir) / 2;
	//h.Normalize();
	//float cosThetaH = h*worldNormal;
	//cosThetaH = (cosThetaH > 0) ? cosThetaH : 0;
	//cosThetaH = 1.0;
	//int m = 1;
	//RGBColor test2 = Cspec*((m + 8) / (8 * 3.1415926))*pow(cosThetaH, m);

	return v2f;
}

bool GeometryPipeline::BackFaceTest(Point4 v0, Point4 v1, Point4 v2)
{
	Vector3 e1;
	e1[0] = v1[0] - v0[0], e1[1] = v1[1] - v0[1], e1[2] = v1[3] - v0[3];
	Vector3 e2;
	e2[0] = v2[0] - v0[0], e2[1] = v2[1] - v0[1], e2[2] = v2[3] - v0[3];
	Vector3 Normal = itk::CrossProduct(e1, e2);
	Vector3 vec;
	vec[0] = v0[0], vec[1] = v0[1], vec[2] = v0[3];
	bool result = (Normal*vec>0);
	//todo:返回结果有可能需要修正
	return result;
}

//裁剪以后生成的顶点数将和索引数相同
//虽然会增加顶点数，增加所占用的内存。
//但是因为后续算法都是按照三角形来执行的，并不会增加后续算法的时间。
//虽然这样会导致同一个顶点有多个副本，但是没有关系，
//后续算法不会出现按索引修改顶点坐标和顶点属性的情况
void GeometryPipeline::Clip()
{
	V2FBuffer clipBuffer;
	IBO clipIBO;
	std::list<TriangleIndex>::iterator it = mIBO.begin();
	std::list<TriangleIndex>::iterator end = mIBO.end();
	for (; it != end; ++it) {
		IBO ibo;
		V2FBuffer v2fBuffer;
		//裁剪一个三角形，生成裁剪后的索引和V2FBuffer
		ClipTriangle(*it, ibo, v2fBuffer);
		
		//将裁剪后的V2FBuffer附加到clipBuffer的后面，索引也增加相应的增量。
		unsigned int addition = clipBuffer.size();
		IBO::iterator IBOIt = ibo.begin();
		IBO::iterator IBOEnd = ibo.end();
		for (; IBOIt != IBOEnd; ++IBOIt) {
			TriangleIndex triIndex = *IBOIt;
			for (int i = 0; i < 3; ++i) {
				triIndex.v[i] += addition;
			}
			clipIBO.push_back(triIndex);
		}
		V2FBuffer::iterator V2FIt = v2fBuffer.begin();
		V2FBuffer::iterator V2FEnd = v2fBuffer.end();
		for (; V2FIt != V2FEnd; ++V2FIt) {
			clipBuffer.push_back(*V2FIt);
		}

	}

	clipIBO.swap(mIBO);
	clipBuffer.swap(mV2FBuffer);
}

void GeometryPipeline::ClipTriangle(TriangleIndex triIndex, IBO & ibo, V2FBuffer &v2fBuffer)
{
	PolygonClipSuthHodg(triIndex, v2fBuffer);

	//构造三角形索引
	int size = v2fBuffer.size();
	for (int i = 1; i < size - 1; ++i) {
		TriangleIndex index;
		index.v[0] = 0;
		index.v[1] = i;
		index.v[2] = i + 1;
		ibo.push_back(index);
	}
}

void GeometryPipeline::PerspectiveDivision(V2F &v2f)
{
	//attr = attr / input[3];
	//input[0] = input[0] / input[3];
	//input[1] = input[1] / input[3];
	//input[2] = input[2] / input[3];
	//input[3] = 1.;

	float w = v2f.point[3];

	v2f.one /= w;
	v2f.point[0] /= w;
	v2f.point[1] /= w;
	v2f.point[2] /= w;
	v2f.point[3] /= w;
	v2f.uv[0] /= w;
	v2f.uv[1] /= w;
	v2f.normal /= w;
	v2f.tangent /= w;
	v2f.bitangent /= w;
}

void GeometryPipeline::ScreenTransform(V2F &v2f)
{
	//缩放到[0,Image_Width-1]范围，因为屏幕范围是[0,Image_Width-1],整个长度为Image_Width
	//图像的生成改为采样生成，因此范围修改为缩放到[0.Image_Width]
	v2f.point[0]= (mScreenWidth)*(v2f.point[0] + 1) / 2.;
	v2f.point[1]= (mScreenHeight)*(1 - v2f.point[1]) / 2.;
}

void GeometryPipeline::PerspectiveDivisionAndScreenTransform()
{
	V2FBuffer::iterator it = mV2FBuffer.begin();
	V2FBuffer::iterator end = mV2FBuffer.end();
	for (; it != end; ++it) {
		PerspectiveDivision(*it);
		ScreenTransform(*it);
	}
}

void GeometryPipeline::Excute()
{
	//经过顶点着色以后VBO中的数据被转换成V2FBuffer中的数据
	//VBO中的数据完全失效，且被释放。
	ApplyVertexShader();
	
	//背面剔除只删除三角形，因此IBO被修改。
	BackFaceCulling();

	//裁剪会更新索引和V2FBuffer
	Clip();

	//将V2FBuffer中的数据进行透视除法和屏幕变换
	PerspectiveDivisionAndScreenTransform();

	//输出的结果是IBO和V2FBuffer
}

void GeometryPipeline::Init()
{
	//Buffer清空
	V2FBuffer().swap(mV2FBuffer);
}

GeometryPipeline::V2FBuffer & GeometryPipeline::GetV2FBuffer()
{
	return mV2FBuffer;
}

bool GeometryPipeline::isInside(Point4 p, Enum_Boundary b)
{
	switch (b)
	{
	case Enum_Boundary::Left:
		if ((p[0] / p[3]) < -1)
			return false;
		break;
	case Enum_Boundary::Right:
		if ((p[0] / p[3]) > 1)
			return false;
		break;
	case Enum_Boundary::Bottom:
		if ((p[1] / p[3]) < -1)
			return false;
		break;
	case Enum_Boundary::Top:
		if ((p[1] / p[3]) > 1)
			return false;
		break;
	case Enum_Boundary::Near:
		if ((p[2] / p[3]) < 0)
			return false;
		break;
	case Enum_Boundary::Far:
		if ((p[2] / p[3]) > 1)
			return false;
		break;
	default:
		break;
	}
	return true;
}

bool GeometryPipeline::isCross(Point4 p1, Point4 p2, Enum_Boundary plane)
{
	if (isInside(p1, plane) == isInside(p2, plane))
		return false;
	else return true;
}

V2F GeometryPipeline::Intersect(const V2F &sp1, const V2F &sp2, Enum_Boundary plane)
{
	//发生除0错误的情况为直线与平面不相交有0个交点，或者直线就在这个平面上，有无穷多个交点。
	//传入函数的两点分别位于平面的两侧，所以不会发生除0错误。

	Point4 p1, p2;
	p1 = sp1.point;
	p2 = sp2.point;
	float u;
	switch (plane)
	{
	case Enum_Boundary::Left:
		u = (p1[0] + p1[3]) / ((p1[0] + p1[3]) - (p2[0] + p2[3]));
		break;
	case Enum_Boundary::Right:
		u = (p1[0] - p1[3]) / ((p1[0] - p1[3]) - (p2[0] - p2[3]));
		break;
	case Enum_Boundary::Bottom:
		u = (p1[1] + p1[3]) / ((p1[1] + p1[3]) - (p2[1] + p2[3]));
		break;
	case Enum_Boundary::Top:
		u = (p1[1] - p1[3]) / ((p1[1] - p1[3]) - (p2[1] - p2[3]));
		break;
	case Enum_Boundary::Near:
		//z/w的区间为[0,1]
		u = p1[2] / (p1[2] - p2[2]);
		break;
	case Enum_Boundary::Far:
		u = (p1[2] - p1[3]) / ((p1[2] - p1[3]) - (p2[2] - p2[3]));
		break;
	default:
		u = 0;
		break;
	}

	//LinearInterpolation的p1,p2必须按顺序来。因为u是按照p1,p2的顺序来求的
	//通过线性插值生成交点的属性
	//此时仍在裁剪阶段，顶点位于齐次空间，索引应该是通过线性插值生成顶点属性
	return V2F::Lerp(sp1,sp2,u);
}

void GeometryPipeline::CloseClip(V2FBuffer &v2fBuffer, V2F* first[], V2F* start[])
{
	for (Enum_Boundary plane = Enum_Boundary::Left; plane <= Enum_Boundary::Far; plane = (Enum_Boundary)(plane + 1))
	{
		//如果start和first都不存在，说明3个顶点在到达这个平面前已经被全部裁剪。
		//即3个顶点完全在视景体之外
		if (start[plane] && first[plane]) {
			if (isCross(start[plane]->point, first[plane]->point, plane)) {
				V2F intersection = Intersect(*start[plane], *first[plane], plane);
				//交点一定会通过当前平面的裁剪测试，因此之间送入下一个平面做裁剪测试
				if (plane < Enum_Boundary::Far)
					ClipPoint(intersection, (Enum_Boundary)(plane + 1), v2fBuffer, first, start);
				else {
					v2fBuffer.push_back(intersection);
				}
			}
		}

	}
}

void GeometryPipeline::ClipPoint(V2F v2f, Enum_Boundary plane, V2FBuffer &v2fBuffer, V2F* first[], V2F* start[])
{
	//first之所以要用指针数组是因为要判断一个点是不是第一个进入裁剪管线的。指针first为空可以表示没有firs点。但是如果直接用数组就无法表示没有first点的情况。
	if (!first[plane]) {
		first[plane] = new V2F;
		(*first[plane]) = v2f;
	}

	else {
		if (isCross(start[plane]->point, v2f.point, plane)) {
			V2F intersection= Intersect(*start[plane], v2f, plane);
			//交点一定会通过当前平面的裁剪测试，所以直接送入下一个平面做裁剪测试
			if (plane < Enum_Boundary::Far)
				ClipPoint(intersection, (Enum_Boundary)(plane + 1), v2fBuffer, first, start);
			else
			{
				v2fBuffer.push_back(intersection);
			}

		}
	}
	//进入裁剪管线线段为start-----p
	//程序进入到这里说明已经对p处理完成，接下来要判断是否应将p传入到下一个裁剪面上去。
	{
		//保存裁剪起点
		start[plane] = new V2F;
		*(start[plane]) = v2f;
	}
	//如果p通过了plane的裁剪测试，将p传递到下一个裁剪平面做裁剪。否则直接丢弃p。
	if (isInside(v2f.point, plane)) {
		if (plane < Enum_Boundary::Far)
			ClipPoint(v2f, (Enum_Boundary)(plane + 1), v2fBuffer, first, start);
		else {
			v2fBuffer.push_back(v2f);
		}
	}
}
