
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_PLANE_H
#define NCK_PLANE_H

#include "nckVec3.h"

_MATH_BEGIN

class Line;

/**
* Infinite plane defined in three-dimensional
* space using real numbers (single precision).
*/ 
class Plane
{
public:
	/// Default constructor.
	Plane ();

	/// Constructor, create plane from coordinates.
	Plane (float a,float b,float c,float d);

	/// Constructor, process plane coordinates from 3 support points.
	Plane (const Vec3 & v0,const Vec3 & v1,const Vec3 & v2);
		
	/// Get plane X coordinate.
	inline float GetX() const{return m_A;}

	/// Get plane Y coordinate.
	inline float GetY() const{return m_B;}

	/// Get plane Z coordinate.
	inline float GetZ() const{return m_C;}

	/// Get plane D coordinate.
	inline float GetD() const{return m_D;}

	/// Get plane normal vector.
	Vec3 GetNormal() const;

	/// Set plane X coordinate.
	inline Plane & SetX(float value){m_A = value;return *this;}

	/// Set plane Y coordinate.
	inline Plane & SetY(float value){m_B = value;return *this;}

	/// Set plane Z coordinate.
	inline Plane & SetZ(float value){m_C = value;return *this;}

	/// Set plane D coordinate.
	inline Plane & SetD(float value){m_D = value;return *this;}

	/// Set plane normal vector.
	void SetNormal(const Vec3 & nor);

	/// Returns normalized plane
	friend Plane Normalize(const Plane & plane);

	/// Returns distance between the plane and a point.
	friend float Distance(const Plane & plane,const Vec3 & vec);

	/**
	* Test the intersection of plane with a line.
	* @param p Intersected plane.
	* @param l Intersection line.
	* @param res Reference to a point in the plane.
	* @return True if the plane was intersected, false otherwise.
	*/
	friend bool Intersection(const Plane & plane, const Line & line, Vec3 *res);

private:
	/// Plane coordinates.
	float m_A,m_B,m_C,m_D;
};

Plane Normalize(const Plane & plane);
float Distance(const Plane & plane,const Vec3 & vec);
bool Intersection(const Plane & plane, const Line & line, Vec3 *res);

_MATH_END

#endif // #ifndef NCK_PLANE_H
