
/**
 * NCtoolKit © 2007-2015 Lu’s F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckMathUtils.h"
#include <stdlib.h>
#include <sstream>
#include <iomanip>

_MATH_BEGIN

float RandomValue(float min, float max){
	float fRandNum = (float)rand () / RAND_MAX;
    return min + (max - min) * fRandNum;
}

float Clamp(float min,float max,float val){
	if(val<min)
		return min;
	if(val>max)
		return max;
	return val;
}

float Signal(float val){
	if(val>0)
		return 1;
	else 
		return -1;
}

float Max(float a,float b){
	if(a>b)
		return a;
	else 
		return b;
}

float Min(float a,float b){
	if(a>b)
		return b;
	else 
		return a;
}

std::string FloatToString(float value){
	std::stringstream sstream;
	sstream << value;
	return sstream.str();
}

std::string FloatToString(float value, int decimal_precision){
	std::stringstream sstream;
	sstream << std::setprecision (decimal_precision) << value;
	return sstream.str();
}

std::string IntToString(int value){
	std::stringstream sstream;
	sstream << value;
	return sstream.str();
}

_MATH_END
