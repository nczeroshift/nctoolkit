
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_QUAT_H
#define NCK_QUAT_H

#include "nckVec3.h"

_MATH_BEGIN

/**
* Quaternion class with single precision floating point.
*/
class Quat
{
public:
	/// Default constructor.
	Quat();

	/// Constructor from each coordinate.
	Quat(float x, float y,float z,float w);

	/// Constructor from axis and angle.
	Quat(const Vec3 & vector,float w);

	/// Get X coordinate.
	inline float GetX() const{return m_qV.GetX();}

	/// Get Y coordinate.
	inline float GetY() const{return m_qV.GetY();}

	/// Get Z coordinate.
	inline float GetZ() const{return m_qV.GetZ();}

	/// Get W coordinate.
	inline float GetW() const{return m_q0;}

	/// Set X coordinate.
	inline Quat & SetX(float value){m_qV.SetX(value);return *this;}

	/// Set Y coordinate.
	inline Quat & SetY(float value){m_qV.SetY(value);return *this;}

	/// Set Z coordinate.
	inline Quat & SetZ(float value){m_qV.SetZ(value);return *this;}

	/// Set W coordinate.
	inline Quat & SetW(float value){m_q0 = value;return *this;}

	/// Copy operator.
	Quat & operator = (const Quat & quat);

	/// Copy and add operator.
	Quat & operator += (const Quat & quat);

	/// Copy and subtract operator.
	Quat & operator -= (const Quat & quat);

	/// Copy and multiply operator.
	Quat & operator *= (float value);

	/// Subtraction operation between two quaternions.
	friend Quat operator - (const Quat & quatA, const Quat & quatB);

	/// Sum operation between two quaternions.
	friend Quat operator + (const Quat & quatA, const Quat & quatB);

	/// Multiplication operation between two quaternions.
	friend Quat operator * (const Quat & quatA, const Quat & quatB);

	/// Multiplication by scalar.
	friend Quat operator * (const Quat & quat, float value);

	/// Multiplication by scalar.
	friend Quat operator * (float value, const Quat & quat);

	/// Division by scalar operator.
	friend Quat operator / (const Quat & quat, float value);

	/// Division by scalar operator.
	friend Quat operator / (float value, const Quat & quat);
	
	/// Compute conjugated quaternion.
	Quat operator -() const;

	/// Compute quaternion length.
	float Length() const;

	/// Compute normalized quaternion.
	friend Quat Normalize(const Quat & quat);

	/// Cubic Bezier interpolation function.
	friend Quat BezierInterpolation(const Quat & p0,
										const Quat & p1,
										const Quat & p2,
										const Quat & p3,
										float t);
	
	/// Linear interpolation function.
	friend Quat LinearInterpolation(const Quat & p0,
										const Quat & p1,
										float t);

	/// Convert euler rotation(in radians) to quaternion.
	friend Quat EulerToQuat(const Vec3 & vec); 

private:
	Vec3 m_qV;
	float m_q0;
};

Quat operator - (const Quat & quatA, const Quat & quatB);
Quat operator + (const Quat & quatA, const Quat & quatB);
Quat operator * (const Quat & quatA, const Quat & quatB);
Quat operator * (const Quat & quat, float value);
Quat operator / (const Quat & quat, float value);
Quat operator * (float value,const Quat & quat);
Quat operator / (float value,const Quat & quat);
Quat Normalize(const Quat & quat);
Quat BezierInterpolation(const Quat & p0,const Quat & p1,
	const Quat & p2, const Quat & p3,float t);
Quat LinearInterpolation(const Quat & p0, const Quat & p1,
	float t);
Quat EulerToQuat(const Vec3 & vec); 

_MATH_END

#endif // #ifndef NCK_QUAT_H
