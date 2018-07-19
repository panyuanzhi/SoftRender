#include "RasterizeStage.h"

RasterizeStage::RasterizeStage(IBO &ibo,V2FBuffer &v2fBuffer)
	:mIBO(ibo),mV2FBuffer(v2fBuffer)
{
	mBlockSize = 2;
	GenSamplePattern();
}

RasterizeStage::~RasterizeStage()
{
}

void RasterizeStage::Init(int w, int h, std::string filename)
{
	mColorBuffer = ImageType::New();
	ImageType::IndexType start;
	start[0] = 0;
	start[1] = 0;
	ImageType::SizeType size;
	size[0] = w;
	size[1] = h;
	mBufferWidth = w;
	mBufferHeight = h;
	ImageType::RegionType region;
	region.SetSize(size);
	region.SetIndex(start);
	mColorBuffer->SetRegions(region);
	mColorBuffer->Allocate(true);
	InitFrameBuffer();
	mOutputFileName = filename;
}

int RasterizeStage::GetBufferWidth()
{
	return mBufferWidth;
}

int RasterizeStage::GetBufferHeight()
{
	return mBufferHeight;
}

void RasterizeStage::RasterizeLine(int x0, int y0, int x1, int y1)
{
	int midX = (x0 + x1) / 2;
	int midY = (y0 + y1) / 2;
	DrawLine(midX, midY, x0, y0);
	DrawLine(midX, midY, x1, y1);
}

void RasterizeStage::RasterizeTriangle(TriangleIndex triIndex)
{
	int xMin, xMax, yMin, yMax;
	BoundingBox(triIndex, xMin, xMax, yMin, yMax);
	//采样范围为[xMin,xMax)左闭右开
	for (int y = yMin; y<yMax; y += mBlockSize)
		for (int x = xMin; x < xMax; x += mBlockSize) {
			for (int j = 0; j<mBlockSize; ++j)
				for (int i = 0; i < mBlockSize; ++i) {
					AttributionInterpolate(x + i, y + j, triIndex,i,j);
				}
			for (int j = 0; j<mBlockSize; ++j)
				for (int i = 0; i < mBlockSize; ++i) {
					for (int n = 0; n < mSampleNumber; ++n) {
						Sample(x + i, y + j, n, triIndex,i,j);
					}
					RGBColor pixel = GetAccumulationBuffer(x + i, y + j);
					DrawPixel(x + i, y + j, pixel);
				}

		}
}

void RasterizeStage::ImageWrite()
{
	typedef itk::ImageFileWriter< ImageType > WriterType;
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(mOutputFileName);
	writer->SetInput(mColorBuffer);
	try
	{
		writer->Update();
	}
	catch (itk::ExceptionObject & error)
	{
		std::cerr << "Error: " << error << std::endl;
	}
}

void RasterizeStage::SetSampler(std::shared_ptr<Sampler> sampler)
{
	mSampler = sampler;
}

void RasterizeStage::SetMaterial(std::shared_ptr<Material> material)
{
	mMaterial = material;
}

void RasterizeStage::SetNormalSampler(std::shared_ptr<Sampler> sampler)
{
	mNormalSampler = sampler;
}

void RasterizeStage::SetLight(std::shared_ptr<Light> light)
{
	mLight = light;
}

void RasterizeStage::DrawPixel(int x, int y, RGBColor color)
{
	ImageType::IndexType pixelIndex = { x,y };
	color.saturate();
	ImagePixelType pixel;
	pixel[0] = itk::NumericTraits<ImagePixelType::ValueType>::max()*color[0];
	pixel[1] = itk::NumericTraits<ImagePixelType::ValueType>::max()*color[1];
	pixel[2] = itk::NumericTraits<ImagePixelType::ValueType>::max()*color[2];
	ImagePixelType origin = mColorBuffer->GetPixel(pixelIndex);
	ImagePixelType blend;
	blend[0] = pixel[0] * color[3] + origin[0] * (1 - color[3]);
	blend[1] = pixel[1] * color[3] + origin[1] * (1 - color[3]);
	blend[2] = pixel[2] * color[3] + origin[2] * (1 - color[3]);

	mColorBuffer->SetPixel(pixelIndex, blend);
}

