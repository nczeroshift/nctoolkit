
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
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
    	sstream << std::fixed << std::setprecision (decimal_precision) << value;
	return sstream.str();
}

std::string IntToString(int value){
	std::stringstream sstream;
	sstream << value;
	return sstream.str();
}

std::string IntToString(int value, int leading) {
    std::stringstream sstream;
    sstream << value;
    std::string v = sstream.str();
    int s = leading + 1 - v.size();
    if (s < 0)s = 0;
    return std::string(s,'0')+v;
}

std::string TimeToString(int64_t t) {
    int hours = t / (3600 * 1e6);
    int minutes = (int64_t)(t / (60 * 1e6)) % 60;
    int seconds = ((int64_t)(t / 1e6)) % 60;
    int milis = ((t / 1000) % 1000) / 10;
   return IntToString(hours, 1) + ":" + IntToString(minutes, 1) + ":" + IntToString(seconds, 1) + ":" + IntToString(milis, 1);
}

_MATH_END
