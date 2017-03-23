
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckQuat.h"
#include <math.h>

_MATH_BEGIN

Quat::Quat(){
	m_qV = Vec3(0,0,0);
	m_q0 = 1;	
}

Quat::Quat(float x,float y,float z,float w){
	m_qV = Vec3(x,y,z);
	m_q0 = w;	
}

Quat::Quat(const Vec3 & qV, float q0){
	m_qV= qV;
	m_q0= q0;
}

Quat & Quat::operator = (const Quat & quat){
	m_qV = quat.m_qV;
	m_q0 = quat.m_q0;
	return *this;
}

Quat & Quat::operator += (const Quat & quat){
	m_qV += quat.m_qV;
	m_q0 += quat.m_q0;
	return *this;
}

Quat & Quat::operator -= (const Quat & quat){
	m_qV -= quat.m_qV;
	m_q0 -= quat.m_q0;
	return *this;
}

Quat & Quat::operator *= (float value){
	m_qV *= value;
	m_q0 *= value;
	return *this;
}

Quat Quat::operator -() const{
	return Quat(-m_qV,m_q0);
}

float Quat::Length() const{
	return sqrt(Dot(m_qV,m_qV) + m_q0*m_q0);
}

Quat operator + (const Quat & quatA, const Quat & quatB){
	return Quat(quatA.m_qV+quatB.m_qV,quatA.m_q0+quatB.m_q0);
}

Quat operator * (const Quat & quatA, const Quat & quatB){
	return Quat(quatB.m_qV*quatA.m_q0 + quatA.m_qV*quatB.m_q0 + Cross(quatA.m_qV,quatB.m_qV) ,quatA.m_q0*quatB.m_q0 + Dot(quatA.m_qV,quatB.m_qV));
}

Quat operator - (const Quat & quatA, const Quat & quatB){
	return Quat(quatA.m_qV-quatB.m_qV,quatA.m_q0-quatB.m_q0);
}

Quat operator * (const Quat & quatA, const float value){
	return Quat(quatA.m_qV*value,quatA.m_q0*value);
}

Quat operator / (const Quat&quatA, const float value){
	return Quat(quatA.m_qV/value,quatA.m_q0/value);
}

Quat Normalize(const Quat & quat){
	float l = quat.Length();

	if(l==0.0)
		return quat;

	l = 1.0f/l;

	return Quat(quat.m_qV * l, quat.m_q0*l);
}

Quat BezierInterpolation(const Quat & p0,const Quat & p1,
							const Quat & p2, const Quat & p3,float t){
	if(t<0.0f)
		return p0;
	if(t>1.0f)
		return p3;

	// http://en.wikipedia.org/wiki/B�zier_curve
	float t1 = powf(1.0f-t,3.0f);
	float t2 = 3.0f*powf(1.0f-t,2.0f)*t;
	float t3 = 3.0f*(1.0f-t)*powf(t,2.0f);
	float t4 = powf(t,3.0f);

	return p0*t1 + p1*t2 + p2*t3 + p3*t4;
}

Quat LinearInterpolation(const Quat & p0,const Quat & p1,float t)
{
	if(t<0.0f)
		return p0;
	if(t>1.0f)
		return p1;
	return p0*(t-1.0f) + p1*t;
}

Quat EulerToQuat(const Vec3 & quat)
{
	// 3D Math Primer For Graphics & Game Development
	// page 190-191
	float cr, cp, cy, sr, sp, sy, cpcy, spsy;

	cr = cos(quat.GetX()/2);
	cp = cos(quat.GetY()/2);
	cy = cos(quat.GetZ()/2);

	sr = sin(quat.GetX()/2);
	sp = sin(quat.GetY()/2);
	sy = sin(quat.GetZ()/2);

	cpcy = cp * cy;
	spsy = sp * sy;

	Quat q (sr * cpcy - cr * spsy,
			cr * sp * cy + sr * cp * sy, 
			cr * cp * sy - sr * sp * cy,
			cr * cpcy + sr * spsy);

	return  q;
}

_MATH_END