void RasterizeStage::DrawLine(int x0, int y0, int x1, int y1)
{
	int x = x0, y = y0;
	//dx dy表示x,y的方向，即在x,y方向上是增加的还是减少的.
	int dx = x1 - x0, dy = y1 - y0;
	int sx, sy;
	if (dx > 0) sx = 1;
	else if (dx < 0) {
		sx = -1;
		dx = -dx;
	}
	else sx = 0;
	if (dy > 0) sy = 1;
	else if (dy < 0) {
		sy = -1;
		dy = -dy;
	}
	else sy = 0;

	int ax = 2 * dx, ay = 2 * dy;
	RGBColor pixel(1, 1, 1, 1);
	if (dx >= dy) {
		for (int decy = ay - dx;/**/; x += sx, decy += ay) {
			DrawPixel(x, y, pixel);

			if (x == x1)break;
			if (decy >= 0) {
				decy -= ax;
				y += sy;
			}
		}
	}
	else {
		for (int decx = ax - dy;/**/; y += sy, decx += ax) {
			DrawPixel(x, y, pixel);

			if (y == y1)break;
			if (decx >= 0) {
				decx -= ay;
				x += sx;
			}
		}
	}
}

void RasterizeStage::BoundingBox(TriangleIndex triIndex, int & xMin, int & xMax, int & yMin, int & yMax)
{
	float fXMin = std::numeric_limits<float>::max();
	float fXMax = std::numeric_limits<float>::min();
	float fYMax = std::numeric_limits<float>::min();
	float fYMin = std::numeric_limits<float>::max();

	for (int i=0; i<3; ++i) {
		fXMin = (mV2FBuffer[triIndex.v[i]].point[0] < fXMin) ? mV2FBuffer[triIndex.v[i]].point[0] : fXMin;
		fXMax = (mV2FBuffer[triIndex.v[i]].point[0] > fXMax) ? mV2FBuffer[triIndex.v[i]].point[0] : fXMax;
		fYMin = (mV2FBuffer[triIndex.v[i]].point[1] < fYMin) ? mV2FBuffer[triIndex.v[i]].point[1] : fYMin;
		fYMax = (mV2FBuffer[triIndex.v[i]].point[1] > fYMax) ? mV2FBuffer[triIndex.v[i]].point[1] : fYMax;
	}
	xMin = floorf(fXMin);
	yMin = floorf(fYMin);
	xMax = ceilf(fXMax);
	yMax = ceilf(fYMax);
}

void RasterizeStage::AttributionInterpolate(int x, int y, TriangleIndex triIndex, int m, int n)
{
	itk::Matrix<float, 3, 3> A;
	for (int i = 0; i < 3; ++i) {
		Point4 point =mV2FBuffer[triIndex.v[i]].point;
		A[0][i] = point[0];
		A[1][i] = point[1];
		A[2][i] = 1;
	}
	//进行着色采样
	Point4 shadingSample;
	shadingSample[0] = float(x) + 0.5; shadingSample[1] = float(y) + 0.5;
	itk::Vector<float, 3> b;
	b[0] = shadingSample[0]; b[1] = shadingSample[1]; b[2] = 1.0;
	vnl_vector<float> X;
	X = A.GetInverse()*b.GetVnlVector();
	float alpha = X[0], beta = X[1], gamma = X[2];
	V2F v2f = mV2FBuffer[triIndex.v[0]] * X[0] + mV2FBuffer[triIndex.v[1]] * X[1] + mV2FBuffer[triIndex.v[2]] * X[2];

	//perpstive-correct
	v2f = v2f*(1.0 / v2f.one);
	mPerspective_Correct[m][n] = v2f;
}

