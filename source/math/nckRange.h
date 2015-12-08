
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_RANGE_H
#define NCK_RANGE_H

#include "nckMathConfig.h"

_MATH_BEGIN

class Range
{
public:
	/// Default constructor.
	Range (float min = 0.0, float max = 0.0){m_Min = min;m_Max = max;}

	inline float GetMin(){return m_Min;}
	inline float GetMax(){return m_Max;}
	inline void SetMin(float val){m_Min = val;}
	inline void SetMax(float val){m_Max = val;}

private:
	/// Min & Max range values.
	float m_Max,m_Min;
};

_MATH_END

#endif // #ifndef NCK_RANGE_H
