
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef _NCK_STATISTICS_
#define _NCK_STATISTICS_

#include "nckMathConfig.h"
#include <vector>

_MATH_BEGIN

class Statistics{
public:
	Statistics(int capacity);
	~Statistics();

	void Add(double val, bool rolling = false);
	void Clear();

	int GetN();
	int GetCapacity();

	double GetMean();
	double GetMin();
	double GetMax();
private:
	void Compute();
	void Sort();
	int m_Index;
	int m_Capacity;
	std::vector<double> m_Values;
	double m_Mean;
	double m_Min;
	double m_Max;
	bool m_Updated;
};

_MATH_END

#endif