void RasterizeStage::Sample(int x, int y, int num, TriangleIndex triIndex, int m, int n)
{
	//对于任意一点，有v=alpha*v0+beta*v1+gamma*v2	alpha+beta+gamma=1
	//可以写成矩阵形式[v0 v1 v2][alpha beta gamma]T=v 
	//				  [1   1  1]
	//根据矩阵形式可以方便的求解alpha beta gamma
	itk::Matrix<float, 3, 3> A;
	for (int i = 0; i < 3; ++i) {
		Point4 point= mV2FBuffer[triIndex.v[i]].point;
		A[0][i] = point[0];
		A[1][i] = point[1];
		A[2][i] = 1;
	}
	Vector3 edge01, edge12, edge20;
	edge01[0] = A[0][1] - A[0][0]; edge01[1] = A[1][1] - A[1][0];
	edge12[0] = A[0][2] - A[0][1]; edge12[1] = A[1][2] - A[1][1];
	edge20[0] = A[0][0] - A[0][2]; edge20[1] = A[1][0] - A[1][2];
	//alpha beta gamma  为x[0] [1] [2]
	vnl_vector<float> X;
	int hits = 0;
	Point4 geometrySample;
	geometrySample[0] = float(x) + mSamplePattern[num][0];
	geometrySample[1] = float(y) + mSamplePattern[num][1];
	itk::Vector<float, 3> b;
	b[0] = geometrySample[0]; b[1] = geometrySample[1]; b[2] = 1;
	X = A.GetInverse()*b.GetVnlVector();
	
	if ((X[0] > 0 || (X[0] == 0 && TopLeftRule(edge12)))
		&& (X[1] > 0 || (X[1] == 0 && TopLeftRule(edge20)))
		&& (X[2] > 0 || (X[2] == 0 && TopLeftRule(edge01)))) {
		RGBColor frag;
		if (num == 0) {			
			frag = PixelShader(mPerspective_Correct[m][n], m, n);
		}
		else {
			frag = GetFragement(x, y, 0);
			
		}
		V2F v2f = mV2FBuffer[triIndex.v[0]] * X[0] + mV2FBuffer[triIndex.v[1]] * X[1] + mV2FBuffer[triIndex.v[2]] * X[2];
		float depth = v2f.point[2] / v2f.point[3];
		if (depth < GetDepthBuffer(x, y, num)) {
			SetDepthBuffer(x, y, num, depth);
			SetFragment(x, y, num, frag);
		}
	}
}

void RasterizeStage::GenSamplePattern()
{
	mSampleNumber = 4;

	mSamplePattern[0][0] = 0; mSamplePattern[0][1] = 0.25;
	mSamplePattern[1][0] = 0.5; mSamplePattern[1][1] = 0;
	mSamplePattern[2][0] = 0.75; mSamplePattern[2][1] = 0.5;
	mSamplePattern[3][0] = 0.25; mSamplePattern[3][1] = 0.75;
}

float RasterizeStage::GetDepthBuffer(int x, int y,int i)
{
	return mFrameBuffer[i].DepthBuffer[y*mBufferWidth + x];
}

void RasterizeStage::SetDepthBuffer(int x, int y,int i, float value)
{
	mFrameBuffer[i].DepthBuffer[y*mBufferWidth + x] = value;
}

void RasterizeStage::SetFragment(int x, int y, int i, RGBColor & frag)
{
	mFrameBuffer[i].FragmentBuffer[y*mBufferWidth + x] = frag;
}

const RGBColor & RasterizeStage::GetFragement(int x, int y, int i)
{
	return mFrameBuffer[i].FragmentBuffer[y*mBufferWidth + x];
}

