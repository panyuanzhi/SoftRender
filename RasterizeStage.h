#ifndef RasterizeStage_H
#define RasterizeStage_H

#include "RGBColor.h"
#include "Sampler.h"
#include "Material.h"
#include <string>
#include "Traits.h"
#include "Light.h"
struct FrameBuffer {
	std::unique_ptr<float[]> DepthBuffer;
	std::unique_ptr<RGBColor[]> FragmentBuffer;
};
class RasterizeStage
{
public:
	typedef typename MeshTraits::ImagePixelType ImagePixelType;
	typedef typename MeshTraits::ImageType ImageType;
	typedef typename MathTraits::Point4 Point4;
	typedef typename MathTraits::Vector3 Vector3;
	typedef typename MathTraits::Matrix3 Matrix3;
	typedef typename RenderTraits::V2FBuffer V2FBuffer;
	typedef typename RenderTraits::IBO IBO;

	RasterizeStage(IBO &ibo, V2FBuffer &v2fBuffer);
	~RasterizeStage();
	void Init(int w, int h,std::string filename);
	int GetBufferWidth();
	int GetBufferHeight();
	void RasterizeLine(int x0, int y0, int x1, int y1);
	void RasterizeTriangle(TriangleIndex triIndex);
	void ImageWrite();
	void SetSampler(std::shared_ptr<Sampler> sampler);
	void SetMaterial(std::shared_ptr<Material> material);
	void SetNormalSampler(std::shared_ptr<Sampler> sampler);
	void SetLight(std::shared_ptr<Light> light);
private:
	void DrawPixel(int x, int y, RGBColor color);
	void DrawLine(int x0, int y0, int x1, int y1);
	void BoundingBox(TriangleIndex triIndex, int &xMin, int &xMax, int &yMin, int &yMax);
	void AttributionInterpolate(int x, int y, TriangleIndex triIndex,int m,int n);
	void Sample(int x, int y, int num, TriangleIndex triIndex, int m, int n);
	void GenSamplePattern();
	float GetDepthBuffer(int x, int y,int i);
	void SetDepthBuffer(int x, int y, int i,float value);
	void SetFragment(int x, int y, int i, RGBColor &frag);
	const RGBColor& GetFragement(int x, int y, int i);
	RGBColor PixelShader(const V2F &v2f , int x, int y);
	bool TopLeftRule(Vector3 edge);
	void InitFrameBuffer();
	RGBColor GetAccumulationBuffer(int x, int y);
private:
	ImageType::Pointer mColorBuffer;
	int mBufferWidth;
	int mBufferHeight;
	int mBlockSize;
	Point4 mSamplePattern[4];
	int mSampleNumber;
	V2F mPerspective_Correct[2][2];
	std::string mOutputFileName;
	std::shared_ptr<Sampler>  mSampler;
	std::shared_ptr<Material>  mMaterial;
	std::shared_ptr<Sampler> mNormalSampler;
	std::unique_ptr<FrameBuffer[]>  mFrameBuffer;
	IBO &mIBO;
	V2FBuffer &mV2FBuffer;
	std::shared_ptr<Light> mLight;
};


#endif // !RasterizeStage_H
