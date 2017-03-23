
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_TRIANGLE_H
#define NCK_TRIANGLE_H

#include "nckVec3.h"

_MATH_BEGIN

class Line;

/**
*  3D triangle primitive.
*/
class Triangle
{
	/// A vertex.
	Vec3 m_A;

	/// B vertex.
	Vec3 m_B;

	/// C vertex.
	Vec3 m_C;

public:

	/// Default constructor.
	Triangle ();

	/// Constructor, define triangle from vertices.
	Triangle (const Vec3 & a,const Vec3 & b,const Vec3 & c);
	
	/// Get A vertex.
	inline Vec3 GetA() const {return m_A;}

	/// Get B vertex.
	inline Vec3 GetB() const {return m_B;}

	/// Get C vertex.
	inline Vec3 GetC() const {return m_C;};

	/// Get A vertex.
	Triangle & SetA(const Vec3 & v){m_A = v;return *this;}

	/// Get B vertex.
	Triangle & SetB(const Vec3 & v){m_B = v;return *this;}

	/// Get C vertex.
	Triangle & SetC(const Vec3 & v){m_C = v;return *this;}

	/**
	* Check if a point is inside the triangle using 
	* barycentric coordinates.
	* @param t Triangle primitive.
	* @param p Tested point.
	* @param lambda_1 barycentric coordinate 1.
	* @param lambda_2 barycentric coordinate 2.
	* @return True if the point is inside the triangle, false otherwise.
	*/
	friend bool CheckPointInside(const Triangle & t,
		const Vec3 & p, 
		float *lambda_1,
		float *lambda_2);
};

bool CheckPointInside(const Triangle & t,const Vec3 & p,float *lambda_1,float *lambda_2);

_MATH_END

#endif // #ifndef NCK_TRIANGLE_H