RGBColor RasterizeStage::PixelShader(const V2F &v2f, int x, int y)
{
	float dudx, dvdx, dudy, dvdy;
	dudx = (mPerspective_Correct[y][1].uv[0] - mPerspective_Correct[y][0].uv[0])*mBufferWidth;
	dvdx = (mPerspective_Correct[y][1].uv[1] - mPerspective_Correct[y][0].uv[1])*mBufferHeight;
	dudy = (mPerspective_Correct[1][x].uv[0] - mPerspective_Correct[0][x].uv[0])*mBufferWidth;
	dvdy = (mPerspective_Correct[1][x].uv[1] - mPerspective_Correct[0][x].uv[1])*mBufferHeight;

	//float d = log2(max(sqrt(dudx * dudx + dvdx * dvdx), sqrt(dudy * dudy + dvdy * dvdy)));
	float mx = sqrtf(dudx*dudx + dvdx*dvdx);
	float my = sqrtf(dudy*dudy + dvdy*dvdy);
	float max = (mx >= my) ? mx : my;
	float d = log10f(max)/log10f(6);

	//ImageType::SizeType size = mSampler->GetTextureSize();
	//float factorx = (float)mPolyWidth / size[0];
	//float factory = (float)mPloyHeight / size[1];
	//float factor = (factorx > factory) ? factorx : factory;
	//float d = 1;
	//ImagePixelType pixel = mSampler->texture2D(v2f.uv[0], v2f.uv[1]);
	//ImagePixelType pixel = mSampler->MipmapLookAt(v2f.uv[0], v2f.uv[1], d);

	ImagePixelType normalMap = mNormalSampler->texture2D(v2f.uv[0], v2f.uv[1]);
	Vector3 normal;
	normal[0] = ((float)normalMap[0]) / ((float)itk::NumericTraits<unsigned char>::max());
	normal[1] = ((float)normalMap[1]) / ((float)itk::NumericTraits<unsigned char>::max());
	normal[2] = ((float)normalMap[2]) / ((float)itk::NumericTraits<unsigned char>::max());
	normal = normal * 2 - 1;

	Matrix3 TBN;
	TBN[0][0] = v2f.tangent[0]; TBN[0][1] = v2f.tangent[1]; TBN[0][2] = v2f.tangent[2];
	TBN[1][0] = v2f.bitangent[0]; TBN[1][1] = v2f.bitangent[1]; TBN[1][2] = v2f.bitangent[2];
	TBN[2][0] = v2f.normal[0]; TBN[2][1] = v2f.normal[1]; TBN[2][2] = v2f.normal[2];
	vnl_vector<float> vnlWorldNormal = TBN.GetInverse()*normal.GetVnlVector();
	Vector3 worldNormal;
	worldNormal[0] = vnlWorldNormal[0]; worldNormal[1] = vnlWorldNormal[1]; worldNormal[2] = vnlWorldNormal[2];
	worldNormal.Normalize();
	RGBColor Cdiff;
	//Cdiff[0] = ((float)pixel[0]) / ((float)itk::NumericTraits<unsigned char>::max());
	//Cdiff[1] = ((float)pixel[1]) / ((float)itk::NumericTraits<unsigned char>::max());
	//Cdiff[2] = ((float)pixel[2]) / ((float)itk::NumericTraits<unsigned char>::max());
	//Cdiff[3] = mMaterial->mCdiff[3];

	Cdiff[0] = 1.0f;
	Cdiff[1] = 1.0f;
	Cdiff[2] = 1.0f;
	Cdiff[3] = mMaterial->mCdiff[3];

	Vector3 lightDir = mLight->GetDirection();
	float cosI = lightDir*worldNormal;
	cosI = std::max(cosI, 0.0f);
	//RGBColor color =Cdiff;
	RGBColor color = Cdiff*(mLight->GetIrradiance()*cosI);
	//gamma-correct
	float gamma = 2.2;
	color.Pow(1 / gamma);
	return color;
}

bool RasterizeStage::TopLeftRule(Vector3 edge)
{
	if (edge[1] == 0 && edge[0] < 0) {
		//上边
		return true;
	}
	else if (edge[1] < 0) {
		//左边
		return true;
	}
	else return false;
}

void RasterizeStage::InitFrameBuffer()
{
	mFrameBuffer.reset(new FrameBuffer[mSampleNumber]);
	for (int i = 0; i < mSampleNumber; ++i) {
		mFrameBuffer[i].DepthBuffer.reset(new float[mBufferWidth*mBufferHeight]);
		for (int j = 0; j < mBufferHeight; ++j)
			for (int k = 0; k < mBufferWidth; ++k)
				mFrameBuffer[i].DepthBuffer[j*mBufferWidth + k] = std::numeric_limits<float>::max();
		mFrameBuffer[i].FragmentBuffer.reset(new RGBColor[mBufferWidth*mBufferHeight]);
	}
}

RGBColor RasterizeStage::GetAccumulationBuffer(int x, int y)
{
	RGBColor color(0, 0, 0, 0);
	for (int i = 0; i < mSampleNumber; ++i) {
		RGBColor frag = GetFragement(x, y, i);
		color = color + frag;
	}
	return color/mSampleNumber;
}
