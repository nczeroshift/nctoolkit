
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_VEC4_H
#define NCK_VEC4_H

#include "nckMathConfig.h"
#include <string>

_MATH_BEGIN

class Vec3;
class Mat44;

/**
* 4D single precision floating point vector class.
*/
class Vec4 
{
public:
	/// Default constructor.
	Vec4();

	// Constructor from each coordinate.
	Vec4(float x,float y, float z, float w);

	/// Constructor from Vec3 and W coordinate.
	Vec4(const Vec3 & vec,float w);

	/// Get X coordinate.
	inline float GetX() const{return m_X;}

	/// Get Y coordinate.
	inline float GetY() const{return m_Y;}

	/// Get Z coordinate.
	inline float GetZ() const{return m_Z;}

	/// Get W coordinate.
	inline float GetW() const{return m_W;}

	/// Set X coordinate.
	inline Vec4 & SetX(float value){m_X=value;return *this;}

	/// Set Y coordinate.
	inline Vec4 & SetY(float value){m_Y=value;return *this;}

	/// Set Z coordinate.
	inline Vec4 & SetZ(float value){m_Z=value;return *this;}

	/// Set W coordinate.
	inline Vec4 & SetW(float value){m_W=value;return *this;}

	/// Copy operator.
	Vec4 & operator = (const Vec4 & vec);

	/// Copy and add operator.
	Vec4 & operator += (const Vec4 & vec);

	/// Copy and subtract operator.
	Vec4 & operator -= (const Vec4 & vec);

	/// Copy and multiply operator.
	Vec4 & operator *= (float);

	/// Negation operator.
	Vec4 operator -() const;

	/// Subtraction operation between two vectors.
	friend Vec4 operator - (const Vec4 & vecA, const Vec4 & vecB);

	/// Sum operation between two vectors.
	friend Vec4 operator + (const Vec4 & vecA, const Vec4 & vecB);

	/// Multiplication between two vectors.
	friend Vec4 operator * (const Vec4 & vecA, const Vec4 & vecB);

	/// Multiplication by scalar.
	friend Vec4 operator * (const Vec4 & vecA, float value);

	/// Multiplication by scalar.
	friend Vec4 operator * (float value, const Vec4 & vec);

	/// Division by scalar.
	friend Vec4 operator / (const Vec4 & vec, float value);

	/// Division by scalar.
	friend Vec4 operator / (float value, const Vec4 & vec);

	/// Compute the normalized vector.
	friend Vec4 Normalize(const Vec4 & vec);

	/// Compute the dot product between two vectors.
	friend float Dot(const Vec4 & vecA, const Vec4 & vecB);

	/// Returns the angle(radians) between two vectors.
	friend float Angle(const Vec4 & vecA, const Vec4 & vecB);

	/// Cubic Bezier interpolation function.
	friend Vec4 BezierInterpolation(const Vec4 & p0,const Vec4 & p1,
										const Vec4 & p2, const Vec4& p3,
										float t);
	
	/// Linear interpolation function.
	friend Vec4 LinearInterpolation(const Vec4 & p0,const Vec4 & p1,float t);
		
	/// Compute vector length.
	float Length() const;

	/// Get vector debug string.
	std::string GetString();

private:

	/// Vector coordinates.
	float m_X,m_Y,m_Z,m_W;
};

Vec4 operator - (const Vec4 & vecA, const Vec4 & vecB);
Vec4 operator + (const Vec4 & vecA, const Vec4 & vecB);
Vec4 operator * (const Vec4 & vecA, const Vec4 & vecB);
Vec4 operator * (const Vec4 & vecA, float value);
Vec4 operator * (float value, const Vec4 & vec);
Vec4 operator / (const Vec4 & vec, float value);
Vec4 operator / (float value, const Vec4 & vec);
Vec4 Normalize(const Vec4 & vec);
float Dot(const Vec4 & vecA, const Vec4 & vecB);
float Angle(const Vec4 & vecA, const Vec4 & vecB);
Vec4 BezierInterpolation(const Vec4 & p0,const Vec4 & p1,const Vec4 & p2,
							const Vec4 & p3,float t);
Vec4 LinearInterpolation(const Vec4 & p0,const Vec4 & p1,float t);

_MATH_END

#endif // #ifndef NCK_VEC4_H
