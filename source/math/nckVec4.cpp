
/**
 * NCtoolKit © 2007-2015 Lu’s F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckVec4.h"
#include "nckVec3.h"
#include "nckMat44.h"
#include <sstream>

_MATH_BEGIN

Vec4::Vec4(){
	m_X = 0;m_Y = 0;m_Z = 0;m_W = 0;
}

Vec4::Vec4(float x,float y,float z,float w){
	m_X = x;m_Y = y;m_Z = z;m_W = w;
}

Vec4::Vec4(const Vec3 & vec, float w ){
	m_X = vec.GetX();
	m_Y = vec.GetY();
	m_Z = vec.GetZ();
	m_W = w;
}

Vec4 & Vec4::operator = (const Vec4 & vec)
{
	m_X = vec.m_X;
	m_Y = vec.m_Y;
	m_Z = vec.m_Z;
	m_W = vec.m_W;
	return *this;
}

Vec4 & Vec4::operator += (const Vec4 & vec){
	m_X += vec.m_X;
	m_Y += vec.m_Y;
	m_Z += vec.m_Z;
	m_W += vec.m_W;
	return *this;
}

Vec4 & Vec4::operator -= (const Vec4 & vec){
	m_X -= vec.m_X;
	m_Y -= vec.m_Y;
	m_Z -= vec.m_Z;
	m_W -= vec.m_W;
	return *this;
}

Vec4 & Vec4::operator *= (float val){
	m_X *= val;
	m_Y *= val;
	m_Z *= val;
	m_W *= val;
	return *this;
}


Vec4 Vec4::operator -() const{
	return Vec4(-m_X,-m_Y,-m_Z,-m_W);
}

Vec4 operator + (const Vec4 & vecA, const Vec4 & vecB){
	return Vec4(vecA.m_X+vecB.m_X,vecA.m_Y+vecB.m_Y,vecA.m_Z+vecB.m_Z,vecA.m_W+vecB.m_W);
}

Vec4 operator * (const Vec4 & vecA, const Vec4 & vecB){
	return Vec4(vecA.m_X*vecB.m_X,vecA.m_Y*vecB.m_Y,vecA.m_Z*vecB.m_Z,vecA.m_W*vecB.m_W);
}

Vec4 operator - (const Vec4 & vecA, const Vec4 & vecB){
	return Vec4(vecA.m_X-vecB.m_X,vecA.m_Y-vecB.m_Y,vecA.m_Z-vecB.m_Z,vecA.m_W-vecB.m_W);
}

Vec4 operator * (const Vec4 & vecA, float value){
	return Vec4(vecA.m_X*value,vecA.m_Y*value,vecA.m_Z*value,vecA.m_W*value);
}

Vec4 operator / (const Vec4 & vecA, float value){
	return Vec4(vecA.m_X/value,vecA.m_Y/value,vecA.m_Z/value,vecA.m_W/value);
}

Vec4 operator * (float value, const Vec4 & vecA){
	return Vec4(vecA.m_X*value,vecA.m_Y*value,vecA.m_Z*value,vecA.m_W*value);
}

Vec4 operator / (float value, const Vec4 & vecA){
	return Vec4(value/vecA.m_X,value/vecA.m_Y,value/vecA.m_Z,value/vecA.m_W);
}

float Dot(const Vec4 & vecA, const Vec4 & vecB){
	return vecA.m_X* vecB.m_X + vecA.m_Y* vecB.m_Y + vecA.m_Z* vecB.m_Z+ vecA.m_W* vecB.m_W;
}

float Angle(const Vec4 & vecA, const Vec4 & vecB){
	return acosf(Dot(vecA,vecB));
}

float Vec4::Length() const{
	return sqrtf(m_X * m_X + m_Y * m_Y + m_Z * m_Z + m_W * m_W);
}

Vec4 Normalize(const Vec4 & vec){
	float l = vec.Length();

	if(l==0.0)
		return Vec4();

	l = 1.0f/l;

	return Vec4(vec.m_X*l,vec.m_Y*l,vec.m_Z*l,vec.m_W*l);
}

Vec4 BezierInterpolation(const Vec4 & p0,const Vec4 & p1,const Vec4 & p2,
							const Vec4 & p3,float t){
	if(t<0.0f)
		return p0;
	if(t>1.0f)
		return p3;

	// http://en.wikipedia.org/wiki/Bézier_curve
	float t1 = powf(1.0f-t,3.0f);
	float t2 = 3.0f*powf(1.0f-t,2.0f);
	float t3 = 3.0f*(1.0f-t)*powf(t,2.0f);
	float t4 = powf(t,3.0f);

	return p0*t1 + p1*t2 + p2*t3 + p3*t4;
}

Vec4 LinearInterpolation(const Vec4 & p0,const Vec4 & p1,float t){
	if(t<0.0f)
		return p0;
	if(t>1.0f)
		return p1;
	return p0*(1.0f-t) + p1*t;
}

std::string Vec4::GetString(){
	std::stringstream sstream;
	sstream << m_X << m_Y << m_Z << m_W;
	return sstream.str();
}

_MATH_END
