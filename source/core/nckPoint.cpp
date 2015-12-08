
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckPoint.h"
#include <math.h>

_CORE_BEGIN

Point::Point(){
	m_X=0;
	m_Y=0;
}

/// Constructor from coordinates.
Point::Point(int x, int y){
	m_X=x;
	m_Y=y;
}

/// Set X coordinate.
const Point& Point::SetX(int x){
	m_X = x;
	return *this;
}

/// Set Y coordinate.
const Point& Point::SetY(int y){
	m_Y = y;
	return *this;
}

bool Point::DistanceToLine(const Core::Point & start,const Core::Point & end,
	float Px,float Py,float *dist)
{
	bool valid = false;
	float dx = Px-start.GetX();
	float dy = Py-start.GetY();
	float dx2 = (float)(end.GetX()-start.GetX());
	float dy2 = (float)(end.GetY()-start.GetY());

	float u = (dx*dx2 + dy*dy2)/(dx2*dx2 + dy2*dy2);

	if(u<0.0f)
		u = 0.0f;
	else if (u>1.0f)
		u = 1.0f;
	else
		valid = true;

	float tx = start.GetX() * (1.0f - u) + end.GetX()*u;
	float ty = start.GetY() * (1.0f - u) + end.GetY()*u;

	float dx3 = tx - Px;
	float dy3 = ty - Py;

	*dist = sqrt( dx3*dx3 + dy3*dy3);

	return valid;
}

_CORE_END
