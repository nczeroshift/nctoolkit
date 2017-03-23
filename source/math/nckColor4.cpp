
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckColor4.h"
#include <sstream>

_MATH_BEGIN

Color4ub::Color4ub(){
	m_R=0xff;m_G=0xff;m_B=0xff;m_A=0xff;
}

Color4ub::Color4ub(unsigned char r,unsigned char g,unsigned char b,unsigned char a){
	m_R=r;m_G=g;m_B=b;m_A=a;
}

Color4ub::Color4ub(const Color4ub & col, unsigned char a ){
	m_R=col.m_R;m_G=col.m_G;m_B=col.m_B;m_A=a;
}

Color4ub & Color4ub::operator = (const Color4ub & v){
	m_R = v.m_R;m_G = v.m_G;m_B = v.m_B;m_A = v.m_A;
	return *this;
}

Color4ub & Color4ub::operator += (const Color4ub & v){
	m_R += v.m_R;m_G += v.m_G;m_B += v.m_B;m_A += v.m_A;
	return *this;
}

Color4ub operator * (const Color4ub & a, float value){
	return Color4ub((unsigned char)(a.m_R * value),
					(unsigned char)(a.m_G * value),
					(unsigned char)(a.m_B * value),
					(unsigned char)(a.m_A * value));
}

Color4ub operator * (float value, const Color4ub & a){
	return Color4ub((unsigned char)(a.m_R * value),
					(unsigned char)(a.m_G * value),
					(unsigned char)(a.m_B * value),
					(unsigned char)(a.m_A * value));
}

Color4ub operator - (const Color4ub & a, const Color4ub & b){
	return Color4ub((unsigned char)(a.m_R - b.m_R),
					(unsigned char)(a.m_G - b.m_G),
					(unsigned char)(a.m_B - b.m_B),
					(unsigned char)(a.m_A - b.m_A));
}

Color4ub operator + (const Color4ub & a, const Color4ub & b){
	return Color4ub((unsigned char)(a.m_R + b.m_R),
			(unsigned char)(a.m_G + b.m_G),
			(unsigned char)(a.m_B + b.m_B),
			(unsigned char)(a.m_A + b.m_A));
}


/*inline Color4ub & Color4ub::SetR(unsigned char val){ m_R = val; return *this; }

inline Color4ub & Color4ub::SetG(unsigned char val){ m_G = val; return *this; }

inline Color4ub & Color4ub::SetB(unsigned char val){ m_B = val; return *this; }

inline Color4ub & Color4ub::SetA(unsigned char val){ m_A = val; return *this; }*/

std::string Color4ub::GetString(){
	std::stringstream sstream;
	sstream << m_R << m_G << m_B << m_A;
	return sstream.str();
}



Color4f::Color4f(){
	m_R=0;m_G=0;m_B=0;m_A=0;
}

Color4f::Color4f(float r,float g,float b,float a){
	m_R=r;m_G=g;m_B=b;m_A=a;
}

Color4f::Color4f(const Color4f & col ,float a){
	m_R=col.m_R;m_G=col.m_G;m_B=col.m_B;m_A=a;
}

Color4f & Color4f::operator = (const Color4f & v){
	m_R = v.m_R;m_G = v.m_G;m_B = v.m_B;m_A = v.m_A;
	return *this;
}

Color4f & Color4f::operator += (const Color4f & v){
	m_R += v.m_R;m_G += v.m_G;m_B += v.m_B;m_A += v.m_A;
	return *this;
}

Color4f operator * (const Color4f & a, float value){
	return Color4f(a.m_R * value,a.m_G * value,a.m_B * value,a.m_A * value);
}

Color4f operator * (float value,const Color4f & a){
	return Color4f(a.m_R * value,a.m_G * value,a.m_B * value,a.m_A * value);
}

Color4f operator * (const Color4f & a,const Color4f & b){
	return Color4f(a.m_R * b.m_R,a.m_G * a.m_G,a.m_B * a.m_B,a.m_A * a.m_A);
}

Color4f operator - (const Color4f & a, const Color4f & b){
	return Color4f(a.m_R - b.m_R,a.m_G - b.m_G,a.m_B - b.m_B,a.m_A - b.m_A);
}

Color4f operator + (const Color4f & a, const Color4f & b){
	return Color4f(a.m_R + b.m_R,a.m_G + b.m_G,a.m_B + b.m_B,a.m_A + b.m_A);
}
/*
inline Color4f & Color4f::SetR(float val){ m_R = val; return *this;}

inline Color4f & Color4f::SetG(float val){ m_G = val; return *this;}

inline Color4f & Color4f::SetB(float val){ m_B = val; return *this;}

inline Color4f & Color4f::SetA(float val){ m_A = val; return *this;}*/

std::string Color4f::GetString(){
	std::stringstream sstream;
	sstream << m_R << m_G << m_B << m_A;
	return sstream.str();
}


_MATH_END
