
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckTriangle.h"

_MATH_BEGIN

Triangle::Triangle(){
}

Triangle::Triangle(const Vec3 & a,const Vec3 & b,const Vec3 & c){
	m_A = a;m_B = b;m_C = c;
}

bool CheckPointInside(const Triangle & t,
	const Vec3 & p, 
	float *lambda_1,
	float *lambda_2){
	Vec3 V0 = p - t.m_C;
	Vec3 V1 = t.m_A - t.m_C;
	Vec3 V2 = t.m_B - t.m_C;

	float DOT22 = Dot(V2,V2);
	float DOT11 = Dot(V1,V1);
	float DOT21 = Dot(V2,V1);
	float DOT02 = Dot(V0,V2);
	float DOT01 = Dot(V0,V1);
	
	float inv_denominator = 1.0F/(DOT22*DOT11 - DOT21*DOT21);

	*lambda_1 = (DOT22*DOT01 - DOT21*DOT02)* inv_denominator;
	*lambda_2 = (DOT11*DOT02 - DOT21*DOT01)* inv_denominator;

	float lambda_3 = 1.0F -* lambda_1 - *lambda_2;

	if( *lambda_1 >= 0 && *lambda_2 >= 0 && lambda_3 >= 0)
		return true;

	return false;
}

_MATH_END
