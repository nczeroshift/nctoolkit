
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckStatistics.h"

_MATH_BEGIN

Statistics::Statistics(int capacity){
	m_Values.reserve(capacity);
	m_Min = 0;
	m_Max = 0;
	m_Mean = 0;
	m_Updated = false;
	m_Index = 0;
	m_Capacity = capacity;
}

Statistics::~Statistics(){
	
}

int Statistics::GetCapacity(){
	return m_Capacity;
}

void Statistics::Compute(){
	if(!m_Updated)
		return;

	Sort();

	m_Min = 0;
	m_Max = 0;
	m_Mean = 0;
	
	for(int i = 0;i<m_Values.size();i++)
	{
		double v = m_Values[i];
		if(i == 0){
			m_Min = v;
			m_Max = v;
			m_Mean = 0;
		}
		else{
			if(v > m_Max)
				m_Max = v;
			if(v < m_Min)
				m_Min = v;
			m_Mean += v;
		}
	}

	if(m_Values.size()>0)
		m_Mean /= m_Values.size();

	m_Updated = false;
}

void Statistics::Sort(){

}

void Statistics::Add(double val, bool rolling)
{
	m_Updated = true;
	if(rolling && m_Values.size() >= m_Values.capacity()){
		m_Values[m_Index++] = val;
		if(m_Index >= m_Values.capacity())
			m_Index = 0;
	}
	else
		m_Values.push_back(val);
}

void Statistics::Clear(){
	m_Updated = false;
	m_Values.clear();
}

int Statistics::GetN(){
	return m_Values.size();
}

double Statistics::GetMean(){
	Compute();
	return m_Mean;
}

double Statistics::GetMin(){
	Compute();
	return m_Min;
}

double Statistics::GetMax(){
	Compute();
	return m_Max;
}

_MATH_END