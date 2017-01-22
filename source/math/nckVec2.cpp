
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckVec3.h"
#include "nckVec2.h"
#include "nckVec4.h"
#include "nckMat44.h"
#include <sstream>
#include <math.h>

_MATH_BEGIN

Vec2::Vec2(){
	m_X = 0;
	m_Y = 0;
}

Vec2::Vec2(float x,float y){
	m_X = x;
	m_Y = y;
}

Vec2::Vec2(const float *farray){
	m_X = farray[0];
	m_Y = farray[1];
}

Vec2::Vec2(const Vec3 & vec){
	m_X = vec.GetX();
	m_Y = vec.GetY();
}

Vec2 & Vec2::operator = (const Vec2 & vec){
	m_X = vec.m_X;
	m_Y = vec.m_Y;
	return *this;
}	

Vec2 & Vec2::operator += (const Vec2 & vec){
	m_X += vec.m_X;
	m_Y += vec.m_Y;
	return *this;
}

Vec2 & Vec2::operator -= (const Vec2 & vec){
	m_X -= vec.m_X;
	m_Y -= vec.m_Y;
	return *this;
}

Vec2 & Vec2::operator *= (float vec){
	m_X *= vec;
	m_Y *= vec;
	return *this;
}

Vec2 operator + (const Vec2 & vecA, const Vec2 & vecB){
	return Vec2(vecA.m_X+vecB.m_X,vecA.m_Y+vecB.m_Y);
}

Vec2 operator * (const Vec2 & vecA, const Vec2 & vecB){
	return Vec2(vecA.m_X*vecB.m_X,vecA.m_Y*vecB.m_Y);
}

Vec2 operator - (const Vec2 & vecA, const Vec2 & vecB){
	return Vec2(vecA.m_X-vecB.m_X,vecA.m_Y-vecB.m_Y);
}

Vec2 operator * (const float value,const Vec2 & vecA){
	return Vec2(vecA.m_X*value,vecA.m_Y*value);
}

Vec2 operator * (const Vec2 & vecA, const float value){
	return Vec2(vecA.m_X*value,vecA.m_Y*value);
}

Vec2 operator / (const Vec2 & vecA, const float value){
	return Vec2(vecA.m_X/value,vecA.m_Y/value);
}

Vec2 operator / (const Vec2 & vecA, const Vec2 & vecB) {
    return Vec2(vecA.m_X / vecB.m_X, vecA.m_Y / vecB.m_Y);
}

Vec2 operator / (const float value,const Vec2 & vecA){
	return Vec2(value/vecA.m_X,value/vecA.m_Y);
}

float Dot(const Vec2 & vecA, const Vec2 & vecB){
	return vecA.m_X* vecB.m_X + vecA.m_Y* vecB.m_Y;
}

float Angle(const Vec2 & vecA, const Vec2 & vecB){
	return acosf(Dot(vecA,vecB));
}

float Vec2::Length() const{
	return sqrtf(m_X* m_X + m_Y*m_Y);
}

Vec2 Normalize(const Vec2 & vec){
	float l = vec.Length();

	if(l==0.0)
		return Vec2();

	l = 1.0f/l;

	return Vec2(vec.m_X*l,vec.m_Y*l);
}

Vec2 BezierInterpolation(const Vec2 & p0,const Vec2 & p1,const Vec2 & p2,
							const Vec2 & p3,float t){
	if(t<0.0f) return p0;
	if(t>1.0f) return p3;

	// http://en.wikipedia.org/wiki/B�zier_curve
	float t1 = powf(1.0f-t,3.0f);
	float t2 = 3.0f*powf(1.0f-t,2.0f)*t;
	float t3 = 3.0f*(1.0f-t)*powf(t,2.0f);
	float t4 = powf(t,3.0f);

	return p0*t1 + p1*t2 + p2*t3 + p3*t4;
}

Vec2 LinearInterpolation(const Vec2 & p0,const Vec2 & p1,float t){
	if(t<0.0f) return p0;
	if(t>1.0f) return p1;
	return p0*(t-1.0f) + p1*t;
}

Vec2 BezierInterpolation(const Mat44 & inv_mat,const Vec2 & p0,const Vec2 & p1,
							const Vec2 & p2,const Vec2 & p3, float t)
{
	if(t<0.0f) return p0;
	if(t>1.0f) return p3;

	// inv_mat = B Matrix, Mixture matrix.
	Vec4 U = Vec4(1.0,t, t*t, t*t*t);
	// A Matrix = UxB
	Vec4 A = U*inv_mat;
	// p(u) = UxBxP
	return p0*A.GetX() + p1*A.GetY() + p2*A.GetZ() + p3*A.GetW();
};

std::string Vec2::GetString(){
	std::stringstream sstream;
	sstream << m_X << m_Y;
	return sstream.str();
}

Vec2 Vec2::operator - () const{
	return Vec2(-m_X,-m_Y);
}

_MATH_END
