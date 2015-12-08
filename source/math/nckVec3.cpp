
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckVec2.h"
#include "nckVec3.h"
#include "nckVec4.h"
#include "nckMat44.h"
#include <sstream>

_MATH_BEGIN
	
Vec3::Vec3(){
	m_X = 0;
	m_Y = 0;
	m_Z = 0;
}

Vec3::Vec3(float x,float y,float z){
	m_X = x;
	m_Y = y;
	m_Z = z;
}

Vec3::Vec3(const float *farray){
	m_X = farray[0];
	m_Y = farray[1];
	m_Z = farray[2];
}

Vec3::Vec3(const Vec4 & vec){
	m_X = vec.GetX();
	m_Y = vec.GetY();
	m_Z = vec.GetZ();
}

Vec3::Vec3(const Vec2 & vec,float z)
{
	m_X = vec.GetX();
	m_Y = vec.GetY();
	m_Z = z;
}

Vec3 Vec3::operator - () const{
	return Vec3(-m_X,-m_Y,-m_Z);
}

Vec3 & Vec3::operator = (const Vec3 & vec){
	m_X = vec.m_X;
	m_Y = vec.m_Y;
	m_Z = vec.m_Z;
	return *this;
}	

Vec3 & Vec3::operator += (const Vec3 & vec){
	m_X += vec.m_X;
	m_Y += vec.m_Y;
	m_Z += vec.m_Z;
	return *this;
}

Vec3 & Vec3::operator -= (const Vec3 & vec){
	m_X -= vec.m_X;
	m_Y -= vec.m_Y;
	m_Z -= vec.m_Z;
	return *this;
}

Vec3 & Vec3::operator *= (float value){
	m_X *= value;
	m_Y *= value;
	m_Z *= value;
	return *this;
}

Vec3 operator + (const Vec3 & vecA, const Vec3 & vecB){
	return Vec3(vecA.m_X+vecB.m_X,vecA.m_Y+vecB.m_Y,vecA.m_Z+vecB.m_Z);
}

Vec3 operator * (const Vec3 & vecA, const Vec3 & vecB){
	return Vec3(vecA.m_X*vecB.m_X,vecA.m_Y*vecB.m_Y,vecA.m_Z*vecB.m_Z);
}

Vec3 operator - (const Vec3 & vecA, const Vec3 & vecB){
	return Vec3(vecA.m_X-vecB.m_X,vecA.m_Y-vecB.m_Y,vecA.m_Z-vecB.m_Z);
}

Vec3 operator * (const Vec3 & vecA, float value){
	return Vec3(vecA.m_X*value,vecA.m_Y*value,vecA.m_Z*value);
}

Vec3 operator / (const Vec3 & vecA, float value){
	return Vec3(vecA.m_X/value,vecA.m_Y/value,vecA.m_Z/value);
}

float Dot(const Vec3 & vecA, const Vec3 & vecB){
	return vecA.m_X* vecB.m_X + vecA.m_Y* vecB.m_Y + vecA.m_Z* vecB.m_Z;
}

Vec3 Cross(const Vec3 & vecA, const Vec3 & vecB){
	return Vec3(vecA.m_Y*vecB.m_Z-vecB.m_Y*vecA.m_Z,
				-(vecA.m_X*vecB.m_Z-vecB.m_X*vecA.m_Z),
				vecA.m_X*vecB.m_Y-vecB.m_X*vecA.m_Y);
}

float Angle(const Vec3 & vecA, const Vec3 & vecB){
	return acosf(Dot(vecA,vecB));
}

float Vec3::Length() const{
	return sqrtf(m_X* m_X + m_Y* m_Y + m_Z* m_Z);
}

Vec3 Normalize(const Vec3 & vec){
	float l = vec.Length();

	if(l < 1.0e-35)
		return Vec3();

	l = 1.0f/l;

	return Vec3(vec.m_X*l,vec.m_Y*l,vec.m_Z*l);
}

Vec3 Reflect(const Vec3 & nor, const Vec3 & vec){
	return nor*(Dot(nor,vec))*2.0f-vec;
}

Vec3 BezierInterpolation(const Vec3 & vecA,const Vec3 & vecA_n,
									const Vec3 & vecB_n, const Vec3 & vecB,
										float t){
	if(t<0.0f)
		return vecA;
	if(t>1.0f)
		return vecB;

	// http://en.wikipedia.org/wiki/B�zier_curve
	float t1 = powf(1.0f-t,3.0f);
	float t2 = 3.0f*powf(1.0f-t,2.0f)*t;
	float t3 = 3.0f*(1.0f-t)*powf(t,2.0f);
	float t4 = powf(t,3.0f);

	return vecA*t1 + vecA_n*t2 + vecB_n*t3 + vecB*t4;
}

Vec3 LinearInterpolation(const Vec3 & vecA,const Vec3 & vecB,float t){
	if(t<0.0f)
		return vecA;
	if(t>1.0f)
		return vecB;
	return vecA*(t-1.0f) + vecB*t;
}

Vec3 BezierInterpolation(const Mat44& inv_mat,const Vec3 & P0,const Vec3 & P1,
	const Vec3 & P2,const Vec3 & P3, float t)
{
	// inv_mat = B Matrix, Mixture matrix.
	Vec4 U = Vec4(1.0,t, t*t, t*t*t);
	// A Matrix = UxB
	Vec4 A = U*inv_mat;
	// p(u) = UxBxP
	return P0*A.GetX() + P1*A.GetY() + P2*A.GetZ() + P3*A.GetW();
};

std::string Vec3::GetString(){
	std::stringstream sstream;
	sstream << m_X << m_Y << m_Z;
	return sstream.str();
}

_MATH_END
