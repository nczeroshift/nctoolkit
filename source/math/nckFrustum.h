
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_FRUSTUM_H
#define NCK_FRUSTUM_H

#include "nckPlane.h"

_MATH_BEGIN

class Mat44;

/**
* Projection volume for object clipping operations.
*/
class Frustum 
{
public:
	/// Default constructor.
	Frustum();

	/// Update frustum planes from view and projection matrix.
	void Update(const Mat44 & mv,const Mat44 & proj);

	/// Check if sphere is in the frustum. 
	bool CheckSphere(const Vec3 & center,const float & radius = 0.0f) const;

	/// Check if one of the cube vertices is in the frustum
	bool CheckCube(const Vec3 & min,const Vec3 & max) const;

private:

	/// Projection volume boundaries.
	Plane m_Plane[6];
};

_MATH_END

#endif // #ifndef NCK_FRUSTUM_H
