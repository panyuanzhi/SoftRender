#include "SoftRenderer.h"

#include "itkImageFileWriter.h"
SoftRenderer::SoftRenderer(int w, int h, std::string outputFileName)
{
	mGeometryPipeline = std::make_shared<GeometryPipeline>(mVBO,mIBO,w,h,mLight);
	mRasterizeStage = std::make_shared<RasterizeStage>(mIBO,mGeometryPipeline->GetV2FBuffer());
	mRasterizeStage->Init(w, h,outputFileName);	
	mAspect = (float)w / (float)h;
}
SoftRenderer::~SoftRenderer()
{
}

void SoftRenderer::CameraInit()
{
	//Eye[0] = 0., Eye[1] = 0., Eye[2] = 0., Eye[3] = 1.;
	//D[0] = 0., D[1] = 0., D[2] = 1.;
	//D.Normalize();
	//U[0] = 0., U[1] = 1., U[2] = 0.;
	//U.Normalize();
	//R[0] = 1., R[1] = 0., R[2] = 0.;
	//R.Normalize();
	//
	//mTop = 10; mBottom = -mTop;
	//float width = (mTop - mBottom) * mAspect;
	//mRight = width / 2; mLeft= - mRight;
	//mNear = 50; mFar = 5000;

	//mGeometryPipeline->SetViewMatrix(Eye, R, U, D);
	//mGeometryPipeline->SetProjectionMatrix(mNear, mFar, mLeft, mRight, mTop, mBottom);
}

//void SoftRenderer::LightInit()
//{
//	mLightDir[0] = 0.0;
//	mLightDir[1] = 1.0;
//	mLightDir[2] = 0.0;
//
//	mLightIrradiance[0] = 1.0;
//	mLightIrradiance[1] = 1.0;
//	mLightIrradiance[2] = 1.0;
//	mLightIrradiance[3] = 1.0;
//}

void SoftRenderer::SetLight(const Light & light)
{
	mLight = light;
}

void SoftRenderer::DrawTriangle()
{
	mGeometryPipeline->Init();
	mGeometryPipeline->Excute();
	//V2FBuffer &v2fBuffer = mGeometryPipeline->GetV2FBuffer();

	IBO::iterator it = mIBO.begin();
	IBO::iterator end = mIBO.end();
	for (; it != end; ++it) {
		mRasterizeStage->RasterizeTriangle(*it);
	}
	mRasterizeStage->ImageWrite();
}

void SoftRenderer::OpenDepthTest()
{
	//InitDepthBuffer();
}

void SoftRenderer::SetAspect(float aspect)
{
	mAspect = aspect;
}

RasterizeStage & SoftRenderer::GetRasterizor()
{
	return *mRasterizeStage;
}

void SoftRenderer::UpdateVBO(const VBO & vbo)
{
	VBO::const_iterator it = vbo.begin();
	VBO::const_iterator end = vbo.end();
	for (; it != end; ++it) {
		mVBO.push_back(*it);
	}
}

void SoftRenderer::UpdateIBO(const IBO & ibo)
{
	IBO::const_iterator it = ibo.begin();
	IBO::const_iterator end = ibo.end();
	for (; it != end; ++it) {
		mIBO.push_back(*it);
	}
}

void SoftRenderer::RenderScene(Scene &scene)
{
	//渲染全局设置
	//TODO
	mGeometryPipeline->SetViewMatrix(scene.GetCamera()->GetViewMatrix());
	mGeometryPipeline->SetProjectionMatrix(scene.GetCamera()->GetProjectionMatrix());

	mRasterizeStage->SetLight(scene.GetLight());

	//计算可见性集合PVS
	PVS pvs;
	scene.GetPVS(pvs);
	//提交PVS
	PVS::iterator it = pvs.begin();
	PVS::iterator end = pvs.end();
	for (; it != end; ++it) {		
		mGeometryPipeline->SetModelMatrix((*it)->GetWorldTransform());
		UpdateVBO((*it)->GetMesh()->GetVBO());
		UpdateIBO((*it)->GetMesh()->GetIBO());
		mRasterizeStage->SetSampler((*it)->GetMaterial()->GetMainSampler());
		mRasterizeStage->SetNormalSampler((*it)->GetMaterial()->GetNormalSampler());
		mRasterizeStage->SetMaterial((*it)->GetMaterial());
		DrawTriangle();
	}
}
