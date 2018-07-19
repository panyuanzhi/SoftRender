#ifndef Material_H
#define Material_H

#include "RGBColor.h"
#include "Sampler.h"
class Material
{
public:
	typedef typename std::shared_ptr<Sampler> SamplerPtr;
	Material();
	~Material();
	Material(float r, float g, float b, float a);
	RGBColor mCdiff;
	void SetMainTexture(SamplerPtr sampler);
	SamplerPtr GetMainSampler();
	void SetNormalTexture(SamplerPtr sampler);
	SamplerPtr GetNormalSampler();
private:
	SamplerPtr mMainTexture;
	SamplerPtr mNormalTexture;
};
#endif // !Material_H

#include "RGBColor.h"



