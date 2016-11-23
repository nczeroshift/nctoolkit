
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_MAT44_H
#define NCK_MAT44_H

#include "nckMathConfig.h"

_MATH_BEGIN

class Vec2;
class Vec3;
class Vec4;
class Quat;

/**
* 4 by 4 single precision float matrix.
*/
class Mat44
{
public:
	/// Default constructor.
	Mat44();

	/// Constructor from float array.
	Mat44(const float *v);

	/// Copy operator.
	Mat44 & operator = (const Mat44 & vec);

	/// Copy and multiply operator.
	Mat44 & operator *= (const Mat44 & vec);

	/// Matrix-Matrix multiplication operator.
	friend Mat44 operator * (const Mat44 & matA, const Mat44 & matB);

	/// Vec4-Matrix multiplication operator.
	friend Vec4 operator * (const Vec4 & vec,const Mat44 & mat);

	/// Vec3-Matrix multiplication operator.
	friend Vec3 operator * (const Vec3 & vec,const Mat44 & mat);

	/// Vec2-Matrix multiplication operator.
	friend Vec2 operator * (const Vec2 & vec,const Mat44 & mat);

	/// Matrix-Vec3 multiplication operator.
	friend Vec4 operator * (const Mat44 & mat,const Vec4 & vec);

	/// Matrix-Vec3 multiplication operator.
	friend Vec3 operator * (const Mat44 & mat,const Vec3 & vec);

	/// Matrix-Vec2 multiplication operator.
	friend Vec2 operator * (const Mat44 & mat,const Vec2 & vec);

	/// Return rotation part from the matrix.
	friend Mat44 RotationPart(const Mat44 & mat);

	/// Convert rotation part to a quaternion.
	friend Quat Rotation(const Mat44 & mat);

	/// Get translation part from the matrix.
	friend Vec3 TranslationPart(const Mat44 & mat);

	/// Compute translation matrix.
	friend Mat44 Translate(const Vec3 & vec);

	/// Compute scale matrix.
	friend Mat44 Scale(const Vec3 & vec);

	/// Compute rotation matrix in the X axis.
	/// @param x Rotation angle in radians.
	friend Mat44 RotateX(const float & x);

	/// Compute rotation matrix in the Y axis.
	/// @param y Rotation angle in radians.
	friend Mat44 RotateY(const float & y);

	/// Compute rotation matrix in the Z axis.
	/// @param z Rotation angle in radians.
	friend Mat44 RotateZ(const float & z);

	/// Compute the rotation matrix from a quaternion.
	friend Mat44 Rotate(const Quat & quat);

	/// Get identity matrix.
	friend Mat44 Identity();

	/// Get transposed matrix.
	friend Mat44 Transpose(const Mat44 & mat);

	/// Compute inverted matrix.
	friend bool Invert(const Mat44 & source, Mat44 *target);

	/// Compute perspective projection matrix. 
	friend Mat44 Perspective(float aspect, float fov, float znear, float zfar);

	/// Compute orthogonal projection matrix. 
    Mat44 Ortho(float left, float top, float right, float bottom, float znear,float zfar);

	/// Compute track to vector(or lookat) transformation matrix. 
	//friend Mat44 TrackTo(const Vec3 & source, const Vec3 & target, 
	//								const Vec3 & up_vector);

	/** Project vector from 3d space to viewport screenspace.
	* @param proj Projection matrix.
	* @param view View matrix.
	* @param vp Viewport width and height specifically.
	* @param vec Point in the 3D space.
	* @return Vector in viewport screenspace.
	*/
	//friend Vec4 Project(const &Mat44 proj, const &Mat44 view, const float vp[2], const Vec4& vec);

public:
    float m_n[4][4];
};

Mat44 operator * (const Mat44 & matA, const Mat44 & matB);
Vec4 operator * (const Vec4 & vec,const Mat44 & mat);
Vec3 operator * (const Vec3 & vec,const Mat44 & mat);
Vec2 operator * (const Vec2 & vec,const Mat44 & mat);
Vec4 operator * (const Mat44 & mat,const Vec4 & vec);
Vec3 operator * (const Mat44 & mat,const Vec3 & vec);
Vec2 operator * (const Mat44 & mat,const Vec2 & vec);
Mat44 RotationPart(const Mat44 & mat);
Quat Rotation(const Mat44 & mat);
Vec3 TranslationPart(const Mat44 & mat);
Mat44 Translate(const Vec3 & vec);
Mat44 Scale(const Vec3 & vec);
Mat44 RotateX(const float & x);
Mat44 RotateY(const float & y);
Mat44 RotateZ(const float & z);
Mat44 Rotate(const Quat & quat);
Mat44 Identity();
Mat44 Transpose(const Mat44 & mat);
bool Invert(const Mat44 & source, Mat44 *target);
Mat44 Perspective(float aspect, float fov, float znear, float zfar);
Mat44 Ortho(float left, float top, float right, float bottom, float znear, float zfar);
//Mat44 TrackTo(const Vec3 & source, const Vec3 & target, Vec3 & up_vector);

_MATH_END

#endif // #ifndef NCK_MAT44_H
