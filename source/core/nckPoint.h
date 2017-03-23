
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_POINT_H
#define NCK_POINT_H

#include "nckCoreConfig.h"

_CORE_BEGIN

/**
* Simple 2D point with integer coordinates.
*/
class Point{
public:
	/// Default Constructor.
	Point();

	/// Constructor from coordinates.
	Point(int x, int y);

	/// Set X coordinate.
	const Point& SetX(int x);

	/// Set Y coordinate.
	const Point& SetY(int y);

	/// Get X coordinate.
	inline int GetX() const{return m_X;}

	/// Get Y coordinate.
	inline int GetY() const{return m_Y;}

	/// Compute distance from a point to a line.
	/// Point coordinates are in float for extra precision in Anti-Aliasing computation.
	static bool DistanceToLine(const Core::Point & start,const Core::Point & end,
		float Px,float Py,float *dist);

private:
	/// X Coordinate.
	int m_X;

	/// Y Coordinate.
	int m_Y;
};

_CORE_END

#endif // #ifndef NCK_POINT_H
