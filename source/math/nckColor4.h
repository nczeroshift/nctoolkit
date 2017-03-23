
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_COLOR4_H
#define NCK_COLOR4_H

#include "nckMathConfig.h"
#include <string>

_MATH_BEGIN

/**
* Color manipulation class, unsigned char components.
*/
class Color4ub
{
public:
	/// Default constructor.
	Color4ub();

	/// Constructor from color components.
	Color4ub(unsigned char dr,unsigned char dg,unsigned char db,unsigned char da=255);

	/// Constructor from RGB components and alpha color.
	Color4ub(const Color4ub & col , unsigned char a );

	/// Copy operator.
	Color4ub & operator = (const Color4ub & v);

	/// Copy and sum operator.
	Color4ub & operator += (const Color4ub & v);

	/// Multiplication between a color and a scalar.
	friend Color4ub operator * (const Color4ub & a,float value);

	/// Multiplication between a color and a scalar.
	friend Color4ub operator * (float value, const Color4ub & a);

	/// Subtraction between two colors.
	friend Color4ub operator - (const Color4ub & a, const Color4ub & b);

	/// Sum between two colors.
	friend Color4ub operator + (const Color4ub & a, const Color4ub & b);

	/// Get red value.
	inline unsigned char GetR() const{return m_R;}

	/// Get green value.
	inline unsigned char GetG() const{return m_G;}

	/// Get blue value.
	inline unsigned char GetB() const{return m_B;}

	/// Get alpha value.
	inline unsigned char GetA() const{return m_A;}

	/// Set red value.
	inline Color4ub & SetR(unsigned char val){m_R = val;return *this;}

	/// Set green value.
	inline Color4ub & SetG(unsigned char val){m_G = val;return *this;}

	/// Set blue value.
	inline Color4ub & SetB(unsigned char val){m_B = val;return *this;}

	/// Set alpha value.
	inline Color4ub & SetA(unsigned char val){m_A = val;return *this;}

	/// Get color string.
	std::string GetString();

private:

	/// Red value.
	unsigned char m_R;

	/// Green value.
	unsigned char m_G;

	/// Blue value.
	unsigned char m_B;
	
	/// Alpha value.
	unsigned char m_A;
};

Color4ub operator * (const Color4ub & a, float value);
Color4ub operator * (float value, const Color4ub & a);
Color4ub operator - (const Color4ub & a, const Color4ub & b);
Color4ub operator + (const Color4ub & a, const Color4ub & b);

/**
* Color manipulation class, single precision floating point components.
*/
class Color4f
{
public:
	/// Default constructor.
	Color4f();

	/// Constructor from color components.
	Color4f(float r,float g,float b,float a=1.0f);

	/// Constructor from color rgb and alpha.
	Color4f(const Color4f & col ,float a);

	/// Copy operator.
	Color4f & operator = (const Color4f & v);

	/// Copy and sum operator.
	Color4f & operator += (const Color4f & v);

	/// Multiplication between a colors and a scalar.
	friend Color4f operator * (const Color4f & a, float value);

	/// Multiplication between a colors and a scalar.
	friend Color4f operator * (float value,const Color4f & a);

	/// Multiplication between two colors.
	friend Color4f operator * (const Color4f & a, const Color4f & b);

	/// Subtraction between two colors.
	friend Color4f operator - (const Color4f & a, const Color4f & b);

	/// Sum between two colors.
	friend Color4f operator + (const Color4f & a, const Color4f & b);

	/// Get red value.
	inline float GetR() const{return m_R;}

	/// Get green value.
	inline float GetG() const{return m_G;}

	/// Get blue value.
	inline float GetB() const{return m_B;}

	/// Get alpha value.
	inline float GetA() const{return m_A;}


	/// Set red value.
	inline Color4f & SetR(float val){m_R = val;return *this;};

	/// Set green value.
	inline Color4f & SetG(float val){m_G = val;return *this;};

	/// Set blue value.
	inline Color4f & SetB(float val){m_B = val;return *this;};

	/// Set alpha value.
	inline Color4f & SetA(float val){m_A = val;return *this;};

	/// Get color string.
	std::string GetString();

private:

	/// Red value.
	float m_R;

	/// Green value.
	float m_G;

	/// Blue value.
	float m_B;
	
	/// Alpha value.
	float m_A;
};

Color4f operator * (const Color4f & a, float value);
Color4f operator * (float value,const Color4f & a);
Color4f operator * (const Color4f & a, const Color4f & b);
Color4f operator - (const Color4f & a, const Color4f & b);
Color4f operator + (const Color4f & a, const Color4f & b);

_MATH_END

#endif // #ifndef NCK_COLOR4_H
