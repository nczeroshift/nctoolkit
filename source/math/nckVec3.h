
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_VEC3_H
#define NCK_VEC3_H

#include "nckMathConfig.h"
#include <string>

_MATH_BEGIN

class Vec2;
class Vec4;
class Mat44;

/**
* 3D single precision floating point vector class.
*/
class Vec3
{
public:
	/// Default constructor.
	Vec3();

	/// Constructor with each coordinates.
	Vec3(float x,float y,float z);

	/// Contructor from float array.
	Vec3(const float * farray);

	/// Contructor from Vec2 class.
	Vec3(const Vec2 & vec,float z);

	/// Contructor from Vec4 class.
	Vec3(const Vec4 & vec);

	/// Get X coordinate.
	inline float GetX() const{return m_X;}

	/// Get Y coordinate.
	inline float GetY() const{return m_Y;}

	/// Get Z coordinate.
	inline float GetZ() const{return m_Z;}

	/// Set X coordinate.
	inline Vec3 & SetX(float value){m_X = value;return *this;}

	/// Set Y coordinate.
	inline Vec3 & SetY(float value){m_Y = value;return *this;}

	/// Set Z coordinate.
	inline Vec3 & SetZ(float value){m_Z = value;return *this;}

	/// Copy operator.
	Vec3 & operator = (const Vec3 & vec);

	/// Copy and add operator.
	Vec3 & operator += (const Vec3 & vec);

	/// Copy and subtract operator.
	Vec3 & operator -= (const Vec3 & vec);

	/// Copy and multiply operator.
	Vec3 & operator *= (float value);

	/// Negation operator.
	Vec3 operator - () const;

	/// Subtraction operation between two vectors.
	friend Vec3 operator - (const Vec3 & vecA, const Vec3 & vecB);

	/// Sum operation between two vectors.
	friend Vec3 operator + (const Vec3 & vecA, const Vec3 & vecB);

	/// Multiplication operator.
	friend Vec3 operator * (const Vec3 & vecA, const Vec3 & vecB);

	/// Multiplication by scalar.
	friend Vec3 operator * (const Vec3 & vecA, float value);

	/// Multiplication by scalar.
	friend Vec3 operator * (float value,const Vec3 & vec);

	/// Division by scalar.
	friend Vec3 operator / (const Vec3 & vec, float value);

	/// Division by scalar.
	friend Vec3 operator / (float value, const Vec3 & vec);

	/// Return normalized vector.
	friend Vec3 Normalize(const Vec3 & vec);

	/// Return the dot product between two vectors.
	friend float Dot(const Vec3 & vecA, const Vec3 & vecB);

	/// Return the angle(radians) between two vectors.
	friend float Angle(const Vec3 & vecA, const Vec3 & vecB);

	/// Return the cross product between two vectors.
	friend Vec3 Cross(const Vec3 & vecA, const Vec3 & vecB);

	/// Cubic bezier interpolation function.
	friend Vec3 BezierInterpolation(const Vec3 & p0,const Vec3 & p1,
										const Vec3 & p2,const Vec3 & p3,
										float t);
	
	/// Cubic bezier interpolation method with irregular P1,P2 positioning.
	friend Vec3 BezierInterpolation(const Mat44 & mat,const Vec3 & P0,
										const Vec3 & P1,const Vec3 & P2,
										const Vec3 & P3, float t);

	/// Linear interpolation function.
	friend Vec3 LinearInterpolation(const Vec3 & p0, const Vec3 & p1,float t);
	
	/// Compute reflected vector.
	friend Vec3 Reflect(const Vec3 & nor, const Vec3 & vec);

	/// Compute vector length.
	float Length() const;

	/// Get vector debug string.
	std::string GetString();
    
private:

	/// Vector coordinates.
	float m_X,m_Y,m_Z;
};

Vec3 operator - (const Vec3 & vecA, const Vec3 & vecB);
Vec3 operator + (const Vec3 & vecA, const Vec3 & vecB);
Vec3 operator * (const Vec3 & vecA, const Vec3 & vecB);
Vec3 operator * (const Vec3 & vecA, float value);
Vec3 operator * (float value,const Vec3 & vec);
Vec3 operator / (const Vec3 & vec, float value);
Vec3 operator / (float value, const Vec3 & vec);;
Vec3 Normalize(const Vec3 & vec);
float Dot(const Vec3 & vecA, const Vec3 & vecB);
float Angle(const Vec3 & vecA, const Vec3 & vecB);
Vec3 Cross(const Vec3 & vecA, const Vec3 & vecB);
Vec3 BezierInterpolation(const Vec3 & p0,const Vec3 & p1,const Vec3 & p2,const Vec3 & p3,float t);
Vec3 BezierInterpolation(const Mat44 & mat,const Vec3 & P0,const Vec3 & P1,Vec3 & P2,const Vec3 & P3, float time);
Vec3 LinearInterpolation(const Vec3 & p0, const Vec3 & p1,float t);
Vec3 Reflect(const Vec3 & nor, const Vec3 & vec);

_MATH_END

#endif // #ifndef NCK_VEC3_H
