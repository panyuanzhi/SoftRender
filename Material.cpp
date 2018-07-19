#include "Material.h"

Material::Material()
{
}

Material::~Material()
{
}

Material::Material(float r, float g, float b, float a)
{
	RGBColor color(r, g, b, a);
	mCdiff = color;
}

void Material::SetMainTexture(SamplerPtr sampler)
{
	mMainTexture = sampler;
}

Material::SamplerPtr Material::GetMainSampler()
{
	return mMainTexture;
}

void Material::SetNormalTexture(SamplerPtr sampler)
{
	mNormalTexture = sampler;
}

Material::SamplerPtr Material::GetNormalSampler()
{
	return mNormalTexture;
}
