
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_LINE_H
#define NCK_LINE_H

#include "nckVec3.h"

_MATH_BEGIN

class Plane;

/**
* Line defined by 2 vectors in the 
* three-dimensional space.
*/
class Line
{
	/// Start vector.
	Vec3 m_Start;

	/// End vector.
	Vec3 m_End;

public:

	/// Default constructor.
	Line ();

	/// Constructor, define line start and end.
	Line (const Vec3 & start,const Vec3 & end);
	
	/// Set line start.
	inline Line & SetStart(const Vec3 & vec){m_Start = vec;return *this;}

	/// Get line start.
	inline Vec3 GetStart() const {return m_Start;}

	/// Set line end.
	inline Line & SetEnd(const Vec3 & vec){m_End = vec;return *this;}

	/// Get line end.
	inline Vec3 GetEnd() const {return m_End;}

	// Get line length.
	float Length() const;

	/// Get line direction(normal vector).
	Vec3 GetDirection() const;

	/** 
	* Test the intersection of two lines in 2D (z is not used).
	* @param a Line A.
	* @param b Line B.
	* @param p Reference to intersection point
	* @return True if the lines intersect.
	*/
	friend bool Intersection(const Line & a, const Line & b, Vec3 *p);

	
	/** 
	* Get the nearest point to a line.
	* @param l Test line.
	* @param p Test point.
	* @param res Reference to the nearest point calculated.
	* @param dist Reference to the Distance between the point 
	* and the nearest point calculated.
	* @return True if a valid point was found, false otherwise.
	*/
	friend bool NearestPoint( const Line & l,const Vec3 & p, Vec3 * res,
										float *dist);

	/** 
	* Get the nearest point to a intersection between two lines.
	* @param limit "Boundary" path line.
	* @param path "Motion" path line.
	* @param r Circle radius.
	* @param intersection Intersection result position.
	* and the nearest Nearest point to intersection based on circle radius.
	* @return True if a valid point was found, false otherwise.
	*/
	friend bool NearestPointToIntersection(const Math::Line & limit, const Math::Line & path, float r, 
		Math::Vec3 * intersection, Math::Vec3 * nearest);
};

bool Intersection(const Line & a, const Line & b, Vec3 *p);
bool NearestPoint( const Line & l,const Vec3 & p, Vec3 * res,float *dist);
bool NearestPointToIntersection(const Math::Line & limit, const Math::Line & path, float r, 
		Math::Vec3 * intersection, Math::Vec3 * nearest);

_MATH_END

#endif // #ifndef NCK_LINE_H
