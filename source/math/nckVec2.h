
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_VEC2_H
#define NCK_VEC2_H

#include "nckMathConfig.h"
#include <string>

_MATH_BEGIN

class Mat44;
class Vec3;

/**
* 2D single precision floating point vector class.
*/
class Vec2
{
public:
	/// Default constructor.
	Vec2();

	/// Constructor with each coordinates.
	Vec2(float x,float y);

	/// Constructor from float array.
	Vec2(const float *farray);

	/// Constructor from Vec3 class.
	Vec2(const Vec3 & vec);

	/// Get X coordinate.
	float GetX() const{return m_X;}

	/// Get Y coordinate.
	float GetY() const{return m_Y;}

	/// Set X coordinate.
	Vec2 & SetX(float value){m_X = value;return *this;}

	/// Set Y coordinate.
	Vec2 & SetY(float value){m_Y = value;return *this;}

	/// Copy operator.
	Vec2 & operator = (const Vec2 & vec);

	/// Copy and add operator.
	Vec2 & operator += (const Vec2 & vec);

	/// Copy and subtract operator.
	Vec2 & operator -= (const Vec2 & vec);

	/// Copy and multiply operator.
	Vec2 & operator *= (float value);

	/// Negation operator.
	Vec2 operator -() const;

	/// Subtraction operation between two vectors.
	friend Vec2 operator - (const Vec2 & vecA, const Vec2 & vecB);

	/// Sum operation between two vectors.
	friend Vec2 operator + (const Vec2 & vecA, const Vec2 & vecB);

	/// Multiplication operator.
	friend Vec2 operator * (const Vec2 & vecA, const Vec2 & vecB);

	/// Multiplication by scalar operator.
	friend Vec2 operator * (const Vec2 & vecA, const float value);

	/// Multiplication by scalar operator.
	friend Vec2 operator * (const float value, const Vec2 & vec);

	/// Division by scalar operator.
	friend Vec2 operator / (const Vec2 & vec, const float value);

    ///  Division by vector operator.
    friend Vec2 operator / (const Vec2 & vecA, const Vec2 & vecB );

	/// Division by scalar operator.
	friend Vec2 operator / (const float value,const Vec2 & vec);

	/// Vector matrix algebric multiplication operator.
	friend Vec2 operator * (const Vec2 & vec,const Mat44 & mat);

	/// Matrix vector algebric multiplication operator.
	friend Vec2 operator * (const Mat44 & mat,const Vec2 & vec);

	/// Return normalized vector.
	friend Vec2 Normalize(const Vec2 & vec);

	/// Return the dot product between two vectors.
	friend float Dot(const Vec2 & vecA, const Vec2 & vecB);

	/// Return the angle(radians) between two vectors.
	friend float Angle(const Vec2 & vecA, const Vec2 & vecB);

	/// Cubic bezier interpolation function.
	friend Vec2 BezierInterpolation(const Vec2 & p0,const Vec2 & p1,
										const Vec2 & p2, const Vec2 & p3,
										float t);
	
	/// Cubic bezier interpolation method with irregular P1,P2 positioning.
	friend Vec2 BezierInterpolation(const Mat44 & inv_mat,const Vec2 & p0,
										const Vec2 & p1,const Vec2 & p2,
										const Vec2 & p3, float t);

	/// Linear interpolation function.
	friend Vec2 LinearInterpolation(const Vec2 & p0, const Vec2 & p1,float t);

	/// Return vector length.
	float Length() const;

	/// Get vector debug string.
	std::string GetString();

private:
	/// Vector coordinates.
	float m_X,m_Y;
};

Vec2 operator - (const Vec2 & vecA, const Vec2 & vecB);
Vec2 operator + (const Vec2 & vecA, const Vec2 & vecB);
Vec2 operator * (const Vec2 & vecA, const Vec2 & vecB);
Vec2 operator * (const Vec2 & vecA, const float value);
Vec2 operator * (const float value, const Vec2 & vec);
Vec2 operator / (const Vec2 & vec, const float value);
Vec2 operator / (const Vec2 & vecA, const Vec2 & vecB);
Vec2 operator / (const float value,const Vec2 & vec);
Vec2 operator * (const Vec2 & vec,const Mat44 & mat);
Vec2 operator * (const Mat44 & mat,const Vec2 & vec);
Vec2 Normalize(const Vec2 & vec);
float Dot(const Vec2 & vecA, const Vec2 & vecB);
float Angle(const Vec2 & vecA, const Vec2 & vecB);
Vec2 BezierInterpolation(const Vec2 & p0,const Vec2 & p1,
										const Vec2 & p2, const Vec2 & p3,
										float t);
Vec2 BezierInterpolation(const Mat44 & inv_mat,const Vec2 & p0,
										const Vec2 & p1,const Vec2 & p2,
										const Vec2 & p3, float t);
Vec2 LinearInterpolation(const Vec2 & p0, const Vec2 & p1,float t);

_MATH_END

#endif // #ifndef NCK_VEC2_H
