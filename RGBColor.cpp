#include "RGBColor.h"
RGBColor::RGBColor() {
	mRGB[0] = 0.0;
	mRGB[1] = 0.0;
	mRGB[2] = 0.0;
	mRGB[3] = 0.0;
}

RGBColor::~RGBColor() {

}

RGBColor::RGBColor(float r, float g, float b,float a) {
	mRGB[0] = r;
	mRGB[1] = g;
	mRGB[2] = b;
	mRGB[3] = a;
}

float & const RGBColor::operator [](int i) {
	return mRGB[i];
}

void RGBColor::saturate()
{
	for (int i = 0; i < 3; ++i) {
		mRGB[i] = (mRGB[i] >= 0.0) ? mRGB[i] : 0.0;
		mRGB[i] = (mRGB[i] <= 1.0) ? mRGB[i] : 1.0;
	}
}

void RGBColor::Pow(float p)
{
	mRGB[0] = powf(mRGB[0], p);
	mRGB[1] = powf(mRGB[1], p);
	mRGB[2] = powf(mRGB[2], p);
}

RGBColor RGBColor::operator +( RGBColor &B) {
	return RGBColor(mRGB[0] + B[0], mRGB[1] + B[1], mRGB[2] + B[2],mRGB[3]+B[3]);
}

RGBColor RGBColor::operator*(float b)
{
	return RGBColor(mRGB[0] * b, mRGB[1] * b, mRGB[2] * b, mRGB[3]);
}

RGBColor RGBColor::operator/(float b)
{
	return RGBColor(mRGB[0] / b, mRGB[1] / b, mRGB[2] / b, mRGB[3] / b);
}

RGBColor RGBColor::operator*(RGBColor &B) {
	return RGBColor(mRGB[0] * B[0], mRGB[1] * B[1], mRGB[2] * B[2], mRGB[3] * B[3]);
}