
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckChronometer.h"
#include <stdio.h>
#include <time.h>

#if defined(NCK_WINDOWS)
#include <windows.h>
#elif defined(NCK_LINUX) | defined(NCK_APPLE)
#include <sys/time.h>
#endif

_CORE_BEGIN

class Chronometer_LowRes : public Chronometer
{
public:
	Chronometer_LowRes(){
		m_Start=0;
		m_Stop=0;
	}

	virtual ~Chronometer_LowRes(){}

	void Clear(){
		m_Start=0;
		m_Stop=0;
	}

	void Start(){
		time(&m_Start);
	}

	void Stop(){
		time(&m_Stop);
	}

	double GetElapsedTime(){
		return (double)difftime (m_Stop,m_Start);
	}

private:
	time_t	m_Start;
	time_t	m_Stop;
};

#if defined(NCK_WINDOWS)

class Chronometer_HighRes : public Chronometer
{
public:
	Chronometer_HighRes(){
		Clear();
	}

	void Clear(){
		m_Running = false;
		m_Start.QuadPart=0;
		m_Stop.QuadPart=0;
	}

	void Start(){
		m_Running = true;
		QueryPerformanceCounter(&m_Start);
	}

	void Stop(){
		m_Running = false;
		QueryPerformanceCounter(&m_Stop);
	}

	double GetElapsedTime(){
		if(m_Running){
			LARGE_INTEGER thisInstant,freq;
			QueryPerformanceCounter(&thisInstant);
			QueryPerformanceFrequency(&freq);
			return 1000.0*(double)(thisInstant.QuadPart-m_Start.QuadPart)/(double)(freq.QuadPart);
		}
		else{
			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			return 1000.0*(double)(m_Stop.QuadPart-m_Start.QuadPart)/(double)(freq.QuadPart);
		}
	}
private:
	bool m_Running;
	LARGE_INTEGER	m_Start;
	LARGE_INTEGER	m_Stop;
};

#elif defined(NCK_LINUX) | defined(NCK_APPLE)

class Chronometer_HighRes : public Chronometer
{
public:
	Chronometer_HighRes(){
		Clear();
	}

	virtual ~Chronometer_HighRes(){}

	void Clear(){
		m_Running = false;
	}

	void Start(){
		m_Running = true;
		gettimeofday(&m_Start, NULL);
	}
	void Stop(){
		m_Running = false;
		gettimeofday(&m_Stop, NULL);
	}

	double GetElapsedTime(){
		if(m_Running){
			timeval thisInstant;
			gettimeofday(&thisInstant, NULL);
			long int diff = (thisInstant.tv_usec + 1000000 * thisInstant.tv_sec) -
										(m_Start.tv_usec + 1000000 * m_Start.tv_sec);
			return diff/1.0e3;
		}
		else{
			long int diff = (m_Stop.tv_usec + 1000000 * m_Stop.tv_sec) -
							(m_Start.tv_usec + 1000000 * m_Start.tv_sec);
			return diff/1.0e3;
		}
	}

private:
	bool m_Running;
	timeval m_Start;
	timeval m_Stop;
};

#endif // #elif defined(NCK_LINUX) | defined(NCK_APPLE)

Chronometer *CreateChronometer(bool high_res)
{
	if(high_res)
		return new Chronometer_HighRes();
	else 
		return new Chronometer_LowRes();
}

_CORE_END

