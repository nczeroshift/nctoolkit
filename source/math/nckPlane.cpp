
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckPlane.h"
#include "nckLine.h"

_MATH_BEGIN

Plane::Plane()
{
	m_A = m_B = m_C = m_D = 0.0;
}

Plane::Plane (float a,float b,float c,float d)
{
	m_A = a;
	m_B = b;
	m_C = c;
	m_D = d;
}

Plane::Plane (const Vec3 & v0,const Vec3 & v1,const Vec3 & v2)
{
	Vec3	e0	= v1 - v0;
	Vec3	e1	= v2 - v0;

	Vec3 n = Normalize(Cross(e0,e1));

	m_A = n.GetX();
	m_B = n.GetY();
	m_C = n.GetZ();

	m_D = -Dot(v0,n);
}

Vec3 Plane::GetNormal() const{ return Vec3(m_A,m_B,m_C); }

void Plane::SetNormal(const Vec3 & nor){
 m_A = nor.GetX();
 m_B = nor.GetY();
 m_C = nor.GetZ();
}

Plane Normalize(const Plane & p )
{
	float l = sqrt(p.m_A*p.m_A+p.m_B*p.m_B+p.m_C*p.m_C);

	if(l==0.0)
		return Plane();

	l = 1.0f/l;

	return Plane(p.m_A * l,p.m_B * l,p.m_C * l,p.m_D * l);
}

float Distance (const Plane & plane,const Vec3 & vec)
{
	// Tricks of the 3d game programming gurus
	// page 304
	return Dot(Vec3(plane.m_A,plane.m_B,plane.m_C),vec) + plane.m_D;
}

bool Intersection(const Plane & plane, const Line & line, Vec3 *res)
{
	float d_iv = Dot(plane.GetNormal(),line.GetStart()-line.GetEnd());

	if(d_iv == 0.0f)
		return false;

	float t = (-Dot(plane.GetNormal(),line.GetEnd())+plane.m_D)/d_iv;

	if(t >= 0.0f && t <= 1.0f)
	{
		*res = line.GetStart() * t + line.GetEnd() * (1.0f-t);
		return true;
	}

	return false;
}


_MATH_END
