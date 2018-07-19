#include "Sampler.h"
#include <math.h>
#include <string>
Sampler::Sampler():mSamplerType(SampleType::Linear)
{
	mpTexture = ITK_NULLPTR;
	InitPermutation();
}

Sampler::~Sampler()
{
}

void Sampler::SetTexture(ImageType::Pointer pTexture)
{
	if (mpTexture != ITK_NULLPTR) {
		mpTexture->ReleaseData();
		mpTexture->UnRegister();
	}

	ImageType::RegionType region = pTexture->GetBufferedRegion();
	ImageType::IndexType start;
	start[0] = 0;
	start[1] = 0;
	ImageType::SizeType size = region.GetSize();
	mpTexture = ImageType::New();
	mpTexture->SetRegions(region);
	mpTexture->Allocate();
	for(int i=0;i<size[0];++i)
		for (int j = 0; j < size[1]; ++j) {
			ImageType::IndexType index = { i,j };
			ImagePixelType pixel = pTexture->GetPixel(index);			
			mpTexture->SetPixel(index, pixel);
		}
}

Sampler::ImagePixelType Sampler::texture2D(float u, float v)
{
	ImagePixelType pixel;
	switch (mSamplerType)
	{
	case SampleType::Nearest:
		pixel=NearestSample(u, v, mpTexture);
		break;
	case SampleType::Linear:
		pixel = BilinearSample(u, v, mpTexture);
	default:
		break;
	}
	return pixel;
}

Sampler::ImagePixelType Sampler::MipmapLookAt(float u, float v, float d)
{
	float dfloor = floorf(d);
	float dupper = dfloor + 1;
	d = (d - dfloor > 0.5) ? dupper : dfloor;
	int level = mMipmaps.size();
	--level;
	if (d > level) {
		d = level;
		//dfloor = d;
		//dupper = d;
	}
	
	//ImagePixelType pixeld = NearestSample(u, v, mMipmaps[dfloor]);
	//ImagePixelType pixeld1= NearestSample(u, v, mMipmaps[dupper]);
	ImagePixelType pixel = NearestSample(u, v, mMipmaps[d]);
	//float fraction = d - dfloor;
	//ImagePixelType pixel;
	//pixel[0] = (1 - fraction)*pixeld[0] + fraction*pixeld1[0];
	//pixel[1] = (1 - fraction)*pixeld[1] + fraction*pixeld1[1];
	//pixel[2] = (1 - fraction)*pixeld[2] + fraction*pixeld1[2];
	return pixel;
}

void Sampler::GenerateMipmap()
{
	mMipmaps.push_back(mpTexture);
	ImageType::SizeType parentSize = mMipmaps[0]->GetBufferedRegion().GetSize();
	ImageType::SizeType curSize;
	curSize[0] = parentSize[0] / 3;
	curSize[1] = parentSize[1] / 3;
	for (int k = 1; curSize[0] > 0 && curSize[1] > 0; ++k) {
		ImageType::Pointer texture = ImageType::New();
		ImageType::IndexType start;
		start[0] = 0;
		start[1] = 0;
		ImageType::RegionType region;
		region.SetIndex(start);
		region.SetSize(curSize);
		texture->SetRegions(region);
		texture->Allocate();
		mMipmaps.push_back(texture);
		for (int j = 0; j<curSize[1]; ++j)
			for (int i = 0; i < curSize[0]; ++i) {
				float u = ((float)i + 0.5) / curSize[0];
				float v = ((float)j + 0.5) / curSize[1];

				ImagePixelType pixel = NearestSample(u, v, mMipmaps[k - 1]);
				ImageType::IndexType index = { i,j };
				mMipmaps[k]->SetPixel(index, pixel);
			}
		typedef itk::ImageFileWriter< ImageType > WriterType;
		WriterType::Pointer writer = WriterType::New();
		std::string  file = "mipmap";
		char c = '0' + k;
		file = file + c + ".png";
		writer->SetFileName(file);
		writer->SetInput(mMipmaps[k]);
		writer->Update();

		curSize[0] = curSize[0] / 3;
		curSize[1] = curSize[1] / 3;
	}
	
}

