
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckLine.h"

_MATH_BEGIN

Line::Line(){ }

Line::Line(const Vec3 & start,const Vec3 & end){
	m_Start = start;
	m_End = end;
}

float Line::Length() const{
	return (m_Start-m_End).Length();
}

Vec3 Line::GetDirection() const{
	return Normalize(m_End-m_Start);
}

bool NearestPoint(const Line & l,const Vec3 & p, Vec3 *res,float *dist)
{
	bool valid = false;
	float u = Dot(p -l.m_Start,l.m_End-l.m_Start)/(powf((l.m_End-l.m_Start).Length(),2));
	
	if(u<0.0f)
		u = 0;
	else if (u>1.0f)
		u = 1;
	else
		valid = true;

	*res = l.m_Start * (1.0f - u) + l.m_End * u;
	*dist = (*res - p).Length();
	return valid;
}

bool Intersection(const Line & a, const Line & b, Vec3 *p)
{
	//http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
	float x1 = a.GetStart().GetX();
	float x2 = a.GetEnd().GetX();
	float x3 = b.GetStart().GetX();
	float x4 = b.GetEnd().GetX();

	float y1 = a.GetStart().GetY();
	float y2 = a.GetEnd().GetY();
	float y3 = b.GetStart().GetY();
	float y4 = b.GetEnd().GetY();

	float ua = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3))/((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
	float ub = ((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3))/((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));

	if(ua>= 0 && ub>= 0 && ua <= 1 && ub<= 1){

		*p = b.GetStart() + (b.GetEnd() - b.GetStart())*ub;
		return true;
	}
	else 
		return	false;
}

bool NearestPointToIntersection(const Math::Line & limit, const Math::Line & path, float r, 
	Math::Vec3 * intersection, Math::Vec3 * nearest)
{
	Math::Vec3 pp;
	//Math::Line newPath = path;
	if(Math::Intersection(limit,path,&pp))
	{
		*intersection = pp;

		Math::Vec3 nor = path.GetDirection();
		Math::Vec3 n1 = Math::Normalize(limit.GetStart()-pp);
		Math::Vec3 n2 = Math::Normalize(limit.GetEnd()-pp);

		float dot1 = Math::Dot(n1,nor);
		float dot2 = Math::Dot(n2,nor);

		float smallestDot = 0;
		if(dot1 > 0 && dot2 > 0){
			if(dot1 < dot2)
				smallestDot = dot2;
			else 
				smallestDot = dot1;
		}
		else if(dot1 > dot2)
			smallestDot = dot1;
		else 
			smallestDot = dot2;

		if(smallestDot == 0.0){
			*nearest = pp - nor * r;
			double alpha = fabs((pp-limit.GetStart()).Length()/(double)limit.Length());
			if(alpha >= 0.0 && alpha <= 1.0){
				return true;
			}
		}

		double angle = acos(smallestDot);

		double h = r / sin(angle);
		double l = h * cos(angle);

		*nearest = pp - nor * h;

		if(smallestDot == dot1){
			double d1 = (pp-limit.GetStart()).Length();
			if(l < d1)
				return true;
		}else if(smallestDot == dot2){
			double d2 = (pp-limit.GetEnd()).Length();
			if(l < d2)
				return true;
		}
		
	}

	return false;
}

_MATH_END
