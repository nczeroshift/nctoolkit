
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
        m_Running = false;
	}

	virtual ~Chronometer_LowRes(){}

	void Clear(){
		m_Start=0;
		m_Stop=0;
        m_Running = false;
	}

	void Start(){
		time(&m_Start);
        m_Running = true;
	}

	void Stop(){
        m_Running = false;
		time(&m_Stop);
	}

	double GetElapsedTime(){
        time(&m_Stop);
		return (double)difftime (m_Stop,m_Start)*1e6;
	}

    bool IsRunning() {
        return m_Running;
    }

private:
    bool m_Running;
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
		QueryPerformanceCounter(&m_Start);
        m_Running = true;
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
			return 1e6*(double)(thisInstant.QuadPart-m_Start.QuadPart)/(double)(freq.QuadPart);
		}
		else{
			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			return 1e6*(double)(m_Stop.QuadPart-m_Start.QuadPart)/(double)(freq.QuadPart);
		}
	}

    bool IsRunning() {
        return m_Running;
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
    
    bool IsRunning() {
        return IsRunning;
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