void Sampler::GenerateCheckerBoard()
{
	mpTexture = ImageType::New();
	ImageType::IndexType start;
	start[0] = 0;
	start[1] = 0;
	ImageType::SizeType size;
	size[0] = 64;
	size[1] = 64;
	ImageType::RegionType region;
	region.SetIndex(start);
	region.SetSize(size);
	mpTexture->SetRegions(region);
	mpTexture->Allocate(true);
	ImagePixelType black,white;
	black.Fill(0); white.Fill(255);

	for(int j=0;j<size[1];j+=2)
		for (int i = 0; i < size[0]; i += 2) {
			ImageType::IndexType index00 = { i+0,j+0 };
			ImageType::IndexType index10 = { i + 1,j+0 };
			ImageType::IndexType index01 = { i+0,j+1 };			
			ImageType::IndexType index11 = { i+1,j+1 };
			mpTexture->SetPixel(index00, white);
			mpTexture->SetPixel(index01, black);
			mpTexture->SetPixel(index10, black);
			mpTexture->SetPixel(index11, white);
		}
	//typedef itk::ImageFileWriter< ImageType > WriterType;
	//WriterType::Pointer writer = WriterType::New();
	//writer->SetFileName("checkboard.png");
	//writer->SetInput(mpTexture);
	//writer->Update();
}

Sampler::ImageType::SizeType Sampler::GetTextureSize()
{
	return mpTexture->GetBufferedRegion().GetSize();
}

float Sampler::GetAspect()
{
	ImageType::SizeType size = GetTextureSize();
	return (float)size[0]/(float)size[1];
}

void Sampler::SetNearestSample()
{
	mSamplerType = SampleType::Nearest;
}

void Sampler::SetLinearSample()
{
	mSamplerType = SampleType::Linear;
}

float Sampler::PerlinNoise3D(float x, float y, float z)
{
	int xi0 = ((int)std::floor(x)) & mTableSizeMask;
	int yi0 = ((int)std::floor(y)) & mTableSizeMask;
	int zi0 = ((int)std::floor(z)) & mTableSizeMask;

	int xi1 = (xi0 + 1) & mTableSizeMask;
	int yi1 = (yi0 + 1) & mTableSizeMask;
	int zi1 = (zi0 + 1) & mTableSizeMask;

	float tx = x - std::floor(x);
	float ty = y - std::floor(y);
	float tz = z - std::floor(z);

	float u = EaseCurve(tx);
	float v = EaseCurve(ty);
	float w = EaseCurve(tz);

	float x0 = tx, x1 = tx - 1;
	float y0 = ty, y1 = ty - 1;
	float z0 = tz, z1 = tz - 1;

	float a = GradientDotV(Hash(xi0, yi0, zi0), x0, y0, z0);
	float b = GradientDotV(Hash(xi1, yi0, zi0), x1, y0, z0);
	float c = GradientDotV(Hash(xi0, yi1, zi0), x0, y1, z0);
	float d = GradientDotV(Hash(xi1, yi1, zi0), x1, y1, z0);
	float e = GradientDotV(Hash(xi0, yi0, zi1), x0, y0, z1);
	float f = GradientDotV(Hash(xi1, yi0, zi1), x1, y0, z1);
	float g = GradientDotV(Hash(xi0, yi1, zi1), x0, y1, z1);
	float h = GradientDotV(Hash(xi1, yi1, zi1), x1, y1, z1);

	float k0 = a;
	float k1 = (b - a);
	float k2 = (c - a);
	float k3 = (e - a);
	float k4 = (a + d - b - c);
	float k5 = (a + f - b - e);
	float k6 = (a + g - c - e);
	float k7 = (b + c + e + h - a - d - f - g);
	
	//把三线性插值展开了.
	float result= k0 + k1 * u + k2 * v + k3 * w + k4 * u * v + k5 * u * w + k6 * v * w + k7 * u * v * w;
	//把结果转换到[0,1]之间。
	return (result + 2) / 4;
}

float Sampler::TextureWrap(float x)
{
	return 0.0f;
}

