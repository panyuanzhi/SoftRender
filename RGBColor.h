#ifndef RGBColor_H
#define RGBColor_H
#include <math.h>
class RGBColor {
public:
	RGBColor();
	~RGBColor();
	RGBColor(float r, float g, float b, float a);
	RGBColor operator *(RGBColor &B);
	RGBColor operator +(RGBColor &B);
	RGBColor operator *(float b);
	RGBColor operator /(float b);
	//如果只是浅拷贝的话，赋值运算符不需要重载。
	//operator =
	float& const operator  [](int i);
	void saturate();
	void Pow(float p);
private:
	float mRGB[4];
};
#endif // !RGBColor_H



