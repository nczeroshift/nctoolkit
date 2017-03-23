
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_MATHUTILS_H
#define NCK_MATHUTILS_H

#include "nckMathConfig.h"
#include <string>
#include <stdint.h>

_MATH_BEGIN

/// Get a random value.
float RandomValue(float min = 0.0f, float max = 1.0f);

/// Limit value between a minimum and maximum value.
float Clamp(float min,float max,float val);

/// Get value signal.
float Signal(float val);

/// Get the maximum value between two.
float Max(float a,float b);

/// Get the minimum value between two.
float Min(float a,float b);

/// Convert single precision float to string.
std::string FloatToString(float value);

/// Convert single precision float with a specific decimal precision to string.
std::string FloatToString(float value, int decimal_precision);

/// Convert integer to string.
std::string IntToString(int value);
std::string IntToString(int value, int leading);
std::string TimeToString(int64_t v);

_MATH_END

#endif // #ifndef NCK_MATHUTILS_H