Sampler::ImagePixelType Sampler::BilinearSample(float u, float v, ImageType::Pointer texture)
{
	//设置环绕模式为 重复
	u = u - floorf(u);
	v = v - floorf(v);

	ImageType::SizeType size = texture->GetBufferedRegion().GetSize();
	//转换纹理坐标空间
	u = u*(size[0]);
	v = v*(size[1]);

	float floor_u = floorf(u);
	float floor_v = floorf(v);

	float fraction_u = u - floor_u;
	float fraction_v = v - floor_v;

	float left = floor_u;
	float right = (left + 1 == size[0]) ? 0 : left + 1;
	float bottom = floor_v;
	float top = (bottom + 1 == size[1]) ? 0 : bottom + 1;

	ImageType::IndexType indexLB = { left,bottom };
	ImageType::IndexType indexRB = { right,bottom };
	ImageType::IndexType indexLT = { left,top };
	ImageType::IndexType indexRT = { right,top };

	ImagePixelType pixelLB = texture->GetPixel(indexLB);
	ImagePixelType pixelRB = texture->GetPixel(indexRB);
	ImagePixelType pixelLT = texture->GetPixel(indexLT);
	ImagePixelType pixelRT = texture->GetPixel(indexRT);

	ImageType::PixelType pixel;
	pixel[0] = (1 - fraction_u)*(1 - fraction_v)*pixelLB[0]
		+ fraction_u*(1 - fraction_v)*pixelRB[0]
		+ (1 - fraction_u)*fraction_v*pixelLT[0]
		+ fraction_u*fraction_v*pixelRT[0];
	pixel[1] = (1 - fraction_u)*(1 - fraction_v)*pixelLB[1]
		+ fraction_u*(1 - fraction_v)*pixelRB[1]
		+ (1 - fraction_u)*fraction_v*pixelLT[1]
		+ fraction_u*fraction_v*pixelRT[1];
	pixel[2] = (1 - fraction_u)*(1 - fraction_v)*pixelLB[2]
		+ fraction_u*(1 - fraction_v)*pixelRB[2]
		+ (1 - fraction_u)*fraction_v*pixelLT[2]
		+ fraction_u*fraction_v*pixelRT[2];

	return pixel;
}

Sampler::ImagePixelType Sampler::NearestSample(float u, float v, ImageType::Pointer texture)
{
	//设置环绕模式为 重复
	u = u - floorf(u);
	v = v - floorf(v);

	ImageType::SizeType size = texture->GetBufferedRegion().GetSize();
	//转换纹理坐标空间
	u = u*(size[0]);
	v = v*(size[1]);

	float floor_u = floorf(u);
	float floor_v = floorf(v);

	float fraction_u = u - floor_u;
	float fraction_v = v - floor_v;

	u = (fraction_u < 0.5) ? floor_u : floor_u + 1;
	v = (fraction_v < 0.5) ? floor_v : floor_v + 1;
	u = (u == size[0]) ? 0 : u;
	v = (v == size[1]) ? 0 : v;
	ImageType::IndexType index = { u,v };
	int x = u;
	int y = v;
	if ((x % 2 == 1 && y % 2 == 0) || (x % 2 == 0 && y % 2 == 1)) {
		int z = 0;
	}
	return texture->GetPixel(index);
}

void Sampler::InitPermutation()
{
	int length = 512;
	for (int i = 0; i < length; ++i) {
		mPermutation[i] = mPm[i % 256];
	}
}

int Sampler::Hash(int x, int y, int z)
{
	return mPermutation[mPermutation[mPermutation[x] + y] + z];
}

float Sampler::GradientDotV(int hash, float x, float y, float z)
{
	switch (hash & 0xF) {
	case  0: return  x + y; // gradient=(1,1,0) v=(x,y,z) v*g=x+y.
	case  1: return -x + y; // (-1,1,0) 
	case  2: return  x - y; // (1,-1,0) 
	case  3: return -x - y; // (-1,-1,0) 
	case  4: return  x + z; // (1,0,1) 
	case  5: return -x + z; // (-1,0,1) 
	case  6: return  x - z; // (1,0,-1) 
	case  7: return -x - z; // (-1,0,-1) 
	case  8: return  y + z; // (0,1,1), 
	case  9: return -y + z; // (0,-1,1), 
	case 10: return  y - z; // (0,1,-1), 
	case 11: return -y - z; // (0,-1,-1) 
	case 12: return  y + x; // (1,1,0) 
	case 13: return -x + y; // (-1,1,0) 
	case 14: return -y + z; // (0,-1,1) 
	case 15: return -y - z; // (0,-1,-1) 
	}
	return 0.0f;
}

float Sampler::EaseCurve(float t)
{
	return 6 * std::powf(t, 5) - 15 * std::powf(t, 4) + 10 * std::powf(t, 3);
}

void Sampler::ReadTexture(std::string filename)
{
	typedef itk::ImageFileReader<ImageType> ImageReaderType;
	ImageReaderType::Pointer imageReader = ImageReaderType::New();
	imageReader->SetFileName(filename);
	imageReader->Update();
	mpTexture=imageReader->GetOutput();
}
