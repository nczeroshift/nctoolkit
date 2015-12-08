
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_BOUNDBOX_H
#define NCK_BOUNDBOX_H

#include "nckVec3.h"

_MATH_BEGIN

class Mat44;

/**
* Axis aligned bound box.
*/
class BoundBox
{
public:
	/// Default constructor.
	BoundBox();

	/// Constructor from max and min vectors.
	BoundBox(const Vec3 & max,const Vec3 & min);
	
	/// Recalculate box limits from a new vertex.
	void Insert(const Vec3 & v);

	/// Compute transformed boundbox.
	BoundBox operator *(const Mat44 & mat); 

	/// Set boundbox min vector.
	inline BoundBox & SetMin(const Vec3 & vec){ m_Min = vec; return *this; }

	/// Get boundbox max vector.
	inline BoundBox & SetMax(const Vec3 & vec){ m_Max = vec; return *this; }

	/// Get boundbox max vector.
	inline Vec3 GetMax() const { return m_Max; }

	/// Get boundbox min vector.
	inline Vec3 GetMin() const { return m_Min; }

	/// Check if point is inside bounds.
	friend bool Inside(const BoundBox & bb, const Vec3 & v);
	
	/// Check if two bound boxes intersect. 
	friend bool Intersect(const BoundBox & bb1, const BoundBox & bb2);
private:
	/// Boundbox max limit.
	Vec3 m_Max;

	/// Boundbox min limit.
	Vec3 m_Min;

	/// Boundbox inicialization flag.
	bool m_Initialized;
};

bool Inside(const BoundBox & bb, const Vec3 & v);
bool Intersect(const BoundBox & bb1, const BoundBox & bb2);

_MATH_END

#endif // #ifndef NCK_BOUNDBOX_H